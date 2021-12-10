/*
 * @file encoder.h
 * @brief Function prototypes for the rotary encoder
 *
 * This contains the prototypes for the rotary encoder
 * driver and eventually any macros, constants.
 *
 * @author Andrii Ivanchenko
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

//#include "stm32f4xx_hal.h"
#include "main.h"
//#include "tim.h"

#define ENC_S1_Pin 			GPIO_PIN_8
#define ENC_S1_GPIO_Port 	GPIOA
#define ENC_S2_Pin 			GPIO_PIN_9
#define ENC_S2_GPIO_Port 	GPIOA
#define ENC_BTN_Pin 		GPIO_PIN_10
#define ENC_BTN_GPIO_Port 	GPIOA

#define BUTTON_SHORT_PRESS_DELAY 	100
#define BUTTON_LONG_PRESS_DELAY 	1000

typedef struct {
	uint8_t state : 1;
	uint8_t short_state : 1;
	uint8_t long_state : 1;
} bit_fied_TypeDef;

typedef enum {
	Incremented = 1,
	Decremented = -1,
	Neutral = 0,
} Encoder_Status;

typedef enum {
	False_Press,
	Short_Press,
	Long_Press,
} Button_Status;

/*
 * @brief Starts the TIM Encoder Interface
 */
void Encoder_Init(void);

/*
 * @brief Read Timer Counter Register value
 * @retval Value of the timer counter register (TIM1_CNT) divided by 2
 */
uint16_t Encoder_Read(void);

/*
 * @brief Read the current state of the encoder
 * @retval Current encoder status
 */
Encoder_Status Encoder_Get_Status(void);

/*
 * @brief Read and recognizes long and short presses encoder button
 * @retval Current encoder button status
 */
Button_Status Button_Get_Status(void);

#endif /* INC_ENCODER_H_ */
