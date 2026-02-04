#ifndef _HP9200_H
#define _HP9200_H
#include "SysTick.h"
#include "system.h"
#include "24cxx.h"
#include "usart.h"
#include "DF_Player.h"
#define  HT9200_CLK_Pin					GPIO_Pin_1
#define  HT9200_DATA_Pin					GPIO_Pin_2
#define  HT9200_Chip_Enable_Pin 		GPIO_Pin_0
#define HT9200_DATA							PBout(2)
#define HT9200_CLK							PBout(1)
#define HT9200_Chip_Enable	 			PBout(0)

extern char phone_Number_Array[];
extern char home_phone_Number_Array[];
extern u8 control_Enter_Flag;
void HP9200_Init(void);
void dial_one_number( char dialed_character);
void phoneNumber_Filler_from24C02(void);
void dial_Phone_Number_all_Digit(char some_Phone_Number []);
void goingInto_Control_Input_Reader(void);
void home_phoneNumber_Filler_from24C02(void);
void home_dial_Phone_Number_all_Digit(char some_Phone_Number []);

#endif 
