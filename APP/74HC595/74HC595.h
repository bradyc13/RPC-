#ifndef 	_74HC595_H
#define	_74HC595_H


#include "system.h"

#define RCLK_PORT 			GPIOB   
#define RCLK_PIN 			GPIO_Pin_5
#define RCLK_PORT_RCC		RCC_APB2Periph_GPIOB

#define SRCLK_PORT 			GPIOB   
#define SRCLK_PIN 			GPIO_Pin_3
#define SRCLK_PORT_RCC		RCC_APB2Periph_GPIOB

#define SER_PORT 			GPIOB   
#define SER_PIN 			GPIO_Pin_4
#define SER_PORT_RCC		RCC_APB2Periph_GPIOB

#define SER 				PBout(4)
#define SRCLK			PBout(3)
#define RCLK				PBout(5)
extern u8  relay1_Status;
extern u8  relay2_Status;
extern u8  relay3_Status;
extern u8  relay4_Status;
extern u8  relay5_Status;
extern u8  all_Relay_Status;
extern u8 timeout_char_Flag;
	
				
void the_74HC595_Init(void);
void the_74HC595_Output(u8 data);
void relay_Controller(u8 number_Of_the_Relay);
void relay_Action(char action_Buffer[] );
#endif 
