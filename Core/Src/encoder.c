/*
 * @file encoder.c
 * @brief Functions for the rotary encoder
 *
 * This contains functions for working with
 * rotary encoder and push button.
 *
 * @author Andrii Ivanchenko
 */

#include "encoder.h"
#include "tim.h"

static int16_t new_count = 0;
static int16_t prev_count = 0;

bit_fied_TypeDef btn = {0, 0, 0};

uint32_t btn_time = 0;

/*
 * @brief Starts the TIM Encoder Interface
 */
void Encoder_Init(void)
{
	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
}

/*
 * @brief Read Timer Counter Register value
 * @retval Value of the timer counter register (TIM1_CNT) divided by 2
 */
uint16_t Encoder_Read(void)
{
	int16_t val = __HAL_TIM_GET_COUNTER(&htim1);
	return val >> 1;
}

/*
 * @brief Read the current state of the encoder
 * @retval Current encoder status
 */
Encoder_Status Encoder_Get_Status(void)
{
	new_count = Encoder_Read();
	if (new_count != prev_count) {
		if (new_count > prev_count) {
			prev_count = new_count;
			return Incremented;
		}
		else if (new_count < prev_count){
			prev_count = new_count;
			return Decremented;
		}
	}
	return Neutral;
}

/*
 * @brief Read and recognizes long and short presses encoder button
 * @retval Current encoder button status
 */
Button_Status Button_Get_Status(void)
{
	btn.state = !HAL_GPIO_ReadPin(ENC_BTN_GPIO_Port, ENC_BTN_Pin);

	if (btn.state && !btn.short_state && ((HAL_GetTick() - btn_time) > BUTTON_SHORT_PRESS_DELAY)) {
		btn.short_state = 1;
		btn.long_state = 0;
		btn_time = HAL_GetTick();
	}
	else if (btn.state && !btn.long_state && (HAL_GetTick() - btn_time) > BUTTON_LONG_PRESS_DELAY) {
		btn.long_state = 1;
		return Long_Press;
	}
	else if (!btn.state && btn.short_state && (HAL_GetTick() - btn_time) > BUTTON_SHORT_PRESS_DELAY) {
		btn.short_state = 0;
		btn_time = HAL_GetTick();
		if(!btn.long_state) {
			return Short_Press;
		}
	}
	return False_Press;
}
