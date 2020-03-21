/*
 * ym2149f.c
 *
 *  Created on: 14.03.2020
 *      Author: arnew
 */


#include <ym2149.h>
#include "esp_log.h"
#include "driver/ledc.h"
#include "esp_err.h"

static const char *TAG = "YM2149";

static QueueHandle_t cmd_queue;

static struct ym2149 ym2149_configuration;
static struct ym219_register ym219_status;
static struct  ym2149_command current_command;
static uint8_t currentCmdState;
static uint8_t clockValue;

void YM2149_init()
{
	 ESP_LOGE(TAG, "Init YM2149");

	 // Init Control GPIOs
	 gpio_set_direction(YM2149_RESET_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_CLOCK_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_BC1_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_BCDIR_GPIO, GPIO_MODE_OUTPUT);

	 // Init Data GPIOs
	 gpio_set_direction(YM2149_DA0_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_DA1_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_DA2_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_DA3_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_DA4_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_DA5_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_DA6_GPIO, GPIO_MODE_OUTPUT);
	 gpio_set_direction(YM2149_DA7_GPIO, GPIO_MODE_OUTPUT);

	 // Set GPIO defaults
	 gpio_set_level(YM2149_BC1_GPIO, 0);
	 gpio_set_level(YM2149_BCDIR_GPIO, 0);

	 // Init PWM clock
	 YM2149_init_pwm();

	 // Init Command Queue
	 cmd_queue = xQueueCreate (16, sizeof (struct ym2149_command *));
	 currentCmdState = YM2149_COMMAND_STATE_IDLE;

	 // Create Task
	 clockValue = YM2149_CLOCK_DIVIDER;
	 TaskHandle_t xHandle = NULL;
	 xTaskCreate( YM2149_loop, "YM2149_Task", 10000, NULL, 1, &xHandle );
	 if( xHandle != NULL )
	 {
	  vTaskDelete( xHandle );
	 }

}

void YM2149_init_pwm()
{
	ledc_timer_config_t ledc_timer = {
	    .speed_mode = LEDC_HIGH_SPEED_MODE,
	    .timer_num  = LEDC_TIMER_0,
	    .bit_num    = YM2149_CLOCK_GPIO,
	    .freq_hz    = 1000000
	};

	ledc_channel_config_t ledc_channel = {
	    .channel    = LEDC_CHANNEL_0,
	    .gpio_num   = 18,
	    .speed_mode = LEDC_HIGH_SPEED_MODE,
	    .timer_sel  = LEDC_TIMER_0,
	    .duty       = 2
	};
	ledc_timer_config(&ledc_timer);
	ledc_channel_config(&ledc_channel);
}
void YM2149_loop()
{

	while(1)
	{
		if (clockValue % YM2149_CLOCK_DIVIDER == 0)
		{
			clockValue = 0;
			switch (currentCmdState)
			{
			case YM2149_COMMAND_STATE_INIT:
				gpio_set_level(YM2149_BC1_GPIO, 0);
				gpio_set_level(YM2149_BCDIR_GPIO, 0);
				gpio_set_level(YM2149_RESET_GPIO, 1);
				currentCmdState = YM2149_COMMAND_STATE_IDLE;
				break;
			case YM2149_COMMAND_STATE_IDLE:
				gpio_set_level(YM2149_BC1_GPIO, 0);
				gpio_set_level(YM2149_BCDIR_GPIO, 0);
				if (uxQueueMessagesWaiting(cmd_queue) > 0)
				{
					xQueueReceive (cmd_queue, &current_command, 0 );
					currentCmdState = YM2149_COMMAND_STATE_ADDR_MODE;
				}
				break;
			case YM2149_COMMAND_STATE_ADDR_MODE:
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
				currentCmdState = YM2149_COMMAND_STATE_WRITE_MODE;
				break;
			case YM2149_COMMAND_STATE_WRITE_MODE:
				gpio_set_level(YM2149_BC1_GPIO, 1);
				gpio_set_level(YM2149_BCDIR_GPIO, 1);

				gpio_set_level(YM2149_DA0_GPIO, (current_command.register_value & (1 << 0)));
				gpio_set_level(YM2149_DA1_GPIO, (current_command.register_value & (1 << 1)));
				gpio_set_level(YM2149_DA2_GPIO, (current_command.register_value & (1 << 2)));
				gpio_set_level(YM2149_DA3_GPIO, (current_command.register_value & (1 << 3)));
				gpio_set_level(YM2149_DA4_GPIO, (current_command.register_value & (1 << 4)));
				gpio_set_level(YM2149_DA5_GPIO, (current_command.register_value & (1 << 5)));
				gpio_set_level(YM2149_DA6_GPIO, (current_command.register_value & (1 << 6)));
				gpio_set_level(YM2149_DA7_GPIO, (current_command.register_value & (1 << 7)));
				currentCmdState = YM2149_COMMAND_STATE_IDLE;
				break;
			case YM2149_COMMAND_STATE_CLEANUP:
				break;
			case YM2149_COMMAND_STATE_RESET:
					gpio_set_level(YM2149_RESET_GPIO, 0);
					xQueueReset(cmd_queue);
					currentCmdState = YM2149_COMMAND_STATE_INIT;
				break;
			}
		}
		++clockValue;
	}
}

void YM2149_reset()
{
	currentCmdState = YM2149_COMMAND_STATE_RESET;
}
void YM2149_playChannel (uint8_t* channel)
{
	ESP_LOGE(TAG, "playChannel(%d)", *channel);
	YM2149_setChannelLevel(channel, 0xff);
}

void YM2149_stopChannel (uint8_t* channel)
{
	ESP_LOGE(TAG, "stopChannel(%d)", *channel);
	YM2149_setChannelLevel(channel, 0);
}

void YM2149_setChannelFreqFine(uint8_t * channel, uint8_t* value)
{
	 ESP_LOGE(TAG, "setChannelFreqFine(%d, %d)", *channel, *value);
	 struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_CHANNEL_FREQ_FINE;
	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.register_addr = YM2149_REG_0_ADDR;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.register_addr = YM2149_REG_2_ADDR;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.register_addr = YM2149_REG_4_ADDR;
	 	 break;
	 }

	 cmd.register_value = *value;
	 cmd.bit_length = 8;
	 cmd.bit_start = 0;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setChannelFreqFine CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

void YM2149_setChannelFreqRough(uint8_t* channel, uint8_t* value)
{
	 ESP_LOGE(TAG, "setChannelFreqRough(%d, %d)", *channel, *value);

	 struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_CHANNEL_FREQ_ROUGH;
	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.register_addr = YM2149_REG_1_ADDR;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.register_addr = YM2149_REG_3_ADDR;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.register_addr = YM2149_REG_4_ADDR;
		 break;
	 }

	 cmd.register_value = *value;
	 cmd.bit_length = 4;
	 cmd.bit_start = 0;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setChannelFreqRough CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

void YM2149_setNoiseFreq(uint8_t* value)
{
	 ESP_LOGE(TAG, "setNoiseFreq( %d)", *value);
	 struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_NOISE_FREQ;
	 cmd.register_addr = YM2149_REG_6_ADDR;
	 cmd.register_value = *value;
	 cmd.bit_length = 5;
	 cmd.bit_start = 0;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setNoiseFreq CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

void YM2149_setChannelNoise(uint8_t* channel, bool* value)
{
	 ESP_LOGE(TAG, "setNoise(%d, %d)", *channel, *value);
	 struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_NOISE;
	 cmd.register_addr = YM2149_REG_7_ADDR;
	 cmd.register_value = *value;
	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.bit_start = YM2149_NOISE_CHANNEL_A_BIT;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.bit_start = YM2149_NOISE_CHANNEL_B_BIT;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.bit_start = YM2149_NOISE_CHANNEL_C_BIT;
		 break;
	 }
	 cmd.bit_length = 1;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setNoise CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

void YM2149_setChannelTone(uint8_t* channel, bool* value)
{
	ESP_LOGE(TAG, "setTone(%d, %d)", *channel, *value);
	struct ym2149_command cmd;
	cmd.command_id = YM2149_CMD_ID_SET_TONE;
	cmd.register_addr = YM2149_REG_7_ADDR;
	cmd.register_value = *value;
	switch (*channel)
	{
	case YM2149_CHANNEL_A:
		cmd.bit_start = YM2149_TONE_CHANNEL_A_BIT;
		break;
	case YM2149_CHANNEL_B:
		cmd.bit_start = YM2149_TONE_CHANNEL_B_BIT;
		break;
	case YM2149_CHANNEL_C:
		cmd.bit_start = YM2149_TONE_CHANNEL_C_BIT;
		break;
	}
	cmd.bit_length = 1;

	xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	ESP_LOGE(TAG, "setTone CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

void YM2149_setChannelLevelMode(uint8_t* channel, bool* value)
{
	 ESP_LOGE(TAG, "setLevelMode(%d, %d)", *channel, *value);
	 struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_LEVEL_MODE;

	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.register_addr = YM2149_REG_8_ADDR;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.register_addr = YM2149_REG_9_ADDR;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.register_addr = YM2149_REG_A_ADDR;
		 break;
	 }

	 cmd.register_value = *value;
	 cmd.bit_length = 1;
	 cmd.bit_start = YM2149_LEVEL_MODE_BIT;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setLevelMode CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);

}

void YM2149_setChannelLevel(uint8_t* channel, uint8_t* value)
{
	 ESP_LOGE(TAG, "setLevel(%d, %d)", *channel, *value);
	 struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_LEVEL;
	 switch (*channel)
	 {
	 case YM2149_CHANNEL_A:
		 cmd.register_addr = YM2149_REG_8_ADDR;
		 break;
	 case YM2149_CHANNEL_B:
		 cmd.register_addr = YM2149_REG_9_ADDR;
		 break;
	 case YM2149_CHANNEL_C:
		 cmd.register_addr = YM2149_REG_A_ADDR;
		 break;
	 }

	 cmd.register_value = *value;
	 cmd.bit_length = 4;
	 cmd.bit_start = 0;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setLevel CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

void YM2149_setEnvelopeFreqFine(uint8_t* value)
{
	 ESP_LOGE(TAG, "setEnvelopeFreqFine(%d)", *value);
	 struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_ENVELOPE_FREQ_FINE;
	 cmd.register_addr = YM2149_REG_B_ADDR;
	 cmd.register_value = *value;
	 cmd.bit_length = 8;
	 cmd.bit_start = 0;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setEnvelopeFreqFine CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

void YM2149_setEnvelopeFreqRough(uint8_t* value)
{
	 ESP_LOGE(TAG, "setEnvelopeFreqRough(%d)", *value);
	 struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_ENVELOPE_FREQ_ROUGH;
	 cmd.register_addr = YM2149_REG_C_ADDR;
	 cmd.register_value = *value;
	 cmd.bit_length = 8;
	 cmd.bit_start = 0;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setEnvelopeFreqRough CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

void YM2149_setEnvelopeShape(uint8_t* env_shape_type, bool* value)
{
	 ESP_LOGE(TAG, "setEnvelopeShape(%d, %d)", *env_shape_type, *value); struct ym2149_command cmd;
	 cmd.command_id = YM2149_CMD_ID_SET_ENVELOPE_SHAPE;
	 cmd.register_addr = YM2149_REG_D_ADDR;
	 cmd.register_value = *value;
	 cmd.bit_length = 1;
	 cmd.bit_start = *env_shape_type ;

	 xQueueSend( cmd_queue, ( void * ) &cmd, ( TickType_t ) 0 );

	 ESP_LOGE(TAG, "setEnvelopeShape CMD cmd_id:%d register_addr:%d register_value:%d", cmd.command_id, cmd.register_addr, cmd.register_value);
}

