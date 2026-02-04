#include "HP9170.h"
#include "system.h"
#include "SysTick.h"
#include "DF_player.h"
#include "timer.h"
u8 timeout_Flag= 0 ; 
char sequence_Buffer[2];
char user_InputArray[]= {'?','?' ,'?','?','?','?'};
 u8 user_InputArray_Length = 0; 
 /**
  * @brief Initializtion of the pins for HP9170
  * @param  none 
  * @retval none 
  * @note Pin11,12,13,14,15 are set to input mode with default as voltage low.
  * @attention 
  */
 void HP9170_Init(void)
 {
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	 
	 GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	 GPIO_Init(GPIOB,&GPIO_InitStructure);
	 
 }
 /**
  * @brief  Decoding data line into numbers such as 0~9, '#' and '*' 解碼Data line 成數字和'#''*'
  * @param  none 
  * @retval Returns a character value. Value can range from 0~9,# and *
  * @note  Since the data pins (D3~D0) represents binary arithmetic, where D3=0,D2=0,D1=0,D0=1 represents '1' when decode, D3=0,D2=0,D1=1,D0=0 represents '2' and so on.
  * To be aware that, 0 is decoded when  D3=1,D2=0,D1=1,D0=0. '*' decoded when  D3=1,D2=0,D1=1,D0=1. '#' is decoded when  D3=1,D2=1,D1=0,D0=0.
 
  *因為data pins (D3~D0) 實際上是2進制所以當D3=0,D2=0,D1=0,D0=1代表 '1'，D3=0,D2=0,D1=1,D0=0 represents '2' 
    且解碼出0  when  D3=1,D2=0,D1=1,D0=0. 解碼出'*'when  D3=1,D2=0,D1=1,D0=1.解碼出 '#' when  D3=1,D2=1,D1=0,D0=0.
  */
 
char decoding_One_Tone(void)
{ 	
	u8 temp_2Bit_Number= 0;
	// DV is high when the keypad is pressed aka the data through D0~3 are valid 
	// while read_DV
	while ( (!read_DV_Signal) || timeout_Flag)
	{
		if ( timeout_Flag)
		{
			// resetting timeout Flag 
			timeout_Flag= 0;
			TIM_DeInit(TIM4);
			printf("TimedOUT!!!\n");
			return '?';
		}
	}
	if (read_DV_Signal)
	{
		if (read_HT9170_D0)
		{
			temp_2Bit_Number+=1;
		}
		if (read_HT9170_D1)
		{
			temp_2Bit_Number+=2;
		}
		if (read_HT9170_D2)
		{
			temp_2Bit_Number+=4;
		}
		if (read_HT9170_D3)
		{
			temp_2Bit_Number+=8;
		}
	}
	switch  (temp_2Bit_Number){
		case 1:
			printf("return value is 1");
			return '1';
		case 2:
			printf("return value is 2");
			return '2';
		case 3:
			printf("return value is 3");
			return '3';
		case 4:
			printf("return value is 4");
			return '4';
		case 5:
			printf("return value is 5");
			return '5';
		case 6:
			printf("return value is 6");
			return '6';
		case 7:
			printf("return value is 7");
			return '7';
		case 8:
			printf("return value is 8");
			return '8';
		case 9:
			printf("return value is 9");
			return '9';
		case 10:
			printf("return value is 10");
			return '0';
		case 11:
			printf("return value is 11");
			return '*';
		case 12:
			printf("return value is 12");
			return '#';	
		default :
			printf("return value is ?");
			return '?';// error return
	}
		
}


 /**
  * @brief  Decoding the Q0~Q3 data output from the HT9170 
  * @param  none 
  * @retval 
  * @note This function waits for the DV pin to go low ( user release the number button) to calculate the number being pressed
	*@note This function does NOT set the timer. Ir only 1. resets the flag value when timer countdown triggers interrupt 2. De-initialize the clock 3. hang up the phone (pulling the Pin 5 to low)
  */
char decoding_One_Tone_with_DVcontrol(void)
{ 	u8 i ;
	u8 valid_tone_in=0;
	u8 temp_2Bit_Number= 0;
	// DV is high when the keypad is pressed aka the data through D0~3 are valid 
	while (1&&! timeout_Flag)
	{	
		theStart:
		if ( read_DV_Signal ==0)
		{
			delay_ms(5);
			// checking rising edge
			if ( read_DV_Signal ==1)
			{
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
					delay_ms(5);
					if ( timeout_Flag==1)
					{
						goto theEnd;
					}
				}
				if (  read_DV_Signal==0)
				{
					for (  i=0; i<=2;i++)
					{
						delay_ms(5);
						if ( read_DV_Signal==0)
						{
							continue;
						}
					}
				}
				else if ( read_DV_Signal==1)
				{
					goto highVoltage;
				}
				if (i ==3)
				{	
					TIM_DeInit(TIM4);
					valid_tone_in=1;
					printf("valid tone in\n");
					break;
				}
			}
		}
	}
	theEnd:
	if ( timeout_Flag)
	{	// if timeout the timer gets de-initialized and hanged up 
		timeout_Flag= 0;
		TIM_DeInit(TIM4);
		PAout(5) = 0;
		printf("hook reseted");
		printf("TimedOUT!!!\n");
	}
	if (valid_tone_in)
	{
		if (read_HT9170_D0)
		{
			temp_2Bit_Number+=1;
		}
		if (read_HT9170_D1)
		{
			temp_2Bit_Number+=2;
		}
		if (read_HT9170_D2)
		{
			temp_2Bit_Number+=4;
		}
		if (read_HT9170_D3)
		{
			temp_2Bit_Number+=8;
		}
	}
	switch  (temp_2Bit_Number){
		case 1:
			//play_One_Track(1, 1);
			printf("return value is 1\n");
			return '1';
		case 2:
			//play_One_Track(1, 2);
			printf("return value is 2\n");
			return '2';
		case 3:
			//play_One_Track(1, 3);
			printf("return value is 3\n");
			return '3';
		case 4:
			//play_One_Track(1, 4);
			printf("return value is 4\n");
			return '4';
		case 5:
			//play_One_Track(1, 5);
			printf("return value is 5\n");
			return '5';
		case 6:
			//play_One_Track(1, 6);
			printf("return value is 6\n");
			return '6';
		case 7:
			//play_One_Track(1, 7);
			printf("return value is 7\n");
			return '7';
		case 8:
			//play_One_Track(1, 8);
			printf("return value is 8\n");
			return '8';
		case 9:
			//play_One_Track(1, 9);
			printf("return value is 9\n");
			return '9';
		case 10:
			//play_One_Track(1, 10);
			printf("return value is 0\n");
			return '0';
		case 11:
			//play_One_Track(1, 11);
			printf("return value is 11\n");
			return '*';
		case 12:
			//play_One_Track(1, 12);
			printf("return value is 12\n");
			return '#';	
		default :
			//play_One_Track(2,6);
			printf("return value is ?\n");
			return '?';// error return
	}
}

void array_storer(char someArray[], u8 size_Of_Array,char new_Data)
{
	u8 starting_index=0;
	if ( starting_index!=(size_Of_Array))
			{
				someArray[starting_index]=new_Data;
				starting_index++;
			}
			else 
			{
				someArray[0]=sequence_Buffer[1];
				someArray[1]= new_Data;
			}
}
/**
  * @brief The sequence where the user press which relay to control and what to do with it
  * @param  none 
  * @retval 
  * @note This algorithm utilize the state machine.
*State 000 is the default state where no button is pressed. The goal is to reach state 010 where the user pressed the button to tell the MCU which relay to control and what to do with it.
Or reaching state 100 is also the final state where the user presses # twice to quit
  */
void two_User_Input_Sequence_V2(void)
{
	
	char temp1;
	u8 starting_index = 0;
	u8 size_Of_Buf_Array=2;
	// the variable to keep track of the current state 
	u16  two_User_Input_Collected_Flag =0x000;
	while ((two_User_Input_Collected_Flag!=STATE_010)&&(two_User_Input_Collected_Flag!=STATE_100)&&temp1!='?')
	{
		//default state machine 000
		timer_Init(50000,14400-1);
		temp1= decoding_One_Tone_with_DVcontrol();
		// this if statement needs to be checked and tested again
		if ( temp1 =='?')
		{
			sequence_Buffer[0]='?';
		}
		if ( (temp1=='1'||temp1=='2'||temp1=='3'||temp1=='4'||temp1=='5')&& two_User_Input_Collected_Flag==STATE_000)
		{// going into state machine 001
			printf("inside state 001\n");
			if ( starting_index!=(size_Of_Buf_Array))
			{
				sequence_Buffer[starting_index]=temp1;
				starting_index++;
			}
			else 
			{
				sequence_Buffer[0]=sequence_Buffer[1];
				sequence_Buffer[1]= temp1;
			}
			two_User_Input_Collected_Flag=STATE_001;
			continue;
		}
		else if (temp1=='#'&& two_User_Input_Collected_Flag==STATE_000)
		{// going into state machine 011
			printf("inside state 011\n");
			if ( starting_index!=(size_Of_Buf_Array))
			{
				sequence_Buffer[starting_index]=temp1;
				starting_index++;
			}
			else 
			{
				sequence_Buffer[0]=sequence_Buffer[1];
				sequence_Buffer[1]= temp1;
			}
			two_User_Input_Collected_Flag=STATE_011;
			continue;
		}
		else if ( (temp1=='*'||temp1=='6'||temp1=='7'||temp1=='8'||temp1=='9'||temp1=='0')&& two_User_Input_Collected_Flag==STATE_000)
		{// going back to state machine 000
			printf("going back  state 000\n");
			two_User_Input_Collected_Flag=STATE_000;
			continue;
		}
		if ( two_User_Input_Collected_Flag==STATE_001)
		{// if state machine == 001
			if ( temp1=='*')
			{// going into state machine 010
				printf("inside state 010\n");
				if ( starting_index!=(size_Of_Buf_Array))
				{
					sequence_Buffer[starting_index]=temp1;
					starting_index++;
				}
				else 
				{
					sequence_Buffer[0]=sequence_Buffer[1];
					sequence_Buffer[1]= temp1;
				}
				two_User_Input_Collected_Flag=STATE_010;
				//PAout(5)=0;
				continue;
			}
			else if (temp1=='#')
			{//going into state machine 011
				printf("inside state 011\n");
				if ( starting_index!=(size_Of_Buf_Array))
				{
					sequence_Buffer[starting_index]=temp1;
					starting_index++;
				}
				else 
				{
					sequence_Buffer[0]=sequence_Buffer[1];
					sequence_Buffer[1]= temp1;
				}
				two_User_Input_Collected_Flag=STATE_011;
				continue;
			}
			else if (temp1=='6'||temp1=='7'||temp1=='8'||temp1=='9'||temp1=='0')
			{//going into state machine 000
				printf("going back state 000\n");
				if ( starting_index!=(size_Of_Buf_Array))
				{
					sequence_Buffer[starting_index]=temp1;
				}
				else 
				{
					sequence_Buffer[0]=sequence_Buffer[1];
					sequence_Buffer[1]= temp1;
				}
				two_User_Input_Collected_Flag=STATE_000;
				continue;
			} 
			else 
			{
				printf("staying at state 001\n");
				if ( starting_index!=(size_Of_Buf_Array))
				{
					sequence_Buffer[starting_index]=temp1;
				}
				else 
				{
					sequence_Buffer[0]=sequence_Buffer[1];
					sequence_Buffer[1]= temp1;
				}
				two_User_Input_Collected_Flag=STATE_001;
				continue;
			}
		}
		if ( two_User_Input_Collected_Flag==STATE_011)
		{// if state machine ==011
			printf("inside state 011\n");
			if ( temp1=='1'|| temp1=='2'|| temp1=='3'|| temp1=='4'|| temp1=='5')
			{// going into state machine 001
				
				if ( starting_index!=(size_Of_Buf_Array))
				{
					sequence_Buffer[starting_index]=temp1;
					starting_index++;
				}
				else 
				{
					sequence_Buffer[0]=sequence_Buffer[1];
					sequence_Buffer[1]= temp1;
				}
				two_User_Input_Collected_Flag=STATE_001;
				continue;
			}
			else if (temp1=='*'||temp1=='0'||temp1=='6'||temp1=='7'||temp1=='8'||temp1=='9')
			{// going into state machine 000
				printf("going into state 000");
				if ( starting_index!=(size_Of_Buf_Array))
				{
					sequence_Buffer[starting_index]=temp1;
					starting_index++;
				}
				else 
				{
					sequence_Buffer[0]=sequence_Buffer[1];
					sequence_Buffer[1]= temp1;
				}
				two_User_Input_Collected_Flag=STATE_000;
				continue;
			}
			else if (temp1=='#')
			{// going into state machine 100
				printf("inside state 100\n");
				if ( starting_index!=(size_Of_Buf_Array))
				{
					sequence_Buffer[starting_index]=temp1;
					starting_index++;
				}
				else 
				{
					sequence_Buffer[0]=sequence_Buffer[1];
					sequence_Buffer[1]= temp1;
				}
				two_User_Input_Collected_Flag=STATE_100;
				PAout(5) = 0;
				continue;
			}
		}
	}	
	printf("the end");
}
/**
  * @brief  This function scans for the value the user presses  
  * @param  none 
  * @retval note 
  * @note When "#" is pressed it represents "enter". When "*" is pressed it represents the password the user is typing in is clear, and start from the first digit of the password
  */
void user_Typein_Password(void)
{
	char temp;
	u8 user_InputArray_Index=0;
	u8 user_Done_Typing =0;
	u8 i = 0 ;
	user_InputArray_Length=0;
	while (!user_Done_Typing)
	{
		timer_Init(50000,14400-1);
		temp= decoding_One_Tone_with_DVcontrol();
		if ( temp =='#')
		{
			// enter being pressed 
			printf(" enter is pressed\n");
			break;
		}
		else if (temp =='*')
		{
			// clear is being pressed
			for ( i = 0; i < 7; i++) 
			{
				user_InputArray[i] = '?';
			}
			user_InputArray_Index=0;
			user_InputArray_Length=0;
			printf("user password input reseted\n");
			for ( i = 0 ; i <user_InputArray_Length;i++)
			{
				printf("%c",user_InputArray[i]);
			}
		}
		else if (temp =='?')
		{
			// timeout, user no response
			printf("user no response timeout!\n");
			PAout(5)= 0 ; 
			break;
		}
		else 
		{
			if ( user_InputArray_Index==6)
			{
				// offset the index when it is about to overflow 
				// this happens when the user inputs more than 6 passwords,
				// from 7 on it only change the 6th digit of the password. ex: 123456 and the user presses 7, the user input password become 123457
				user_InputArray_Index=5;
				user_InputArray_Length-=1;
			}
			user_InputArray[user_InputArray_Index]= temp; 
			user_InputArray_Index+=1;
			user_InputArray_Length+=1;
			printf("one input detected from keypad %c\n",temp);
		}
	}
	printf("the usertyped in password is \n");
	for ( i = 0 ; i <user_InputArray_Length;i++)
	{
		printf("%c",user_InputArray[i]);
	}
}
/**
  * @brief  Check if the user's password input is same as the one store in the 24C02
* @param  char password_24C02[] : the 6 digit password value stored in the 24C02, u8 length: the length of the 6 digit value stored in the 24C02
  * @retval return 1: The password matches the one stored in the 24C02, else returns 0.
  * @note
  */
u8 password_Matches_orNot(char password_24C02[], u8 length  )
{
	// return 1: password matches
	// return 0 : password doesn't match
	u8 i = 0 ; 
	u8 j = 0 ; 
	// This for loop is for repeating the user input password 
	// each number audio track is set to play 1000ms apart to avoid audio overlapping
	for ( j = 0 ; j<user_InputArray_Length;j++)
	{
		switch( user_InputArray[j])
		{
			case '1':
				play_One_Track(1, 1);
				break;
			case '2':
				play_One_Track(1, 2);
				break;
			case '3':
				play_One_Track(1, 3);
				break;
			case '4':
				play_One_Track(1, 4);
				break;
			case '5':
				play_One_Track(1, 5);
				break;
			case '6':
				play_One_Track(1, 6);
				break;
			case '7':
				play_One_Track(1, 7);
				break;
			case '8':
				play_One_Track(1, 8);
				break;
			case '9':
				play_One_Track(1, 9);
				break;
			case '0':
				play_One_Track(1, 10);
				break;
				
		}
		delay_ms(1000);
			
	}
	if ( length!=user_InputArray_Length)
	{
		// if the password lenght in 24c02 and the length user typed in doesn't match--> return 0
		play_One_Track(2, 3);
		printf("password does not match\n");
		return 0 ; 
	}
	for ( i= 0 ; i<user_InputArray_Length;i++)
	{
		if ( password_24C02[i]!=user_InputArray[i])
		{
			play_One_Track(2, 3);
			printf("password does not match\n");
			return 0 ; 
		}
	}
	play_One_Track(2, 4);
	printf("password matches! \n");
	return 1; 
}
/**
  * @brief  This function creates the sequence of letting the user typing in the password
* @param  char password_24C02[] : the 6 digit password value stored in the 24C02, u8 length: the length of the 6 digit value stored in the 24C02
  * @retval return 1: The password matches the one stored in the 24C02, else returns 0.
  * @note This function pulls PA5 to low ( hanging up the phone) when the user fails to input the correct password.
  */
u8 password_Sequence ( char password_24C02[], u8 length) 
{
	u8 number_Of_Tries = 0 ; 
	u8 max_Tries =2;
	u8 attempt_Result= 0 ; 
	for ( number_Of_Tries = 0 ; number_Of_Tries<max_Tries;number_Of_Tries++)
	{
		
		user_Typein_Password();
		attempt_Result=password_Matches_orNot(password_24C02, length);
		if (attempt_Result)
		{
			printf("user attempt success!\n");
			return 1; 
		}
		else 
		{
			if (number_Of_Tries==0)
			{
				play_One_Track(2, 4);
			}
			printf("user attempt not success!\n");
		}
	}
	PAout(5) = 0; 
	return 0 ; 
}
