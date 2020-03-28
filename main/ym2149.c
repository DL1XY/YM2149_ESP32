/*
 * ym2149f.c
 *
 *  Created on: 14.03.2020
 *      Author: DL1XY
 */



#include <ym2149.h>
#include "esp_log.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "spi_74hc595.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"

static const char *TAG = "YM2149";

volatile QueueHandle_t cmd_queue;


volatile ym2149 ym2149_configuration;
volatile ym219_register ym219_register_status;
volatile ym2149_command current_command;
volatile uint8_t currentCmdState;
volatile uint8_t lastCmdState;


void YM2149_init()
{
	 ESP_LOGE(TAG, "Init YM2149");

	 // Init Command Queue
	 cmd_queue = xQueueCreate (16, sizeof (struct ym2149_command));
	 currentCmdState = YM2149_COMMAND_STATE_IDLE;

	 gpio_config_t io_conf;
	 //disable interrupt
	 io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
	 //set as output mode
	 io_conf.mode = GPIO_MODE_OUTPUT;
	 //bit mask of the pins that you want to set,e.g.GPIO18/19
	 io_conf.pin_bit_mask =
			 ((1ULL<<YM2149_RESET_GPIO)	|
			 (1ULL<<YM2149_CLOCK_GPIO) 	|
			 (1ULL<<YM2149_BC1_GPIO)	|
			 (1ULL<<YM2149_BCDIR_GPIO)	|
			 (1ULL<<YM2149_DA0_GPIO)	|
			 (1ULL<<YM2149_DA1_GPIO)	|
		 	 (1ULL<<YM2149_DA2_GPIO)	|
		 	 (1ULL<<YM2149_DA3_GPIO)	|
		 	 (1ULL<<YM2149_DA4_GPIO)	|
		 	 (1ULL<<YM2149_DA5_GPIO)	|
		 	 (1ULL<<YM2149_DA6_GPIO)	|
		 	 (1ULL<<YM2149_DA7_GPIO)	|
			 (1ULL<<SPI_74HC595_GPIO_OE)); // HACK to avoid Startup troubles
	 //disable pull-down mode
	 io_conf.pull_down_en = 0;
	 //disable pull-up mode
	 io_conf.pull_up_en = 0;
	 //configure GPIO with the given settings
	 gpio_config(&io_conf);


	 // Set GPIO defaults
	 gpio_set_level(YM2149_BC1_GPIO, 0);
	 gpio_set_level(YM2149_BCDIR_GPIO, 0);
	 gpio_set_level(YM2149_RESET_GPIO, 0);

	 gpio_set_level(YM2149_DA0_GPIO, 0);
	 gpio_set_level(YM2149_DA1_GPIO, 0);
	 gpio_set_level(YM2149_DA2_GPIO, 0);
	 gpio_set_level(YM2149_DA3_GPIO, 0);
	 gpio_set_level(YM2149_DA4_GPIO, 0);
	 gpio_set_level(YM2149_DA5_GPIO, 0);
	 gpio_set_level(YM2149_DA6_GPIO, 0);
	 gpio_set_level(YM2149_DA7_GPIO, 0);

	 gpio_set_level(SPI_74HC595_GPIO_OE, 0);

	 // Init PWM clock
	 YM2149_init_pwm();
	 YM2149_init_timer();

}


void YM2149_init_pwm()
{
	ESP_LOGE(TAG, "Init YM2149 PWM");
	ledc_timer_config_t ledc_timer = {
	    .speed_mode = LEDC_HIGH_SPEED_MODE,
	    .timer_num  = LEDC_TIMER_0,
	    .freq_hz    = 1000000
	};

	ledc_channel_config_t ledc_channel = {
	    .channel    = LEDC_CHANNEL_0,
	    .gpio_num   = YM2149_CLOCK_GPIO,
	    .speed_mode = LEDC_HIGH_SPEED_MODE,
	    .timer_sel  = LEDC_TIMER_0,
	    .duty       = 2
	};
	ledc_timer_config(&ledc_timer);
	ledc_channel_config(&ledc_channel);
}

void YM2149_init_timer()
{
	ESP_LOGE(TAG, "Init YM2149 Timer Scale:%d Divider:%d", YM2149_TIMER_SCALE, YM2149_TIMER_DIVIDER);
	timer_config_t config;
	config.divider = YM2149_TIMER_DIVIDER;
	config.counter_dir = TIMER_COUNT_UP;
	config.counter_en = TIMER_PAUSE;
	config.alarm_en = TIMER_ALARM_EN;
	config.intr_type = TIMER_INTR_LEVEL;
	config.auto_reload = 1;
	#ifdef TIMER_GROUP_SUPPORTS_XTAL_CLOCK
	    config.clk_src = TIMER_SRC_CLK_APB;
	#endif
	timer_init(TIMER_GROUP_0, TIMER_0, &config);

	/* Timer's counter will initially start from value below.
	   Also, if auto_reload is set, this value will be automatically reload on alarm */
	timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

	/* Configure the alarm value and the interrupt on alarm. */
	timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 80); // Use YM2149_TIMER_SCALE to get 1sec delay
	timer_enable_intr(TIMER_GROUP_0, TIMER_0);
	timer_isr_register(TIMER_GROUP_0, TIMER_0, YM2149_isrHandler, (void *) TIMER_0, ESP_INTR_FLAG_IRAM, NULL);
	ESP_LOGE(TAG, "Init YM2149 Timer done, now start ...");
	timer_start(TIMER_GROUP_0, TIMER_0);
	ESP_LOGE(TAG, "Init YM2149 Timer started");
}

void IRAM_ATTR YM2149_cmdHandler()
{
	switch (currentCmdState)
	{
		case YM2149_COMMAND_STATE_INIT:
		{
			gpio_set_level(YM2149_BC1_GPIO, 0);
			gpio_set_level(YM2149_BCDIR_GPIO, 0);
			gpio_set_level(YM2149_RESET_GPIO, 0);
			lastCmdState = currentCmdState;
			currentCmdState = YM2149_COMMAND_STATE_IDLE;

		}
			break;
		case YM2149_COMMAND_STATE_IDLE:
		{
			gpio_set_level(YM2149_BC1_GPIO, 0);
			gpio_set_level(YM2149_BCDIR_GPIO, 0);
			lastCmdState = currentCmdState;
			if (uxQueueMessagesWaiting(cmd_queue) > 0)
			{
				xQueueReceiveFromISR (cmd_queue, &current_command, 0 );
				currentCmdState = YM2149_COMMAND_STATE_ADDR_MODE;
			}
		}
		break;
		case YM2149_COMMAND_STATE_ADDR_MODE:
		{
			gpio_set_level(YM2149_BC1_GPIO, 1);
			gpio_set_level(YM2149_BCDIR_GPIO, 1);

			gpio_set_level(YM2149_DA0_GPIO, (current_command.register_addr & (1 << 0)));
			gpio_set_level(YM2149_DA1_GPIO, (current_command.register_addr & (1 << 1)));
			gpio_set_level(YM2149_DA2_GPIO, (current_command.register_addr & (1 << 2)));
			gpio_set_level(YM2149_DA3_GPIO, (current_command.register_addr & (1 << 3)));
			gpio_set_level(YM2149_DA4_GPIO, 0);
			gpio_set_level(YM2149_DA5_GPIO, 0);
			gpio_set_level(YM2149_DA6_GPIO, 0);
			gpio_set_level(YM2149_DA7_GPIO, 0);
			lastCmdState = currentCmdState;
			currentCmdState = YM2149_COMMAND_STATE_WRITE_MODE;
		}
		break;
		case YM2149_COMMAND_STATE_WRITE_MODE:
		{
			gpio_set_level(YM2149_BC1_GPIO, 0);
			gpio_set_level(YM2149_BCDIR_GPIO, 1);

			gpio_set_level(YM2149_DA0_GPIO, (current_command.register_value & (1 << 0)));
			gpio_set_level(YM2149_DA1_GPIO, (current_command.register_value & (1 << 1)));
			gpio_set_level(YM2149_DA2_GPIO, (current_command.register_value & (1 << 2)));
			gpio_set_level(YM2149_DA3_GPIO, (current_command.register_value & (1 << 3)));
			gpio_set_level(YM2149_DA4_GPIO, (current_command.register_value & (1 << 4)));
			gpio_set_level(YM2149_DA5_GPIO, (current_command.register_value & (1 << 5)));
			gpio_set_level(YM2149_DA6_GPIO, (current_command.register_value & (1 << 6)));
			gpio_set_level(YM2149_DA7_GPIO, (current_command.register_value & (1 << 7)));
			lastCmdState = currentCmdState;
			currentCmdState = YM2149_COMMAND_STATE_IDLE;
		}
		break;
		case YM2149_COMMAND_STATE_CLEANUP:
		{
			// TODO
		}
		break;
		case YM2149_COMMAND_STATE_RESET:
		{
			gpio_set_level(YM2149_RESET_GPIO, 0);
			xQueueReset(cmd_queue);
			lastCmdState = currentCmdState;
			currentCmdState = YM2149_COMMAND_STATE_INIT;
		}
		break;
	}
}
void IRAM_ATTR YM2149_isrHandler(void *pvParameter)
{
	// ATTENTION: Don't print logs in ISR methods !!!
	// @see https://esp32.com/viewtopic.php?t=3748

	// Examples
	// https://esp32developer.com/programming-in-c-c/timing/hardware-timers
	YM2149_cmdHandler();

	timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
}

void YM2149_reset()
{
	//currentCmdState = YM2149_COMMAND_STATE_RESET;
}
void YM2149_playChannel (uint8_t* channel)
{
	ESP_LOGE(TAG, "playChannel(%d)", *channel);
	uint8_t full= 0xff;
	YM2149_setChannelLevel(channel, &full);
}

void YM2149_stopChannel (uint8_t* channel)
{
	ESP_LOGE(TAG, "stopChannel(%d)", *channel);
	YM2149_setChannelLevel(channel, 0);
}

void YM2149_setChannelFreqFine(uint8_t* channel, uint8_t* value)
{
	 ESP_LOGE(TAG, "setChannelFreqFine(%d, %d)", *channel, *value);
	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_CHANNEL_FREQ_FINE;
	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.register_addr = YM2149_REG_0_ADDR;
		 ym219_register_status.reg_0 = *value;
		 ym2149_configuration.channel_a_freq_fine = *value;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.register_addr = YM2149_REG_2_ADDR;
		 ym219_register_status.reg_2 = *value;
		 ym2149_configuration.channel_b_freq_fine = *value;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.register_addr = YM2149_REG_4_ADDR;
		 ym219_register_status.reg_4 = *value;
		 ym2149_configuration.channel_c_freq_fine = *value;
	 	 break;
	 }

	 cmd.register_value = *value;
	 cmd.bit_length = 8;
	 cmd.bit_start = 0;


	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setChannelFreqFine CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);

#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif

}

void YM2149_setChannelFreqRough(uint8_t* channel, uint8_t* value)
{
	 ESP_LOGE(TAG, "setChannelFreqRough(%d, %d)", *channel, *value);

	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_CHANNEL_FREQ_ROUGH;
	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.register_addr = YM2149_REG_1_ADDR;
		 ym219_register_status.reg_1 = *value;
		 ym2149_configuration.channel_a_freq_rough = *value & 0x0F;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.register_addr = YM2149_REG_3_ADDR;
		 ym219_register_status.reg_3 = *value;
		 ym2149_configuration.channel_b_freq_rough = *value & 0x0F;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.register_addr = YM2149_REG_5_ADDR;
		 ym219_register_status.reg_5 = *value;
		 ym2149_configuration.channel_c_freq_rough = *value & 0x0F;
		 break;
	 }

	 cmd.register_value = *value;
	 cmd.bit_length = 4;
	 cmd.bit_start = 0;

	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setChannelFreqRough CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);

#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif
}

void YM2149_setNoiseFreq(uint8_t* value)
{
	 ESP_LOGE(TAG, "setNoiseFreq( %d)", *value);
	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_NOISE_FREQ;
	 cmd.register_addr = YM2149_REG_6_ADDR;
	 cmd.register_value = *value;
	 cmd.bit_length = 5;
	 cmd.bit_start = 0;

	 ym219_register_status.reg_6 = *value;
	 ym2149_configuration.noise_freq = *value;

	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setNoiseFreq CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);
#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif

}

void YM2149_setChannelNoise(uint8_t* channel, bool* value)
{
	 ESP_LOGE(TAG, "setChannelNoise(%d, %d)", *channel, *value);
	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_NOISE;
	 cmd.register_addr = YM2149_REG_7_ADDR;
	 cmd.register_value = *value;
	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.bit_start = YM2149_NOISE_CHANNEL_A_BIT;
		 ym219_register_status.reg_7 |= *value <<  YM2149_NOISE_CHANNEL_A_BIT;
		 ym2149_configuration.channel_a_noise = *value;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.bit_start = YM2149_NOISE_CHANNEL_B_BIT;
		 ym219_register_status.reg_7 |= *value <<  YM2149_NOISE_CHANNEL_B_BIT;
		 ym2149_configuration.channel_b_noise = *value;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.bit_start = YM2149_NOISE_CHANNEL_C_BIT;
		 ym219_register_status.reg_7 |= *value <<  YM2149_NOISE_CHANNEL_C_BIT;
		 ym2149_configuration.channel_c_noise = *value;
		 break;
	 }
	 cmd.bit_length = 1;

	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setChannelNoise CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);
#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif

}

void YM2149_setChannelTone(uint8_t* channel, bool* value)
{
	ESP_LOGE(TAG, "setTone(%d, %d)", *channel, *value);
	ym2149_command cmd;
	cmd.command_id = YM2149_CMD_ID_SET_TONE;
	cmd.register_addr = YM2149_REG_7_ADDR;
	cmd.register_value = *value;
	switch (*channel)
	{
	case YM2149_CHANNEL_A:
		cmd.bit_start = YM2149_TONE_CHANNEL_A_BIT;
		ym219_register_status.reg_7 |= *value <<  YM2149_TONE_CHANNEL_A_BIT;
		ym2149_configuration.channel_a_tone = *value;
		break;
	case YM2149_CHANNEL_B:
		ym219_register_status.reg_7 |= *value <<  YM2149_TONE_CHANNEL_B_BIT;
		ym2149_configuration.channel_b_tone = *value;
		cmd.bit_start = YM2149_TONE_CHANNEL_B_BIT;
		break;
	case YM2149_CHANNEL_C:
		ym219_register_status.reg_7 |= *value <<  YM2149_TONE_CHANNEL_C_BIT;
		ym2149_configuration.channel_c_tone = *value;
		cmd.bit_start = YM2149_TONE_CHANNEL_C_BIT;
		break;
	}
	cmd.bit_length = 1;

	xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	ESP_LOGE(TAG, "setTone CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);
#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif

}

void YM2149_setChannelLevelMode(uint8_t* channel, bool* value)
{
	 ESP_LOGE(TAG, "setLevelMode(%d, %d)", *channel, *value);
	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_LEVEL_MODE;

	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.register_addr = YM2149_REG_8_ADDR;
		 ym219_register_status.reg_8 |= *value <<  YM2149_LEVEL_MODE_BIT;
		 ym2149_configuration.channel_a_amp_mode = *value;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.register_addr = YM2149_REG_9_ADDR;
		 ym219_register_status.reg_9 |= *value <<  YM2149_LEVEL_MODE_BIT;
		 ym2149_configuration.channel_b_amp_mode = *value;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.register_addr = YM2149_REG_A_ADDR;
		 ym219_register_status.reg_A |= *value <<  YM2149_LEVEL_MODE_BIT;
		 ym2149_configuration.channel_c_amp_mode = *value;
		 break;
	 }

	 cmd.register_value = *value;
	 cmd.bit_length = 1;
	 cmd.bit_start = YM2149_LEVEL_MODE_BIT;

	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setLevelMode CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);
#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif

}

void YM2149_setChannelLevel(uint8_t* channel, uint8_t* value)
{
	 ESP_LOGE(TAG, "setLevel(%d, %d)", *channel, *value);
	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_LEVEL;
	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.register_addr = YM2149_REG_8_ADDR;
		 ym219_register_status.reg_8 |= *value & 0x0F;
		 ym2149_configuration.channel_a_level = *value & 0x0F;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.register_addr = YM2149_REG_9_ADDR;
		 ym219_register_status.reg_9 |= *value & 0x0F;
		 ym2149_configuration.channel_b_level = *value & 0x0F;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.register_addr = YM2149_REG_A_ADDR;
		 ym219_register_status.reg_A |= *value & 0x0F;
		 ym2149_configuration.channel_c_level = *value & 0x0F;
		 break;
	 }

	 cmd.register_value = *value;
	 cmd.bit_length = 4;
	 cmd.bit_start = 0;

	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setLevel CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);
#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif
}

void YM2149_setEnvelopeFreqFine(uint8_t* value)
{
	 ESP_LOGE(TAG, "setEnvelopeFreqFine(%d)", *value);
	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_ENVELOPE_FREQ_FINE;
	 cmd.register_addr = YM2149_REG_B_ADDR;
	 cmd.register_value = *value;
	 cmd.bit_length = 8;
	 cmd.bit_start = 0;

	 ym219_register_status.reg_B = *value;
	 ym2149_configuration.envelope_fine_freq = *value;

	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setEnvelopeFreqFine CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);
#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif

}

void YM2149_setEnvelopeFreqRough(uint8_t* value)
{
	 ESP_LOGE(TAG, "setEnvelopeFreqRough(%d)", *value);
	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_ENVELOPE_FREQ_ROUGH;
	 cmd.register_addr = YM2149_REG_C_ADDR;
	 cmd.register_value = *value;
	 cmd.bit_length = 8;
	 cmd.bit_start = 0;

	 ym219_register_status.reg_C = *value;
	 ym2149_configuration.envelope_rough_freq = *value;

	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setEnvelopeFreqRough CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);
#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif

}

void YM2149_setEnvelopeShape(uint8_t* env_shape_type, bool* value)
{
	 ESP_LOGE(TAG, "setEnvelopeShape(%d, %d)", *env_shape_type, *value);
	 ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_ENVELOPE_SHAPE;
	 cmd.register_addr = YM2149_REG_D_ADDR;
	 cmd.register_value = *value;
	 cmd.bit_length = 1;
	 cmd.bit_start = *env_shape_type ;

	 ym219_register_status.reg_D |= *value << *env_shape_type;

	 switch (*env_shape_type)
	 {
	 case 0:	// HOLD
		 ym2149_configuration.envelope_shape_hold = *value;
		 break;
	 case 1:	// ALT
		 ym2149_configuration.envelope_shape_alt = *value;
 		 break;
	 case 2:	// ATT
		 ym2149_configuration.envelope_shape_att = *value;
 		 break;
	 case 3:	// CONT
		 ym2149_configuration.envelope_shape_cont = *value;
 		 break;
	 }
	 xQueueSend( cmd_queue, &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setEnvelopeShape CMD cmd_id:%d register_addr:%d register_value:%d bit_start:%d bit_length:%d", cmd.command_id, cmd.register_addr, cmd.register_value, cmd.bit_start, cmd.bit_length);
#ifdef DEBUG_OUTPUT
	 debugReg();
	 debugConf();
#endif

}

void debug()
{
	ESP_LOGE(TAG, "\n");
	ESP_LOGE(TAG, "DEBUG GPIO States");
	ESP_LOGE(TAG, "YM2149_BC1_GPIO: %d", gpio_get_level(YM2149_BC1_GPIO));
	ESP_LOGE(TAG, "YM2149_BCDIR_GPIO: %d", gpio_get_level(YM2149_BCDIR_GPIO));
	ESP_LOGE(TAG, "YM2149_RESET_GPIO: %d", gpio_get_level(YM2149_RESET_GPIO));

	ESP_LOGE(TAG, "YM2149_DA0_GPIO: %d", gpio_get_level(YM2149_DA0_GPIO));
	ESP_LOGE(TAG, "YM2149_DA1_GPIO: %d", gpio_get_level(YM2149_DA1_GPIO));
	ESP_LOGE(TAG, "YM2149_DA2_GPIO: %d", gpio_get_level(YM2149_DA2_GPIO));
	ESP_LOGE(TAG, "YM2149_DA3_GPIO: %d", gpio_get_level(YM2149_DA3_GPIO));
	ESP_LOGE(TAG, "YM2149_DA4_GPIO: %d", gpio_get_level(YM2149_DA4_GPIO));
	ESP_LOGE(TAG, "YM2149_DA5_GPIO: %d", gpio_get_level(YM2149_DA5_GPIO));
	ESP_LOGE(TAG, "YM2149_DA6_GPIO: %d", gpio_get_level(YM2149_DA6_GPIO));
	ESP_LOGE(TAG, "YM2149_DA7_GPIO: %d", gpio_get_level(YM2149_DA7_GPIO));
	ESP_LOGE(TAG, "\n");
}

void debugCmd()
{
	ESP_LOGE(TAG, "\n");
	ESP_LOGE(TAG, "DEBUG CURRENT COMMAND size:%d", sizeof(current_command));
	ESP_LOGE(TAG, "command_id: %d", current_command.command_id);
	ESP_LOGE(TAG, "register_addr: %d", current_command.register_addr);
	ESP_LOGE(TAG, "register_value: %d", current_command.register_value);
	ESP_LOGE(TAG, "bit_start: %d", current_command.bit_start);
	ESP_LOGE(TAG, "bit_length: %d", current_command.bit_length);
	ESP_LOGE(TAG, "\n");
}

void debugReg()
{
	ESP_LOGE(TAG, "\n");
	ESP_LOGE(TAG, "DEBUG REGISTER");
	ESP_LOGE(TAG, "##########################");
	ESP_LOGE(TAG, "REG 0: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_0), ym219_register_status.reg_0);
	ESP_LOGE(TAG, "REG 1: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_1), ym219_register_status.reg_1);
	ESP_LOGE(TAG, "REG 2: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_2), ym219_register_status.reg_2);
	ESP_LOGE(TAG, "REG 3: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_3), ym219_register_status.reg_3);
	ESP_LOGE(TAG, "REG 4: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_4), ym219_register_status.reg_4);
	ESP_LOGE(TAG, "REG 5: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_5), ym219_register_status.reg_5);
	ESP_LOGE(TAG, "REG 6: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_6), ym219_register_status.reg_6);
	ESP_LOGE(TAG, "REG 7: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_7), ym219_register_status.reg_7);
	ESP_LOGE(TAG, "REG 8: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_8), ym219_register_status.reg_8);
	ESP_LOGE(TAG, "REG 9: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_9), ym219_register_status.reg_9);
	ESP_LOGE(TAG, "REG A: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_A), ym219_register_status.reg_A);
	ESP_LOGE(TAG, "REG B: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_B), ym219_register_status.reg_B);
	ESP_LOGE(TAG, "REG C: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_C), ym219_register_status.reg_C);
	ESP_LOGE(TAG, "REG D: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_D), ym219_register_status.reg_D);
	ESP_LOGE(TAG, "REG E: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_E), ym219_register_status.reg_E);
	ESP_LOGE(TAG, "REG F: "BYTE_TO_BINARY_PATTERN" %d", BYTE_TO_BINARY(ym219_register_status.reg_F), ym219_register_status.reg_F);
	ESP_LOGE(TAG, "##########################");
}
void debugConf()
{
	ESP_LOGE(TAG, "\n");
	ESP_LOGE(TAG, "DEBUG CONF");
	ESP_LOGE(TAG, "##########################");
	ESP_LOGE(TAG, "channel_a_freq_fine:%d", ym2149_configuration.channel_a_freq_fine);
	ESP_LOGE(TAG, "channel_a_freq_rough:%d", ym2149_configuration.channel_a_freq_rough);
	ESP_LOGE(TAG, "channel_b_freq_fine:%d", ym2149_configuration.channel_b_freq_fine);
	ESP_LOGE(TAG, "channel_b_freq_rough:%d", ym2149_configuration.channel_b_freq_rough);
	ESP_LOGE(TAG, "channel_c_freq_fine:%d", ym2149_configuration.channel_c_freq_fine);
	ESP_LOGE(TAG, "channel_c_freq_rough:%d", ym2149_configuration.channel_c_freq_rough);

	ESP_LOGE(TAG, "noise_freq:%d", ym2149_configuration.noise_freq);

	ESP_LOGE(TAG, "channel_a_noise:%d", ym2149_configuration.channel_a_noise);
	ESP_LOGE(TAG, "channel_b_noise:%d", ym2149_configuration.channel_b_noise);
	ESP_LOGE(TAG, "channel_c_noise:%d", ym2149_configuration.channel_c_noise);

	ESP_LOGE(TAG, "channel_a_tone:%d", ym2149_configuration.channel_a_tone);
	ESP_LOGE(TAG, "channel_b_tone:%d", ym2149_configuration.channel_b_tone);
	ESP_LOGE(TAG, "channel_c_tone:%d", ym2149_configuration.channel_c_tone);

	ESP_LOGE(TAG, "channel_a_amp_mode:%d", ym2149_configuration.channel_a_amp_mode);
	ESP_LOGE(TAG, "channel_b_amp_mode:%d", ym2149_configuration.channel_b_amp_mode);
	ESP_LOGE(TAG, "channel_c_amp_mode:%d", ym2149_configuration.channel_c_amp_mode);

	ESP_LOGE(TAG, "channel_a_level:%d", ym2149_configuration.channel_a_level);
	ESP_LOGE(TAG, "channel_b_level:%d", ym2149_configuration.channel_b_level);
	ESP_LOGE(TAG, "channel_c_level:%d", ym2149_configuration.channel_c_level);

	ESP_LOGE(TAG, "envelope_fine_freq:%d", ym2149_configuration.envelope_fine_freq);
	ESP_LOGE(TAG, "envelope_rough_freq:%d", ym2149_configuration.envelope_rough_freq);

	ESP_LOGE(TAG, "envelope_shape_cont:%d", ym2149_configuration.envelope_shape_cont);
	ESP_LOGE(TAG, "envelope_shape_att:%d", ym2149_configuration.envelope_shape_att);
	ESP_LOGE(TAG, "envelope_shape_alt:%d", ym2149_configuration.envelope_shape_alt);
	ESP_LOGE(TAG, "envelope_shape_hold:%d", ym2149_configuration.envelope_shape_hold);
	ESP_LOGE(TAG, "##########################");
}
