#include "74HC595.h"
#include "SysTick.h"
#include "usart.h"
#include "DF_Player.h"

u8  relay1_Status = 1;
u8  relay2_Status = 1;
u8  relay3_Status = 1;
u8  relay4_Status = 1;
u8  relay5_Status = 1;
u8 all_Relay_Status = 0xFF;
u8 timeout_char_Flag=0;
	
 
 /**
  * @brief The output function for 74HC595
  * @param  none 
  * @retval none 
  * @note 74HC595 receive 1 byte of data through serial pin, and expects the MSB first and LSB last
  * @attention 
  */
void the_74HC595_Output(u8 data)
{
	
	u8 i=0;
	
	for(i=0;i<8;i++)// iteration for 8 times
	{
		SER=data>>7;// Shifting the MSB to exclude it 
		data<<=1;// shift the next significant bit to the MSB for the next interation 
		SRCLK=0;
		delay_us(1);
		SRCLK=1;
		delay_us(1);// pulling the SRCLK pin from low to high for each bit to be pushed onto the shift register (Starting from the MSB of the data)
	}
	// Pulling the RCK pin from low to high ( rising edge) for the 74HC595 to latch that one byte data from shift register to storage register
	RCLK=0;
	delay_us(1);
	RCLK=1;

}
/**
  * @brief Initializtion of the pins for 74HC595
  * @param  none 
  * @retval none 
  * @note Pin11,12,13,14,15 are set to input mode with default as voltage low.
  * @attention 
  */
void the_74HC595_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCLK_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(SRCLK_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(SER_PORT_RCC,ENABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=RCLK_PIN;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	 
	GPIO_Init(RCLK_PORT,&GPIO_InitStructure); 

	GPIO_InitStructure.GPIO_Pin=SRCLK_PIN;   
	GPIO_Init(SRCLK_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=SER_PIN;   
	GPIO_Init(SER_PORT,&GPIO_InitStructure);
	 the_74HC595_Output(all_Relay_Status);
}
/**
  * @brief Control the relays( hardware) that are connected to the 74HC595
* @param   number_Of_the_Relay: the relay intended to control with relay 1 connected to QA pin of 74HC595, QB connected to 74HC595 and so on...
  * @retval none 
  * @note 74HC595 receive 1 byte of data through serial pin, and expects the MSB first and LSB last
  * @attention IMPORTANT: number_Of_the_Relay is supposed to be value from 1 to 8 
  */

// This function toggles the relay 
// IMPORTANT: number_Of_the_Relay is supposed to be value from 1 to 8 
void relay_Controller(u8 number_Of_the_Relay)
{
	u8 i;
	u8 temp1=0x00;
	u8 temp2=0x00;
	for(i=0;i<8;i++)
	{
		// this statement is used to check the iteration through all_Relay_Status is the intended bit ( corrosponds to the relay ex: relay 1 will be bit 0 of all_Relay_Status, relay 2 will be bit 1 of all_Relay_Status)
		if ( i==(8-number_Of_the_Relay))
		{
			temp1= all_Relay_Status>>7;
			// basically toggling
			 temp1= !temp1;
			/*if ( temp1 ==1)
			{
				temp1=0;
			}
			else if (temp1==0)
			{
				temp1=1;
			}
			*/
			//0x0000 0001
			all_Relay_Status<<=1;
			temp1=temp1<<(7-i);
			temp2 = temp2|temp1;
		}
		else 
		{
			temp1= all_Relay_Status>>7;
			all_Relay_Status<<=1;
			temp1=temp1<<(7-i);
			temp2 = temp2|temp1;
		}
	}
	all_Relay_Status= temp2;
	the_74HC595_Output(all_Relay_Status);
	printf("toggling relay #%u,\n",number_Of_the_Relay);
	printf("all_Relay_Status is %u\n",all_Relay_Status);
}
/**
  * @brief Combining the  void relay_Controller(u8 number_Of_the_Relay) function with playing audio tracks
* @param   char action_Buffer[]: size 2 array of user input. User input can be ex: 1* or 2*....
  * @retval  
  * @note 
  * @attention 
  */

void relay_Action(char action_Buffer[] )
{
	u8 conversion_of_charTo_u8;
	u16 key;
	printf("index 0 of buffer is %u/n",action_Buffer[0]);
	printf("index 0 of buffer is %u/n",action_Buffer[0]);
	
	key =((uint16_t)action_Buffer[0]<<8)|action_Buffer[1];
	conversion_of_charTo_u8= action_Buffer[0]-'0';
	switch (key)
			{
				case (('1' << 8) | '*'):
					TIM_DeInit(TIM4);
					//play_One_Track(1,1);// 使用者按下按下案件確語音
					if ((all_Relay_Status&0x01) ==1)
					{
						printf("relay 1 off\n");
						play_One_Track(3,2);
					}
					else if ((all_Relay_Status&0x01) ==0)
					{
						printf("relay 1 on\n");
						play_One_Track(3,1);
					}
					relay_Controller(conversion_of_charTo_u8);
					printf("case 1* \n");
					break;
				case (('2' << 8) | '*'):
					TIM_DeInit(TIM4);
					//play_One_Track(1,2);// 使用者按下按下案件確語音
					if ((all_Relay_Status&0x02) ==0x02)
					{
						printf("relay 2 off\n");
						play_One_Track(3,4);
					}
					else if ((all_Relay_Status&0x02) !=0x02)
					{
						printf("relay 2 on\n");
						play_One_Track(3,3);
					}
					relay_Controller(conversion_of_charTo_u8);
					printf("case 2* \n");
					break;
				case (('3' << 8) | '*'):
					TIM_DeInit(TIM4);
					//play_One_Track(1,3);// 使用者按下按下案件確語音
					if ((all_Relay_Status&0x04) ==0x04)
					{
						printf("relay3 off\n");
						play_One_Track(3,6);
					}
					else if ((all_Relay_Status&0x04) !=0x04)
					{
						printf("relay 3 on\n");
						play_One_Track(3,5);
					}
					relay_Controller(conversion_of_charTo_u8);
					printf("case 3* \n");
					break;
				case (('4' << 8) | '*'):
					TIM_DeInit(TIM4);
					//play_One_Track(1,3);// 使用者按下按下案件確語音
					if ((all_Relay_Status&0x08) ==0x08)
					{
						printf("relay 4 off\n");
						play_One_Track(3,8);
					}
					else if ((all_Relay_Status&0x08) !=0x08)
					{
						printf("relay 4 on\n");
						play_One_Track(3,7);
					}
					relay_Controller(conversion_of_charTo_u8);	
					printf("case 4* \n");
					break;
				case (('5' << 8) | '*'):
					TIM_DeInit(TIM4);
					//play_One_Track(1,3);// 使用者按下按下案件確語音
					if ((all_Relay_Status&0x10) ==0x10)
					{
						printf("relay 5 off\n");
						play_One_Track(3,10);
					}
					else if ((all_Relay_Status&0x10) !=0x10)
					{
						printf("relay 5 on\n");
						play_One_Track(3,9);
					}
					relay_Controller(conversion_of_charTo_u8);	
					printf("case 5* \n");
					break;
				case (('#' << 8) | '#'):
					TIM_DeInit(TIM4);
					//play_One_Track(1,3);// 使用者按下按下案件確語音
					printf("case quiting* \n");
					break;
				
				default :
					printf("might be invalid input");
				
			}
}
	

