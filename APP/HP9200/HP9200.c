#include "HP9200.h"
#include "timer.h"
#include "HP9170.h"
char phone_Number_Array[10];
char home_phone_Number_Array[8];
u8 control_Enter_Flag = 0 ;
void HP9200_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=HT9200_Chip_Enable_Pin |HT9200_DATA_Pin|HT9200_CLK_Pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	HT9200_DATA	 = 1;
	HT9200_CLK = 1 ; 
	HT9200_Chip_Enable= 1;
	
}
void dial_one_number( char dialed_character)	
{	
	u8 i ;
	u8 dialed_Array[5] ;
	//u8 dialed_Array[5];

	switch(dialed_character)
	{
		case '1':		
			dialed_Array[0] = 1;
			dialed_Array[1] = 0;
			dialed_Array[2] = 0;
			dialed_Array[3] = 0;
			dialed_Array[4] = 0;
			
			break;
		case '2':
			dialed_Array[0] = 0;
			dialed_Array[1] = 1;
			dialed_Array[2] = 0;
			dialed_Array[3] = 0;
			dialed_Array[4] = 0;
			
			break;
		case '3':
			
			dialed_Array[0] = 1;
			dialed_Array[1] = 1;
			dialed_Array[2] = 0;
			dialed_Array[3] = 0;
			dialed_Array[4] = 0;
			
			
			break;
		case '4':
			
			dialed_Array[0] = 0;
			dialed_Array[1] = 0;
			dialed_Array[2] = 1;
			dialed_Array[3] = 0;
			dialed_Array[4] = 0;
			
			break;
		case '5':
			
			dialed_Array[0] = 1;
			dialed_Array[1] = 0;
			dialed_Array[2] = 1;
			dialed_Array[3] = 0;
			dialed_Array[4] = 0;
			
			break;
		case '6':
			
			dialed_Array[0] = 0;
			dialed_Array[1] = 1;
			dialed_Array[2] = 1;
			dialed_Array[3] = 0;
			dialed_Array[4] = 0;
			
			break;
		case '7':
			
			dialed_Array[0] = 1;
			dialed_Array[1] = 1;
			dialed_Array[2] = 1;
			dialed_Array[3] = 0;
			dialed_Array[4] = 0;
			
			break;
		case '8':
			
			dialed_Array[0] = 0;
			dialed_Array[1] = 0;
			dialed_Array[2] = 0;
			dialed_Array[3] = 1;
			dialed_Array[4] = 0;
			
			break;
		case '9':
			
			dialed_Array[0] = 1;
			dialed_Array[1] = 0;
			dialed_Array[2] = 0;
			dialed_Array[3] = 1;
			dialed_Array[4] = 0;
			
			break;
		case '0':
			
			dialed_Array[0] = 0;
			dialed_Array[1] = 1;
			dialed_Array[2] = 0;
			dialed_Array[3] = 1;
			dialed_Array[4] = 0;
			
			break;
		case '*':
			
			dialed_Array[0] = 1;
			dialed_Array[1] = 1;
			dialed_Array[2] = 0;
			dialed_Array[3] = 1;
			dialed_Array[4] = 0;
			
			break;
		case '#':
			dialed_Array[0] = 0;
			dialed_Array[1] = 0;
			dialed_Array[2] = 1;
			dialed_Array[3] = 1;
			dialed_Array[4] = 0;
			
			break;
	}
	
	HT9200_Chip_Enable= 0;
		for ( i = 0 ; i<5;i++)
		{
			HT9200_CLK = 1; 
			HT9200_DATA= 1; 
			delay_us(5);
			// data is latch at the falling edge of HT9200_CLK
			if ( dialed_Array[i])
			{
				
				HT9200_DATA= 1;
				HT9200_CLK= 0;
				delay_us(5);
			}
			else if (dialed_Array[i]==0)
			{
				HT9200_DATA= 0;
				HT9200_CLK= 0;
				delay_us(5);
		
			}	
		}
		
		// DTMF is produced right at the rising edge of the CLK after 5 digit is latched into HP9200
		HT9200_CLK = 1;
		// DTMF producing for 200ms 
		delay_ms(200);
		HT9200_Chip_Enable= 1;
		// wait 200 ms for the gaps in between DTMFs
		delay_ms(200);
			
}
void dial_Phone_Number_all_Digit(char some_Phone_Number [])
{
	u8 i ; 
	PAout(5)=1;
	for ( i = 0 ;i<10;i++)
	{
		dial_one_number(phone_Number_Array[i]);
	}
}
void home_dial_Phone_Number_all_Digit(char some_Phone_Number [])
{
	u8 i ; 
	PAout(5)=1;
	for ( i = 0 ;i<8;i++)
	{
		dial_one_number(home_phone_Number_Array[i]);
	}
}


void goingInto_Control_Input_Reader(void)
{
	char temp; 
	u16 control_Flag = 0x000;
	while ( (control_Flag!=0x010)&&(control_Flag!=0x001)&&(temp != '?'))
	{
		retry_Point:
		timer_Init(50000,14400-1);
		temp= decoding_One_Tone_with_DVcontrol();
		if ( temp =='*')
		{
			
			control_Enter_Flag = 1;
			control_Flag = 0x010;
		}
		else if (temp == '#')
		{
			
			control_Enter_Flag =0;
			control_Flag = 0x001;
			// verify if this do what was intended ( quit the program when # is pressed) 
			PAout(5) = 0; 
		}
		else if (temp =='?')
		{
			PAout(5) = 0; 
			break;
		}
		else 
		{
			control_Flag = 0x000;
			goto retry_Point;
		}
	}
}
void dial_Phone_Number_all_Digit_ext(char some_Phone_Number [])
{
	u8 i ; 

	for ( i = 0 ;i<10;i++)
	{
		dial_one_number(phone_Number_Array[i]);
	}
	play_One_Track(3,1);
	goingInto_Control_Input_Reader();
	if ( control_Enter_Flag==1)
	{
	}
	else if (control_Enter_Flag==0)
	{
	}
	
}
void phoneNumber_Filler_from24C02(void)
{
	u8 i ; 
	for ( i = 0 ; i <10;i++)
	{
		phone_Number_Array[i]=AT24CXX_ReadOneByte(20+i);
	}
}
void home_phoneNumber_Filler_from24C02(void)
{
	u8 i ; 
	for ( i = 0 ; i <8;i++)
	{
		home_phone_Number_Array[i]=AT24CXX_ReadOneByte(20+i);
	}
}

void redialing_Sequence(void)
{

}
