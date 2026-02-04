#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "DF_Player.h"
#include "pin_Config.h"
#include "HP9170.h"
#include "timer.h"
#include "74HC595.h"
#include "24cxx.h"
#include "iic.h"
#include "HP9200.h"
#include "ds18b20.h"
int main()
{	
	u8 callout_Result ;
	u8 number_Of_Retry_Remaining;
	rpc_Status calling_Dected;
	// should add this line below to whenever there's overheating or power outage detected
	number_Of_Retry_Remaining = number_Of_Recall;
	SysTick_Init(72);
	USART1_Init(57600);
	DF_Player_Init();
	HP9170_Init();
	pin_Setup();
	the_74HC595_Init();
	AT24CXX_Init();
	HP9200_Init();
	if ( !DS18B20_Init())
	{
		printf("temperature sensor detected!\n");
	}
	// Storing personal information ( password and phone number) from the 24C02
	password_Filler_from24C02();
	phoneNumber_Filler_from24C02();
	home_phoneNumber_Filler_from24C02();
	printf("program starts");
	while(1)
	{	
		call_In_Dection:
		calling_Dected=voltage_Detection_v5(3);
		if ( timer3_timeout_Flag )
		{
			printf("de Init timer3\n");
			TIM_DeInit(TIM3);
			timer3_timeout_Flag = 0 ;
			heat_Detection(10,20,35); 
			power_Outage_Detection();
			if ( temperature_Status_Token == 4)
			{
				goto start_Dialing;
			}
			else if ( power_Outage_Event ==1 ) 
			{
				goto start_Dialing;
			}
			else {
				overHeating_Task_Waiting=0;
				powerOutage_Task_Waiting = 0;
				goto call_In_Dection;
			}
		}
		else if ( temperature_Status_Token ==4)
		{
			goto start_Dialing;
		}
		else if (power_Outage_Event ==1 ) 
		{
			goto start_Dialing;
		}
		else if (calling_Dected == status_Callin_Dectected)
		{	
			printf( "call detected\n");			
			// 001f older, 001 track is the current problem track (遇到問題語音)
			PAout(5) =1;
			// Check if the password is correct with 2 tries 
			// if correct--> ask user which relay to control 
			// if not correct --> ask user to retry it again if the password still wrong---> hang up 
			play_One_Track(2,1);
			if ( password_Sequence ( password_Array, size_Of_Password))
			{
				afterPassword_StartingPoint:
				play_One_Track (2,5);// 002 folder, 005 track is the track" which relay to control"(控制哪個ｒｅｌａｙ語音）
				
				user_Relay_Control_StartingPoint:
				two_User_Input_Sequence_V2();
				relay_Action(sequence_Buffer);
				if (( sequence_Buffer [0]=='?')||((sequence_Buffer[0]=='#')&&(sequence_Buffer[1]=='#')))
				{
					// timeout or quit 
					calling_Dected=status_Callin_hanging_Up;
					// this delay is just in case re-dial right after hanging up. A
					delay_ms(1000);
					continue;
				}
				else 
				{
					goto user_Relay_Control_StartingPoint;
				}
			}
			else {
				// user hanged up during password typing 
				continue;
			}
			
		}
		// if powerOutage or overheating --> callout flag 
		// if (callout flag == true)--> dialing 
		start_Dialing:
		printf("start dialing\n");
		dial_Phone_Number_all_Digit(phone_Number_Array);
		callout_Result = callout_Detection_mod(4);
		if ( callout_Result == pickedUp)
		{
		
			number_Of_Retry_Remaining = number_Of_Recall;
			delay_ms(1000);
			delay_ms(1000);
			if ( overHeating_Task_Waiting) 
			{
				play_One_Track(4,1);
				delay_ms(1000);
			}
			else if (powerOutage_Task_Waiting) 
			{
				play_One_Track(4,2);
				delay_ms(1000);
			}
		
			overHeating_Task_Waiting = 0 ;
			powerOutage_Task_Waiting=0;
			printf("going into input reader\n");
			goingInto_Control_Input_Reader();
			if ( control_Enter_Flag == 1)
			{
				control_Enter_Flag=0;
				goto afterPassword_StartingPoint;
				}
			}
		else if ((callout_Result ==no_Reply)||(callout_Result ==line_Busy))
		{
			//10 second timer 
			if ( number_Of_Retry_Remaining> 0)
			{
			timer_Init_Timer3(18000, 60000-1) ;
			number_Of_Retry_Remaining--;
			}
			else if (number_Of_Retry_Remaining == 0)
			{
				// hanging up 
				overHeating_Task_Waiting = 0 ;
				powerOutage_Task_Waiting=0;
				printf("recall times run out, hanging up\n");
				PAout(5) = 0;
				number_Of_Retry_Remaining = number_Of_Recall;
				delay_ms(1000);
			}
			// timeout, no one answering, wait x seconds and call later 
		}
	
		}
		// call the digits 
		// see if there's response 
		// if not --> check how many tries left--> set the timer again
		// if yes --> go to play operate message 
		}
