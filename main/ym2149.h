/*
 * ym2149f.h
 *
 *  Created on: 14.03.2020
 *      Author: DL1XY
 */

#ifndef MAIN_YM2149_H_
#define MAIN_YM2149_H_

#include <stdbool.h>
#include "sys/types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#define DEBUG_OUTPUT 0

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define YM2149_CHANNEL_A 	0
#define YM2149_CHANNEL_B 	1
#define YM2149_CHANNEL_C 	2

#define YM2149_ENV_SHAPE_HOLD_BIT 	0
#define YM2149_ENV_SHAPE_ALT_BIT 	1
#define YM2149_ENV_SHAPE_ATT_BIT 	2
#define YM2149_ENV_SHAPE_CONT_BIT	3

#define YM2149_LEVEL_MODE_BIT 		4

#define YM2149_TONE_CHANNEL_A_BIT 	0
#define YM2149_TONE_CHANNEL_B_BIT 	1
#define YM2149_TONE_CHANNEL_C_BIT 	2

#define YM2149_NOISE_CHANNEL_A_BIT 	3
#define YM2149_NOISE_CHANNEL_B_BIT 	4
#define YM2149_NOISE_CHANNEL_C_BIT	5

#define YM2149_REG_0_ADDR 	0b0000
#define YM2149_REG_1_ADDR 	0b0001
#define YM2149_REG_2_ADDR 	0b0010
#define YM2149_REG_3_ADDR 	0b0011
#define YM2149_REG_4_ADDR 	0b0100
#define YM2149_REG_5_ADDR 	0b0101
#define YM2149_REG_6_ADDR 	0b0110
#define YM2149_REG_7_ADDR 	0b0111
#define YM2149_REG_8_ADDR 	0b1000
#define YM2149_REG_9_ADDR 	0b1001
#define YM2149_REG_A_ADDR 	0b1010
#define YM2149_REG_B_ADDR 	0b1011
#define YM2149_REG_C_ADDR 	0b1100
#define YM2149_REG_D_ADDR 	0b1101
#define YM2149_REG_E_ADDR 	0b1110
#define YM2149_REG_F_ADDR 	0b1111

#define YM2149_CMD_ID_SET_CHANNEL_FREQ_FINE  	0
#define YM2149_CMD_ID_SET_CHANNEL_FREQ_ROUGH  	1
#define YM2149_CMD_ID_SET_NOISE_FREQ  		2
#define YM2149_CMD_ID_SET_NOISE  		3
#define YM2149_CMD_ID_SET_TONE  		4
#define YM2149_CMD_ID_SET_LEVEL_MODE  		5
#define YM2149_CMD_ID_SET_LEVEL	  		6
#define YM2149_CMD_ID_SET_ENVELOPE_FREQ_FINE	7
#define YM2149_CMD_ID_SET_ENVELOPE_FREQ_ROUGH	8
#define YM2149_CMD_ID_SET_ENVELOPE_SHAPE	9

#define YM2149_TIMER_DIVIDER	80	// 80MHz / YM2149_CLOCK_DIVIDER = X MHz
#define YM2149_TIMER_SCALE		(TIMER_BASE_CLK / YM2149_TIMER_DIVIDER)


#define YM2149_COMMAND_STATE_IDLE		0
#define YM2149_COMMAND_STATE_ADDR_MODE		1
#define YM2149_COMMAND_STATE_WRITE_MODE		2
#define YM2149_COMMAND_STATE_CLEANUP		5
#define YM2149_COMMAND_STATE_RESET		6
#define YM2149_COMMAND_STATE_INIT		7

// ESP32 GPIO
#define YM2149_RESET_GPIO	12
#define YM2149_CLOCK_GPIO	13
#define YM2149_BC1_GPIO		33
#define YM2149_BCDIR_GPIO	32

#define YM2149_DA0_GPIO		21
#define YM2149_DA1_GPIO		16
#define YM2149_DA2_GPIO		4
#define YM2149_DA3_GPIO		0
#define YM2149_DA4_GPIO		14
#define YM2149_DA5_GPIO		27
#define YM2149_DA6_GPIO		26
#define YM2149_DA7_GPIO		25


typedef struct ym2149 {
	uint8_t channel_a_freq_fine:8;
	uint8_t channel_a_freq_rough:4;
	uint8_t channel_b_freq_fine:8;
	uint8_t channel_b_freq_rough:4;
	uint8_t channel_c_freq_fine:8;
	uint8_t channel_c_freq_rough:4;

	uint8_t noise_freq:5;

	uint8_t channel_a_noise:1;
	uint8_t channel_b_noise:1;
	uint8_t channel_c_noise:1;

	uint8_t channel_a_tone:1;
	uint8_t channel_b_tone:1;
	uint8_t channel_c_tone:1;

	uint8_t channel_a_amp_mode:1;
	uint8_t channel_b_amp_mode:1;
	uint8_t channel_c_amp_mode:1;

	uint8_t channel_a_level:4;
	uint8_t channel_b_level:4;
	uint8_t channel_c_level:4;

	uint8_t envelope_fine_freq:8;
	uint8_t envelope_rough_freq:8;

	uint8_t envelope_shape_cont:1;
	uint8_t envelope_shape_att:1;
	uint8_t envelope_shape_alt:1;
	uint8_t envelope_shape_hold:1;

}ym2149;

typedef struct ym219_register
{
	uint8_t reg_0:8;
	uint8_t reg_1:8;
	uint8_t reg_2:8;
	uint8_t reg_3:8;
	uint8_t reg_4:8;
	uint8_t reg_5:8;
	uint8_t reg_6:8;
	uint8_t reg_7:8;
	uint8_t reg_8:8;
	uint8_t reg_9:8;
	uint8_t reg_A:8;
	uint8_t reg_B:8;
	uint8_t reg_C:8;
	uint8_t reg_D:8;
	uint8_t reg_E:8; // not used
	uint8_t reg_F:8; // not used

} ym219_register;


typedef struct ym2149_command
{
	uint8_t command_id;
	uint8_t register_addr;
	uint8_t register_value;
	uint8_t bit_start;
	uint8_t bit_length;
} ym2149_command;



// init functions
void YM2149_init();
void YM2149_init_pwm();
void YM2149_init_timer_parameter(int timer_period_us);

// ISR
void IRAM_ATTR YM2149_cmdHandler(void *pvParameter);


void YM2149_reset();

// YM2149 play functions
void YM2149_playChannel (uint8_t* channels);
void YM2149_stopChannel (uint8_t* channels);

// YM2149 sound control functions
void YM2149_setChannelFreqFine(uint8_t* channel, uint8_t* value);
void YM2149_setChannelFreqRough(uint8_t* channel, uint8_t* value);
void YM2149_setNoiseFreq(uint8_t* value);
void YM2149_setChannelNoise(uint8_t* channel, bool* value);
void YM2149_setChannelTone(uint8_t* channel, bool* value);
void YM2149_setChannelLevelMode(uint8_t* channel, bool* value);
void YM2149_setChannelLevel(uint8_t* channel, uint8_t* value);
void YM2149_setEnvelopeFreqFine(uint8_t* value);
void YM2149_setEnvelopeFreqRough(uint8_t* value);
void YM2149_setEnvelopeShape(uint8_t* env_shape_type, bool* value);

void debug();
void debugCmd();
void debugReg();
void debugConf();

#endif /* MAIN_YM2149_H_ */
