#ifndef _24cxx_H
#define _24cxx_H

#include "system.h"
#include "iic.h"


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

//开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C02
// password_Array is the array used for comparision to check if the user input is the right password
extern  char password_Array[15];
// size of password default is set to 6.
extern u8 size_Of_Password;
extern u8 password_In_24C02_Flag_Changed;
extern u8 number_Of_Recall; 


u8 AT24CXX_ReadOneByte(u16 ReadAddr);							//指定地址读取一个字节
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len);					//指定地址开始读取指定长度数据
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度的数据
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的数据


u8 password_Setup(char dummy_Password_Array[],u8 dummy_size_Of_Password);
void printout_Password_ForTesting(u8 password_Length);
u8 check_Data_validity(void); // Checking if the data array is valid or not ( needs to start with 0x7e and end with 0xEF)
void password_Filler_from24C02(void); // Filling the password_Array with actually password data in 24C02
u8 AT24CXX_Check(void);  //检查器件
void AT24CXX_Init(void); //初始化IIC


#endif
