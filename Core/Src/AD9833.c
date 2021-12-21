/*
 * AD9833.c
 *
 *  Created on: 10 дек. 2021 г.
 *      Author: ivanc
 */
#include "AD9833.h"

void AD9833_Select(void)
{
	HAL_GPIO_WritePin(AD9833_FSYNC_GPIO_Port, AD9833_FSYNC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AD9833PORT,AD9833SS,GPIO_PIN_RESET); //+
}


void AD9833_Unselect(void)
{
	HAL_GPIO_WritePin(AD9833_FSYNC_GPIO_Port, AD9833_FSYNC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AD9833PORT,AD9833SS,GPIO_PIN_SET); //high = deselected	 //+
}


void AD9833_WriteRegister(uint16_t data)
{
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");

	uint8_t LByte = data & 0x00ff; //Low Byte

	uint8_t HByte = data >> 8;  //High Byte

	HAL_SPI_Transmit(&AD9833_SPI_PORT, &LByte, 1, HAL_MAX_DELAY);
	HAL_SPI_Transmit(&AD9833_SPI_PORT, &HByte, 1, HAL_MAX_DELAY);

//	for (uint8_t i = 0; i < 16 ; i++) {
//		if(data & 0x8000) HAL_GPIO_WritePin(AD9833PORT,AD9833DATA,GPIO_PIN_SET);   //bit=1, Set High
//		else HAL_GPIO_WritePin(AD9833PORT,AD9833DATA,GPIO_PIN_RESET);        //bit=0, Set Low
//	    asm("NOP");
//		HAL_GPIO_WritePin(AD9833PORT,AD9833SCK,GPIO_PIN_RESET);             //Data is valid on falling edge
//		asm("NOP");
//		HAL_GPIO_WritePin(AD9833PORT,AD9833SCK,GPIO_PIN_SET);
//		data = data<<1; //Shift left by 1 bit
//	}
//	HAL_GPIO_WritePin(AD9833PORT,AD9833DATA,GPIO_PIN_RESET);                    //Idle low
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
	asm("NOP");
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
	AD9833_Select();
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
	AD9833_Unselect();
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

	HAL_GPIO_WritePin(AD9833PORT,AD9833DATA,GPIO_PIN_SET);
	HAL_GPIO_WritePin(AD9833PORT,AD9833SCK,GPIO_PIN_SET);
	HAL_GPIO_WritePin(AD9833PORT,AD9833SS,GPIO_PIN_SET);

	AD9833_Select();
	AD9833_Reset(1);
	AD9833_WriteRegister(LSB);
	AD9833_WriteRegister(MSB);
	AD9833_Reset(0);
	AD9833_Unselect();
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
//	AD9833_Reset(1); //dont need

	HAL_GPIO_WritePin(AD9833PORT,AD9833DATA,GPIO_PIN_SET); // Set All SPI pings to High
	HAL_GPIO_WritePin(AD9833PORT,AD9833SCK,GPIO_PIN_SET);  // Set All SPI pings to High
	HAL_GPIO_WritePin(AD9833PORT,AD9833SS,GPIO_PIN_SET);   // Set All SPI pings to High

	AD9833_SetFrequency(freq);

	AD9833_SetWaveform(Wave);
//	AD9833_SetPhase(phase_deg);
}
