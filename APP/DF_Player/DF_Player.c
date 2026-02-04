#include "DF_Player.h"
#include "SysTick.h"
#include "HP9170.h"
/**
  * @brief Initializtion of the pins for DF_Player Mini
  * @param  none 
  * @retval none 
  * @note PA2 as data output pin (Usart 2) into Rx pin of DFplayer mini, PA3 as data input pin from Tx pin of DFplayer mini
  * @attention none 
  */
void DF_Player_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF_PP ;
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING ;
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate= 9600;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b ;
	USART_InitStructure.USART_StopBits= USART_StopBits_1;
	USART_InitStructure.USART_Parity= USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl= USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode= USART_Mode_Rx|USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure);
	
	USART_Cmd(USART2,ENABLE);
}
/**
  * @brief Playing a specific track inside a specific folder 
* @param  u8 number_Of_Folder : the number of the folder ex: folder name : 001myfolder-->  number_Of_Folder should be 0x01
*@param u8 number_Of_Track: the number of the track. Ex: 001myAudio--> number_Of_Track = 0x01
  * @retval none 
  * @note none 
  * @attention none 
  */
// ! Might need to check the current version 
// ! Format：$S  VER  Len  CMD  Feedback  para1  para2  checksum(two byte with highlow bits)  $O
void play_One_Track(u8 number_Of_Folder, u8 number_Of_Track)
{	
	u8 i ;
	u16 data_Array[10];
	u8 CheckSum_HighByte;
	u8 CheckSum_LowByte;
	u16 version = DF_Player_Version;
	u16 length = command_Length;
	u16 feedback = no_Feedback;
	u16 command = play_Specific_Track;
	u16 parameter1 =  number_Of_Folder;
	u16 parameter2 = number_Of_Track;
	u16 checkSum = version+length+command+no_Feedback+parameter1+parameter2;
	checkSum= 0xFFFF-checkSum+1;
	CheckSum_HighByte= ((checkSum&0xFF00)>>8);
	CheckSum_LowByte = (checkSum&0x00FF);
	
	data_Array[0]=start_Byte;
	data_Array[1]=version;
	data_Array[2]=length;
	data_Array[3]=command;
	data_Array[4]=feedback;
	data_Array[5]=parameter1;
	data_Array[6]=parameter2;
	data_Array[7]=CheckSum_HighByte;
	data_Array[8]=CheckSum_LowByte;
	data_Array[9]=end_Byte;
	for ( i = 0 ; i<10;i++)
	{
		USART_SendData(USART2,data_Array[i]);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) { }
	}
	
	
}
void adjust_Volume(void)
{
	{	
	u8 i ;
	u16 data_Array[10];
	u8 CheckSum_HighByte;
	u8 CheckSum_LowByte;
	u16 version = DF_Player_Version;
	u16 length = command_Length;
	u16 feedback = no_Feedback;
	u16 command = 0x06;
	u16 parameter1 =  0x00;
	u16 parameter2 = 0x1E;
	u16 checkSum = version+length+command+no_Feedback+parameter1+parameter2;
	checkSum= 0xFFFF-checkSum+1;
	CheckSum_HighByte= ((checkSum&0xFF00)>>8);
	CheckSum_LowByte = (checkSum&0x00FF);
	
	data_Array[0]=start_Byte;
	data_Array[1]=version;
	data_Array[2]=length;
	data_Array[3]=command;
	data_Array[4]=feedback;
	data_Array[5]=parameter1;
	data_Array[6]=parameter2;
	data_Array[7]=CheckSum_HighByte;
	data_Array[8]=CheckSum_LowByte;
	data_Array[9]=end_Byte;
	for ( i = 0 ; i<10;i++)
	{
		USART_SendData(USART2,data_Array[i]);
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) { }
	}
	
	
}
}
void trackPlayer(char user_Dialed)
{
	switch ( user_Dialed)
			{
				case '1':
					TIM_DeInit(TIM4);
					play_One_Track(1,3);// 使用者按下按下案件確語音
				case'2':
					TIM_DeInit(TIM4);
					play_One_Track(2,2);
				case '3':
					TIM_DeInit(TIM4);
					play_One_Track(2,3);
				case'4':
					TIM_DeInit(TIM4);
					play_One_Track(2,4);
				case '5':
					TIM_DeInit(TIM4);
					play_One_Track(2,5);	
			}
}
void trackPlayer_V2(void )
{
	u16 key =((uint16_t)sequence_Buffer[0]<<8)|sequence_Buffer[1];
	switch (key)
			{
				case (('1' << 8) | '*'):
					TIM_DeInit(TIM4);
					play_One_Track(1,1);// 使用者按下按下案件確語音
					printf("case 1* \n");
					break;
				case (('2' << 8) | '*'):
					TIM_DeInit(TIM4);
					play_One_Track(1,2);// 使用者按下按下案件確語音
					printf("case 2* \n");
					break;
				case (('3' << 8) | '*'):
					TIM_DeInit(TIM4);
					play_One_Track(1,3);// 使用者按下按下案件確語音
					printf("case 3* \n");
					break;
				case (('4' << 8) | '*'):
					TIM_DeInit(TIM4);
					play_One_Track(1,3);// 使用者按下按下案件確語音
					printf("case 4* \n");
					break;
				case (('5' << 8) | '*'):
					TIM_DeInit(TIM4);
					play_One_Track(1,3);// 使用者按下按下案件確語音
					printf("case 5* \n");
					break;
				case (('#' << 8) | '#'):
					TIM_DeInit(TIM4);
					play_One_Track(1,3);// 使用者按下按下案件確語音
					printf("case quiting* \n");
					break;
				
				default :
					printf("might be invalid input");
				
			}
}
