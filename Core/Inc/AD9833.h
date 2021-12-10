/*
 * AD9833.h
 *
 *  Created on: 10 дек. 2021 г.
 *      Author: ivanc
 */

#ifndef INC_AD9833_H_
#define INC_AD9833_H_

#include "main.h"

#define CNTRL_REG	0x2100
#define FREQ_REG	0x4000
#define PHASE_REG	0xC000

#define SINE 		0x2000
#define SQUARE 		0x2028
#define TRIANGLE	0x2002


typedef enum {
	wave_sine,
	wave_square,
	wave_triangle,
} WaveDef;

void AD9833_Select(void);
void AD9833_Unselect(void);

void AD9833_WriteRegister(uint32_t data);

void AD9833_Reset(uint8_t reset_state);

void AD9833_SetFrequency(uint32_t freq);
void AD9833_GetFrequency();

void AD9833_SetWaveform(WaveDef Wave);
void AD9833_GetWaveform();

void AD9833_SetPhase();
void AD9833_GetPhase();

void AD9833_Init();

#endif /* INC_AD9833_H_ */
