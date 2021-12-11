/*
 * AD9833.c
 *
 *  Created on: 10 дек. 2021 г.
 *      Author: ivanc
 */
#include "AD9833.h"

void AD9833_Select(void)
{
	HAL_GPIO_WritePin(GPIOA/*AD9833_FSYNC_GPIO_Port*/, GPIO_PIN_1/*AD9833_FSYNC_Pin*/, GPIO_PIN_RESET);
}


void AD9833_Unselect(void)
{
	HAL_GPIO_WritePin(GPIOA/*AD9833_FSYNC_GPIO_Port*/, GPIO_PIN_1/*AD9833_FSYNC_Pin*/, GPIO_PIN_SET);
}


void AD9833_WriteRegister(uint32_t data)
{
//	digitalWrite(FSYNC, LOW);           // Set FSYNC low before writing to AD9833 registers
	AD9833_Select();

//	delayMicroseconds(10);              // Give AD9833 time to get ready to receive data.
	asm("NOP");

//	SPI.transfer(highByte(dat));        // Each AD9833 register is 32 bits wide and each 16
//	SPI.transfer(lowByte(dat));         // bits has to be transferred as 2 x 8-bit bytes.
//	HAL_SPI_Transmit(&AD9833_SPI_PORT, &data, sizeof(data), HAL_MAX_DELAY);

//	digitalWrite(FSYNC, HIGH);          //Write done. Set FSYNC high
	AD9833_Unselect();
}

void AD9833_Reset(uint8_t reset_state)
{
	if (reset_state == 0) {
		AD9833_WriteRegister(CNTRL_REG & ~(1 << 8)); // Set D8 to 0
	}
	else if (reset_state == 1) {
		AD9833_WriteRegister(CNTRL_REG | (1 << 8)); // Set D8 to 1
	}
}


void AD9833_SetWaveform(WaveDef Wave)
{
	switch(Wave) {
		case wave_sine:
			AD9833_WriteRegister(SINE);
    		break;
		case wave_square:
			AD9833_WriteRegister(SQUARE);
			break;
		case wave_triangle:
			AD9833_WriteRegister(TRIANGLE);
			break;
		default:
			break;
	}
}


void AD9833_SetFrequency(uint32_t freq)
{
	// TODO: calculate max frequency based on refFrequency.
	// Use the calculations for sanity checks on numbers.
	// Sanity check on frequency: Square - refFrequency / 2
	// Sine/Triangle - refFrequency / 4

	if (freq > (FMCLK >> 1))	//bitwise FMCLK / 2
			freq = FMCLK >> 1;
	else if (freq < 0) freq = 0;

	uint32_t freq_reg = (float)(freq * (1 << 28)) / (float)FMCLK; // Tuning Word

	uint16_t MSB = ((freq_reg & 0xFFFC000) >> 14) | FREQ_REG; // FREQ MSB
	uint16_t LSB = (freq_reg & 0x3FFF) | FREQ_REG;  // FREQ LSB

	AD9833_WriteRegister(MSB);
	AD9833_WriteRegister(LSB);
}

void AD9833_SetPhase(uint16_t phase_deg)
{
	if(phase_deg < 0) phase_deg = 0; // Changing Phase Value to Positive
	else if (phase_deg > 360) phase_deg %= 360;
	//TODO: need i &0x0FFF in phase???
	uint16_t phase_val  = ((int)(phase_deg * BITS_PER_DEG)) | 0xC000;  //using 0xC000 which is Phase 0 Register Address
	AD9833_WriteRegister(phase_val);
}

void AD9833_Init(WaveDef Wave, uint32_t freq, uint16_t phase_deg)
{
	AD9833_SetWaveform(Wave);
	AD9833_SetFrequency(freq);
	AD9833_SetPhase(phase_deg);
}
