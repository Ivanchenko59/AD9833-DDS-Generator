/*
 * AD9833.h
 *
 *  Created on: 10 дек. 2021 г.
 *      Author: ivanc
 */

#ifndef INC_AD9833_H_
#define INC_AD9833_H_

#include "main.h"



#define AD9833PORT GPIOA      // PORT OF AD9833
#define AD9833DATA GPIO_PIN_1 // SPI DATA PIN
#define AD9833SCK GPIO_PIN_2  // SPI Clock PIN
#define AD9833SS GPIO_PIN_3   // SPI Chip Select


/*** Redefine if necessary ***/
#define AD9833_SPI_PORT 		hspi2
extern SPI_HandleTypeDef 		AD9833_SPI_PORT;

#define AD9833_FSYNC_Pin 		GPIO_PIN_2
#define AD9833_FSYNC_GPIO_Port 	GPIOB

#define FMCLK	 		25000000
#define BITS_PER_DEG 	11.3777777777778	// 4096 / 360

#define CNTRL_REG		0x2100
#define FREQ_REG		0x4000
#define PHASE_REG		0xC000

#define SINE	 		0x2000
#define SQUARE 			0x2028
#define TRIANGLE		0x2002


typedef enum {
	wave_triangle,
	wave_square,
	wave_sine,
} WaveDef;

void AD9833_Select(void);
void AD9833_Unselect(void);

void AD9833_WriteRegister(uint16_t data);

void AD9833_Reset(uint8_t reset_state);

void AD9833_SetFrequency(uint32_t freq);
void AD9833_GetFrequency();

void AD9833_SetWaveform(WaveDef Wave);
void AD9833_GetWaveform();

void AD9833_SetPhase(uint16_t phase_deg);
void AD9833_GetPhase();

void AD9833_Init(WaveDef Wave, uint32_t freq, uint16_t phase_deg);

#endif /* INC_AD9833_H_ */
