#include "24cxx.h"
#include "SysTick.h"
#include "string.h"
#include "usart.h"
u8 size_Of_Password = 6;
char password_Array[15];
 u8  password_In_24C02_Flag_Changed = 0;
 u8 number_Of_Recall = 1;
/*******************************************************************************
* 函 数 名         : AT24CXX_Init
* 函数功能		   : AT24CXX初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void AT24CXX_Init(void)
{
	IIC_Init();//IIC初始化
}

/*******************************************************************************
* 函 数 名         : AT24CXX_ReadOneByte
* 函数功能		   : 在AT24CXX指定地址读出一个数据
* 输    入         : ReadAddr:开始读数的地址 
* 输    出         : 读到的数据
*******************************************************************************/
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}
	else 
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   // Send the address of the IC 
	} 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   // sending the address of the data to store at in the IC (0~255)
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}

/*******************************************************************************
* 函 数 名         : AT24CXX_WriteOneByte
* 函数功能		   : 在AT24CXX指定地址写入一个数据
* 输    入         : WriteAddr  :写入数据的目的地址 
					 DataToWrite:要写入的数据
* 输    出         : 无
*******************************************************************************/
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}
	else 
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据
	} 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

/**
  * @brief Reads the value password value in 24C02 and stores it in password_Array
  * @param  none 
  * @retval 
  * @note  The first digit of the password is in byte 11 of the storage cell in 24C02
*/
void password_Filler_from24C02(void)
{ 
	u8 i ; 
	for ( i = 0 ; i<6;i++)
	{
		password_Array[i] = AT24CXX_ReadOneByte(11+i);
	}
}
u8 password_Setup(char dummy_Password_Array[],u8 dummy_size_Of_Password)
{

	u8 temp;
	u8 i = 0 ; 
	u8 password_Setup_Correctly= 0;
	//u8 length = strlen(dummy_Password_Array);
	for ( i =11;i<dummy_size_Of_Password;i++)
	{
		AT24CXX_WriteOneByte(i, dummy_Password_Array[i]);
	}
	for ( i =11;i<dummy_size_Of_Password;i++)
	{
		// temp should be the hex value of the character stored in 24c02
		temp = AT24CXX_ReadOneByte(i);
		if ( temp ==dummy_Password_Array[i])
		{
			password_Setup_Correctly = 1; 
		}
		else 
		{
			password_Setup_Correctly = 0; 
			return password_Setup_Correctly;
		}
		
	}
	return password_Setup_Correctly;
}

/**
  * @brief Prints out the current password value in both the password_Array and in the 24C02
  * @param  none 
  * @retval 
  * @note 
*/
void printout_Password_ForTesting(u8 password_Length)
{
	u8 i ;
	printf(" the password (from the 24C02) is : ");
	for ( i = 11 ; i< 17;i++)
	{
		printf("%c\n",AT24CXX_ReadOneByte(i));
	}
	for ( i = 0 ;i<password_Length;i++)
	{
		printf("value in the password array is %c\n", password_Array[i]);
	}
	printf(" the 255 cell data is %u\n", AT24CXX_ReadOneByte(255));
	printf(" the 30 cell data is %u\n", AT24CXX_ReadOneByte(30));
}
/*******************************************************************************
* 函 数 名         : AT24CXX_WriteLenByte
* 函数功能		   : 在AT24CXX里面的指定地址开始写入长度为Len的数据
					 用于写入16bit或者32bit的数据
* 输    入         : WriteAddr  :写入数据的目的地址 
					 DataToWrite:要写入的数据
					 Len        :要写入数据的长度2,4
* 输    出         : 无
*******************************************************************************/
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/*******************************************************************************
* 函 数 名         : AT24CXX_ReadLenByte
* 函数功能		   : 在AT24CXX里面的指定地址开始读出长度为Len的数据
					 用于读出16bit或者32bit的数据
* 输    入         : ReadAddr   :开始读出的地址 
					 Len        :要读出数据的长度2,4
* 输    出         : 读取的数据
*******************************************************************************/
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/*******************************************************************************
* 函 数 名         : AT24CXX_Check
* 函数功能		   : 检查AT24CXX是否正常
* 输    入         : 无
* 输    出         : 1:检测失败，0:检测成功
*******************************************************************************/
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(255);//避免每次开机都写AT24CXX			   
	if(temp==0x36)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X36);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X36)return 0;
	}
	return 1;											  
}

/*******************************************************************************
* 函 数 名         : AT24CXX_Read
* 函数功能		   : 在AT24CXX里面的指定地址开始读出指定个数的数据
* 输    入         : ReadAddr :开始读出的地址 对24c02为0~255
					 pBuffer  :数据数组首地址
					 NumToRead:要读出数据的个数
* 输    出         : 无
*******************************************************************************/
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
} 

/*******************************************************************************
* 函 数 名         : AT24CXX_Write
* 函数功能		   : 在AT24CXX里面的指定地址开始写入指定个数的数据
* 输    入         : WriteAddr :开始写入的地址 对24c02为0~255
					 pBuffer  :数据数组首地址
					 NumToRead:要读出数据的个数
* 输    出         : 无
*******************************************************************************/
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
/**
  * @brief Check if the data in the USART_Storage_Buffer is valid and if valid, store it in the 24C02
  * @param  none 
  * @retval 
  * @note The MCU sends out OK via UART when the data is valid (the first byte of the 22 byte data is 7E and the last byte is EF)and ERR via UART when data is invalid
*/
u8 check_Data_validity(void)
{
	u8 i ; 
	u8 offset_indexer_24C02 = 10;
	u8 ok_Message[2]= {0x4F,0x4B};
	u8 err_Message[3]= {0x45,0x52,0x52};
	if ( (USART1_Storeage_Buffer[0]== 0x7E)&&(USART1_Storeage_Buffer[21]==0xEF))
	{
		// valid data
		for ( i = 0 ; i<2;i++)
		{
			while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			{
			}
			USART_SendData(USART1,ok_Message[i]);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		}
		for (i = 1; i<21;i++)
		{
			AT24CXX_WriteOneByte(offset_indexer_24C02, USART1_Storeage_Buffer[i]);
			offset_indexer_24C02++;
		}
		return 1;
		
	}
	else
	{
		// valid data
		for ( i = 0 ; i<3;i++)
		{
			while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			{
			}
			USART_SendData(USART1,err_Message[i]);
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
		}
		return 0 ; 
	}
	
}




