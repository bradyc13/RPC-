#ifndef __usart_H
#define __usart_H
#include "stdio.h"
#include "system.h" 

#define USART1_REC_LEN		200  	//定义最大接收字节数 200
#define  command_Size 22
extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		//接收状态标记

extern u8 USART1_Storeage_Buffer[command_Size]; // The buffer used to store each byte of the 22 byte setting data
extern u8 buffer_Index; // The indexer for the USART1_Storeage_Buffer
extern u8 new_Data_Flag;
void USART1_Init(u32 bound);

void usart_Testing_Printing_Function(void);


#endif


