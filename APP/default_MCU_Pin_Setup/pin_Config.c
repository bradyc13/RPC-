#include "pin_Config.h"
#include "SysTick.h"
#include "timer.h"
#include "24cxx.h"
#include "usart.h"
#include "ds18b20.h"
#include "HP9170.h"
u8 power_Outage_Event = 0;
u8 powerOutage_Task_Waiting = 0 ;
void pin_Setup(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	PAout(5)= 0; 
}
/**
  * @brief  Detecting if there's phone calling in via the DV signal 
*利用DV腳偵測電話撥號
* @param  number_Of_Rings : number of rings intended  
*響鈴次數
  * @retval 
  * @note  
  */
u8 user_Calling_Dectection_V3_firstEdition( u8 number_Of_Rings)
{  u8 low_Voltage_Counter;
	u8 high_Voltage_Counter;
	u8 total_Valid_RingTone;
	u8 valid_low;
	u8 valid_high;
	u8 falling_edge;
	
	timer_Init(50000,14400-1);
	falling_edge=0;
	valid_low=0;
	valid_high=0;
	total_Valid_RingTone=0;
	low_Voltage_Counter = 0;
	high_Voltage_Counter= 0;
	while (1)
	{	
		low_Voltage_Counter = 0;
		high_Voltage_Counter= 0;
		delay_ms(200);
		// start by checking the falling edge of PA4 
		// 先看PA4 是否由高變低
		if ( PAin(4)==1){
			delay_ms(50);
			if (PAin(4)==0){
				falling_edge= 1;
			}
			else {
				falling_edge= 0; 
			}
		}					
		if ( falling_edge==1)
			{
				while ( PAin(4)==0)
					{
						// while PAin4 is low, check how long was it low
						// we assume checking every 10 ms, therefore with the communication protocal in Taiwan ( ring for 1 sec and stop for 2 seconds), we should be able to check around 10 times where PAin is low
						// 當PA4為低，簡長PA4為低多久
						// 假設為台灣電新設定，響1秒停2秒
					while ( PAin(4)==0){
						delay_ms(100);
						low_Voltage_Counter ++;
					}
					// If the low voltage counter ( the counter for counting how many times PAin was sampled low after the falling edge)
					// it means the low voltage period is invalid. 
					// 如果低電瓶測量次數介於8~12為有效低電瓶，反之低電平過長或過短
					if ( low_Voltage_Counter<7 || low_Voltage_Counter>9){
						// not a valid tone due to low voltage too short 
						printf("not valid low voltage\n");
						printf(" low voltage counter is %u \n",low_Voltage_Counter);
						valid_low=0 ; 
						break;
						}
					// Else the low voltage period is valid
					else{
						valid_low = 1;
						printf("valid low detected\n");
						} 
					// After the low voltage period is valid, we check if the high voltage period is valid ( where the ringing stops) 
					// Using similar method as above.
					//高電瓶邏輯和低電平相似
					while ( PAin(4)==1) {
						delay_ms(100);
						high_Voltage_Counter ++;
					}
					// each sampling is still 100ms apart
					// Since in Taiwan the ring tone goes silent for 2 seconds after 1 second of ring, around 20 count for the high voltage counter will be a proper value 
					if ( high_Voltage_Counter<15|| high_Voltage_Counter>19){
						// not a valid tone due to low voltage too short 
						printf("not valid high voltage \n");
						printf("high voltage counter is %u\n",high_Voltage_Counter);
						break;
						}
					else {
						printf("valid high is detected");
						valid_high = 1;
						break;
					}
					}
				}
			// if both high voltage and low voltage is valid, then it is a valid ring tone
			//如果低和高電頻長度皆有效，則代表一次有效響鈴
		if ( valid_high&&valid_low){
			total_Valid_RingTone++;
		}
		else{
			total_Valid_RingTone= 0;
		}
		// if the detected ringing counts same as the user intended end the function and return 1 
		// 如果紀錄響鈴次數和使用者設定次數一樣 return 1 
		printf("Total number of rings%u\n",total_Valid_RingTone);
		if ( total_Valid_RingTone ==number_Of_Rings) {
			TIM_DeInit(TIM4);
			return 1;
			}
		// timer 4 is set to 10s countdown for timeout 
		// returning 0 to quit this function 
		// timer 計時10秒，10秒後return0 來退出function
		if ( timeout_Flag ==1){
			timeout_Flag= 0;
			TIM_DeInit(TIM4);
			printf("TimedOUT  for dialing!!!\n");
			return 0 ; 
			
		}
	}
}
void power_Outage_Detection(void)
{
	if ( PBin(10))
	{
		power_Outage_Event = 1;
		printf("power out!\n");
	}
	else 
	{
		power_Outage_Event=0;
	}
		
}

/**
  * @brief  Detecting overheating, power outage and user call in 
* @param  rings : number of rings for the RPC to pick up the call when user calls in
* @retval 1: user call in detected, 0: other events dectect (e.g. overheating, power outage) 
  * @note  
  */
rpc_Status voltage_Detection_v5(u8 rings)
{
	u8 i ;
	u8 number_Of_Rings;
	i =0;
	number_Of_Rings=0; 
	while (1){
		// Check if overheating 
		if (overHeating_Task_Waiting == 0)
		{
			heat_Detection(10,20,35); 
		}
		if ( (temperature_Status_Token ==4)&&(overHeating_Task_Waiting == 0))
		{
			// overHeating_Task_Waiting  is a flag for knowing if the temperature went wrong before 
			overHeating_Task_Waiting=1;
			return status_Overheating ; 
		}
		//Check if power outage 
		if (powerOutage_Task_Waiting ==0)
		{
			power_Outage_Detection();
		}
		if ( (power_Outage_Event == 1)&&(powerOutage_Task_Waiting ==0))
		{ 
			powerOutage_Task_Waiting = 1;
			return status_Power_Outage;
		}
		// check if there's previous overheating or power outage 
		if ( timer3_timeout_Flag ==1) 
		{
			return status_Previous_Task_Remaining; 
		}
		theStart:
	// this block checks if there's user call in signal with PA4 
	if (PAin(4))
	{
		delay_ms(100);
		// Checking if high to low
		if (!PAin(4))
		{
			// first delay to determine if going into low voltage sequence
			delay_ms(100);
			if ( !PAin(4))
			{
				// Start of low voltage sequence
				// this timeDeinit is for the timer that was set when 1 valid ringtone is detected. In case there's x ringtones before reaching the target amount of ringtone
				if ( i ==3 )
				{
					TIM_DeInit(TIM4);
				}
			    printf("At the start of the low voltage sequence\n");
				printf("timer started\n");
				timer_Init(50000,14400-1);
			}
			else if ( PAin(4) ==1)
			{
				// Does not go into the low voltage sequence
				goto theStart;
			}
			// check if going into the low voltage sequence 
			while ( !PAin(4))
			{
				low_Voltage:
				delay_ms(100);
			}
			// end of low voltage sequence
			// Start of high voltage sequence
			printf("at the start of high voltage sequence\n");
			if ( PAin(4) ==0)
			{
				goto low_Voltage;
			}
			else
			{
				for ( i= 0 ;i<=2;i++)
				{
					delay_ms(100);
					if ( PAin(4))
					{
						continue;
					}
					else 
					{
						break;
					}
				}
				if ( i ==3)
				{
					TIM_DeInit(TIM4);
					timer_Init(50000,14400-1);
					number_Of_Rings++;
					
				}
				else
				{
					printf("invalid ring tone reset to 0 \n");
					number_Of_Rings= 0;
				}
			}
			}	
		if ( timeout_Flag==1 )
		{
			timeout_Flag= 0 ; 
			TIM_DeInit(TIM4);
			printf("detecting timedout\n");
			printf("reset number of rings detected to 0\n ");
			number_Of_Rings=0;
			return status_calls_reseted; 
		}
		printf("call in rings %u\n", number_Of_Rings);
		if ( number_Of_Rings==rings)
		{
			TIM_DeInit(TIM4);
			return status_Callin_Dectected; 
		}
		}
	}
}
u8 voltage_Detection_v3(u8 rings)
{
	
	u8 i ;
	u8 number_Of_Rings;
	i =0;
	number_Of_Rings=0; 
	while (1){
		// needs to add the flag for the temperature and heat for condition checking if breaking this while loop to dialing 
		theStart:
	if (PAin(4))
	{
		delay_ms(100);
		// Checking if high to low
		if (!PAin(4))
		{
			// first delay to determine if going into low voltage sequence
			delay_ms(100);
			if ( !PAin(4))
			{
				// Start of low voltage sequence
			    printf("At the start of the low voltage sequence\n");
				printf("timer started\n");
				timer_Init(50000,14400-1);
			}
			else if ( PAin(4) ==1)
			{
				// Does not go into the low voltage sequence
				goto theStart;
			}
			// check if going into the low voltage sequence 
			while ( !PAin(4))
			{
				low_Voltage:
				delay_ms(100);
			}
			// end of low voltage sequence
			// Start of high voltage sequence
			printf("at the start of high voltage sequence\n");
			if ( PAin(4) ==0)
			{
				goto low_Voltage;
			}
			else
			{
				for ( i= 0 ;i<=2;i++)
				{
					delay_ms(100);
					if ( PAin(4))
					{
						continue;
					}
					else 
					{
						break;
					}
				}
				if ( i ==3)
				{
					TIM_DeInit(TIM4);
					number_Of_Rings++;
					
				}
				else
				{
					printf("invalid ring tone reset to 0 \n");
					number_Of_Rings= 0;
				}
			}
			}	
		if ( timeout_Flag==1 )
		{
			timeout_Flag= 0 ; 
			TIM_DeInit(TIM4);
			printf("detecting timedout\n");
			printf("reset number of rings detected to 0\n ");
			number_Of_Rings=0;
			return 0 ; 
		}
		printf("number of rings detected %u\n", number_Of_Rings);
		if ( number_Of_Rings==rings)
		{
			return 1; 
		}
		}
	}
}
u8 callout_Detection(u8 rings)
{
	u8 i ;
	u8 number_Of_Rings;
	u8 valid_Ringtone_Tracker;
	u8 invalid_Ringtone_Count;
	u8 firstRingTone_Detected;
	firstRingTone_Detected = 1;
	invalid_Ringtone_Count=0;
	valid_Ringtone_Tracker = 0 ;
	i =0;
	number_Of_Rings=0; 
	// add a 5 second timer 
	// 5 second timer for detecting if there's ringtone or not
	timer_Init(12000,36000-1);
	while (1){
		// needs to add the flag for the temperature and heat for condition checking if breaking this while loop to dialing 
		theStart:

	if (PBin(9))
	{
		delay_ms(25);
		// Checking if high to low
		if (!PBin(9))
		{
			printf("b\n");
			//printf("in second if\n");
			// first delay to determine if going into low voltage sequence
			delay_ms(25);
			if ( !PBin(9))
			{
				//De init the first 3 second timer 
				// Start of low voltage sequence
			    //printf("At the start of the low voltage sequence\n");
				//printf("timer started\n");
				if ( firstRingTone_Detected) 
				{
					TIM_DeInit(TIM4);
					firstRingTone_Detected=0;
				}
				if ( valid_Ringtone_Tracker == 1)
				{
					valid_Ringtone_Tracker = 0;
					TIM_DeInit(TIM4);
				}
			}
			else if ( PBin(9) ==1)
			{
				// Does not go into the low voltage sequence
				goto theStart;
			}
			// check if going into the low voltage sequence 
			while ( !PBin(9))
			{
				low_Voltage:
				delay_ms(100);
			}
			// end of low voltage sequence
			// Start of high voltage sequence
			//printf("at the start of high voltage sequence\n");
			if ( PBin(9) ==0)
			{
				goto low_Voltage;
			}
			else
			{
				for ( i= 0 ;i<=2;i++)
				{
					delay_ms(100);
					if ( PBin(9))
					{
						continue;
					}
					else 
					{
						break;
					}
				}
				if ( i ==3)
				{
					//TIM_DeInit(TIM4);
					number_Of_Rings++;
					timer_Init(8000,36000-1);
					valid_Ringtone_Tracker = 1;
					//printf("current number of rings %u\n", number_Of_Rings);
				}
				else
				{
					invalid_Ringtone_Count++;
					//printf("value of i is %u\n",i);
					//printf("invalid ring tone count is  %u \n",invalid_Ringtone_Count);
					
				}
			}
			}	
		//printf("number of rings detected %u\n", number_Of_Rings);
		printf("%u\n",number_Of_Rings);
		if ( timeout_Flag==1 )
		{

			timeout_Flag= 0 ; 
			TIM_DeInit(TIM4);
			if ( number_Of_Rings== 0)
			{
				//  user busy 
				printf("user busy !\n");
				PAout(5) = 0;
				return 0 ; 
			}
			else if ((number_Of_Rings>0)&&(PBin(9)==1))
			{
				// user picked up
				printf("user Picked up!\n");
				return 2; 
			}
		}
		if ( number_Of_Rings>rings)
		{
			// timeout as the when number of rings is bigger than rings ( set by user) , no one is answering 
			TIM_DeInit(TIM4);
			printf(" no one answering\n");
			// hang up
			PAout(5) = 0;
			return 1; 
		}
		}
	}
}
u8 DV_Checking (void )
{
u8 i = 0 ; 
theStart:
	if ( read_DV_Signal ==0)
	{
		printf("0 ");
		delay_ms(5);
		// checking rising edge
		if ( read_DV_Signal ==1)
		{
			printf("1 ");
			delay_ms(5);
			if ( read_DV_Signal==0)
			{
				goto theStart;
			}
			// going into high voltage phase
			else if ( read_DV_Signal==1)
			{
				// Start the timer?
				// do nothing?
			}
			while ( read_DV_Signal==1) 
			{
				highVoltage:
				printf("1 ");
				delay_ms(5);
			}
			if (  read_DV_Signal==0)
			{
				for (  i=0; i<=2;i++)
				{
					printf("0 ");
					delay_ms(5);
					if ( read_DV_Signal==0)
					{
						continue;
					}
				}
			}
			else if ( read_DV_Signal==1)
			{
				printf("1 ");
				goto highVoltage;
			}
			if (i ==3)
			{	
				
				return 1; 
			}
		}
	}
	return 0 ; 
}
u8  DV_Checking_Simplified (void )
{

	if ( read_DV_Signal ==1) 
	{
		while ( read_DV_Signal == 1) 
		{
		}
		return 1; 
	}

	return 0 ; 
}
	callout_User_Status callout_Detection_mod(u8 rings)
	{
		u8 i ;
		u8 number_Of_Rings;
		u8 valid_Ringtone_Tracker;
		u8 invalid_Ringtone_Count;
		u8 firstRingTone_Detected;
		u8 user_PickedUp;
		user_PickedUp=0;
		firstRingTone_Detected = 1;
		invalid_Ringtone_Count=0;
		valid_Ringtone_Tracker = 0 ;
		i =0;
		number_Of_Rings=0; 
		// add a 5 second timer 
		// 5 second timer for detecting if there's ringtone or not
		timer_Init(12000,36000-1);
		while (1){
			// needs to add the flag for the temperature and heat for condition checking if breaking this while loop to dialing 
			theStart:
		user_PickedUp = DV_Checking_Simplified();
			pickUp_Breakpoint:
		if ( user_PickedUp)
		{

			if ( firstRingTone_Detected || valid_Ringtone_Tracker) 
			{
				if ( firstRingTone_Detected)
				{
					printf("A");
				}
				else if (valid_Ringtone_Tracker)
				{
					printf("B");
				}
				TIM_DeInit(TIM4);
			}
			printf("picked up b \n");
			return pickedUp;
		}
		if (PBin(9))
		{
			delay_ms(25);
			// Checking if high to low
			if (!PBin(9))
			{
				printf("b\n");
				//printf("in second if\n");
				// first delay to determine if going into low voltage sequence
				delay_ms(25);
				if ( !PBin(9))
				{
					//De init the first 3 second timer 
					// Start of low voltage sequence
					//printf("At the start of the low voltage sequence\n");
					//printf("timer started\n");
					if ( firstRingTone_Detected) 
					{
						TIM_DeInit(TIM4);
						firstRingTone_Detected=0;
					}
					if ( valid_Ringtone_Tracker == 1)
					{
						valid_Ringtone_Tracker = 0;
						TIM_DeInit(TIM4);
					}
				}
				else if ( PBin(9) ==1)
				{
					// Does not go into the low voltage sequence
					goto theStart;
				}
				// check if going into the low voltage sequence 
				while ( !PBin(9))
				{
					user_PickedUp = DV_Checking();
					if ( user_PickedUp) 
					{
						goto pickUp_Breakpoint;
					}
					low_Voltage:
					delay_ms(100);
				}
				// end of low voltage sequence
				// Start of high voltage sequence
				//printf("at the start of high voltage sequence\n");
				if ( PBin(9) ==0)
				{
					goto low_Voltage;
				}
				else
				{
					for ( i= 0 ;i<=2;i++)
					{
						
						user_PickedUp = DV_Checking();
						if ( user_PickedUp) 
						{
							goto pickUp_Breakpoint;
						}
						delay_ms(100);
						if ( PBin(9))
						{
							continue;
						}
						else 
						{
							break;
						}
					}
					if ( i ==3)
					{
						//TIM_DeInit(TIM4);
						number_Of_Rings++;
						timer_Init(8000,36000-1);
						valid_Ringtone_Tracker = 1;
						//printf("current number of rings %u\n", number_Of_Rings);
					}
					else
					{
						invalid_Ringtone_Count++;
						//printf("value of i is %u\n",i);
						//printf("invalid ring tone count is  %u \n",invalid_Ringtone_Count);
						
					}
				}
				}	
			//printf("number of rings detected %u\n", number_Of_Rings);
			
			printf("r: %u\n",number_Of_Rings);
			
			if ( timeout_Flag==1 )
			{

				timeout_Flag= 0 ; 
				TIM_DeInit(TIM4);
				if ( number_Of_Rings== 0)
				{
					//  user busy 
					printf("user busy !\n");
					PAout(5) = 0;
					return line_Busy ; 
				}
				
				else if ((number_Of_Rings>0)&&(PBin(9)==1))
				{
					// user picked up
					printf("user Picked up!\n");
					return pickedUp; 
				}
			
			}
			if ( number_Of_Rings>rings)
			{
				// timeout as the when number of rings is bigger than rings ( set by user) , no one is answering 
				TIM_DeInit(TIM4);
				printf(" no one answering\n");
				// hang up
				PAout(5) = 0;
				return no_Reply; 
			}
			}
		}
}
u8 callout_Detection_withoutComment(u8 rings)
{
	u8 i ;
	u8 number_Of_Rings;
	u8 valid_Ringtone_Tracker;
	u8 invalid_Ringtone_Count;
	u8 firstRingTone_Detected;
	firstRingTone_Detected = 1;
	invalid_Ringtone_Count=0;
	valid_Ringtone_Tracker = 0 ;
	i =0;
	number_Of_Rings=0; 
	// add a 5 second timer 
	// 5 second timer for detecting if there's ringtone or not
	timer_Init(12000,36000-1);
	while (1){
		// needs to add the flag for the temperature and heat for condition checking if breaking this while loop to dialing 
		theStart:

	if (PBin(9))
	{
		delay_ms(50);
		// Checking if high to low
		if (!PBin(9))
		{
			printf("in second if\n");
			// first delay to determine if going into low voltage sequence
			delay_ms(50);
			if ( !PBin(9))
			{
				//De init the first 3 second timer 
				// Start of low voltage sequence
			    printf("At the start of the low voltage sequence\n");
				printf("timer started\n");
				if ( firstRingTone_Detected) 
				{
					TIM_DeInit(TIM4);
					firstRingTone_Detected=0;
				}
				if ( valid_Ringtone_Tracker == 1)
				{
					valid_Ringtone_Tracker = 0;
					TIM_DeInit(TIM4);
				}
			}
			else if ( PBin(9) ==1)
			{
				// Does not go into the low voltage sequence
				goto theStart;
			}
			// check if going into the low voltage sequence 
			while ( !PBin(9))
			{
				low_Voltage:
				delay_ms(100);
			}
			// end of low voltage sequence
			// Start of high voltage sequence
			printf("at the start of high voltage sequence\n");
			if ( PBin(9) ==0)
			{
				goto low_Voltage;
			}
			else
			{
				for ( i= 0 ;i<=15;i++)
				{
					delay_ms(100);
					if ( PBin(9))
					{
						continue;
					}
					else 
					{
						break;
					}
				}
				if ( i ==16)
				{
					//TIM_DeInit(TIM4);
					number_Of_Rings++;
					timer_Init(8000,36000-1);
					valid_Ringtone_Tracker = 1;
					printf("current number of rings %u\n", number_Of_Rings);
				}
				else
				{
					invalid_Ringtone_Count++;
					printf("value of i is %u\n",i);
					printf("invalid ring tone count is  %u \n",invalid_Ringtone_Count);
					
				}
			}
			}	
		printf("number of rings detected %u\n", number_Of_Rings);
		if ( timeout_Flag==1 )
		{

			timeout_Flag= 0 ; 
			TIM_DeInit(TIM4);
			if ( number_Of_Rings== 0)
			{
				//  user busy 
				printf("user busy !\n");
				return 0 ; 
			}
			else if ((number_Of_Rings>0)&&(PBin(9)==1))
			{
				// user picked up
				printf("user Picked up!\n");
				return 2; 
			}
		}
		if ( number_Of_Rings>rings)
		{
			// timeout as the when number of rings is bigger than rings ( set by user) , no one is answering 
			TIM_DeInit(TIM4);
			printf(" no one answering\n");
			return 1; 
		}
		}
	}
}


