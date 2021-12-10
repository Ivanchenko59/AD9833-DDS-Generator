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


//	SPI.transfer(highByte(dat));        // Each AD9833 register is 32 bits wide and each 16
//	SPI.transfer(lowByte(dat));         // bits has to be transferred as 2 x 8-bit bytes.
//	HAL_SPI_Transmit(&ST7735_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);

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

	switch(Wave){
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
