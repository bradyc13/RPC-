#include "ds18b20.h"
#include "SysTick.h"
#include "DF_Player.h"
# include "usart.h"
u8 temperature_Status_Token = 1;
// overHeating_Task_Waiting is a flag to mark if overHeating was dealt with already or not 
u8 overHeating_Task_Waiting = 0;
/**
  * @brief Defining the DQ pin as input for DS18B20
* @param  none 
  * @retval none 
  * @note none 
  * @attention The input pin is define with input pull up mode as the DQ pin should be default voltage high, and pulled down by the DS18B20 when needed 
  */
void DS18B20_IO_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
}

/**
  * @brief Defining the DQ pin as output for DS18B20
* @param  none 
  * @retval none 
  * @note none 
  * @attention The output pin is defined with push-pull mode as the MCU should be able to drive low or high as demanded.
  */
void DS18B20_IO_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
}

/**
  * @brief Master (MCU) reset pulse
* @param  none 
  * @retval none 
  * @note none 
  * @attention The reset pulse sequence consists of the MCU driving the DQ pin low for a minimum of 480us and release the DQ pin afterwards(letting the DQ pin going back to high) 
  */
void DS18B20_Reset(void)	   
{                 
	DS18B20_IO_OUT(); //SET PG11 OUTPUT
	DS18B20_DQ_OUT=0; //À­µÍDQ
	delay_us(750);    //À­µÍ750us
	DS18B20_DQ_OUT=1; //DQ=1 
	delay_us(15);     //15US
}

/**
  * @brief Slave (DS18B20) presence pulse 
* @param  none 
  * @retval if return 1 represents the MCU can't detect the presence of the slave, otherwise if the MCU can detect the presence of the slave it returns 0 
  * @note none 
  * @attention After the Slave detects the reset pulse sent by the master (MCU), it acknowledges it by drive the DQ line low for 60-240us
  */
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();//SET PG11 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

/**
  * @brief Reading of bit sequence 
* @param  none 
* @retval data : the bit tried to be read 
  * @note none 
  * @attention 
  */
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
	u8 data;
	DS18B20_IO_OUT();//SET PG11 OUTPUT
	DS18B20_DQ_OUT=0; 
	delay_us(2);
	DS18B20_DQ_OUT=1; 
	DS18B20_IO_IN();//SET PG11 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
	else data=0;	 
	delay_us(50);           
	return data;
}

/**
  * @brief Reading a byte from the ScratchPad
* @param  none 
  * @retval The byte that was read
  * @note none 
  * @attention The reading starts from the least significant byte in the scratchpad 
  */
u8 DS18B20_Read_Byte(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
		j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}

/**
  * @brief Writing one byte to the slave
* @param u8 dat : the one byte data intended to be sent 
  * @retval none 
  * @note none 
  * @attention 
  */
void DS18B20_Write_Byte(u8 dat)     
{             
	u8 j;
    u8 testb;
	DS18B20_IO_OUT();//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT=0;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT=0;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT=1;
            delay_us(2);                          
        }
    }
}

/**
  * @brief The transcaction sequence of Intialization, ROM command, and DS18B20 function command (convert).  
* @param  none 
  * @retval none 
  * @note none 
  * @attention none 
  */
void DS18B20_Start(void)// ds1820 start convert
{   						               
    DS18B20_Reset();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
} 

/**
  * @brief Pin Initializtion for the DS18B20
* @param  none 
  * @retval returning 1 as the MCU can't recognize DS18B20, returning 0 as the MCU can recognize DS18B20 
  * @note none 
  * @attention none
  */ 
u8 DS18B20_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(DS18B20_PORT_RCC,ENABLE);

	GPIO_InitStructure.GPIO_Pin=DS18B20_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(DS18B20_PORT,&GPIO_InitStructure);
 
 	DS18B20_Reset();
	return DS18B20_Check();
}  

/**
  * @brief Getting the temperature from the slave 
* @param  none 
  * @retval the temperature measured 
  * @note none 
  * @attention It is very important to follow this sequence every time the DS18B20 is accessed, as the DS18B20 will not respond.
*The steps are: Step 1: Initializtion, Step 2 ROM command, and Step 3 DS18B20 Function Command 
  */
float DS18B20_GetTemperture(void)
{
    u16 temp;
	u8 a,b;
	float value;
    DS18B20_Start();                    // ds1820 start convert
	
    DS18B20_Reset();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// sending command from the MCU to slave for allowing to read the scratchpad	    
    a=DS18B20_Read_Byte(); // LSB   
    b=DS18B20_Read_Byte(); // MSB   
	temp=b;
	temp=(temp<<8)+a;
    if((temp&0xf800)==0xf800)
	{
		temp=(~temp)+1;
		value=temp*(-0.0625);
	}
	else
	{
		value=temp*0.0625;	
	}
	return value;    
}

/**
  * @brief Detect and play audio track to report the temperature status
* @param  low_Temperature_Value: the lowest temperature threshold, medium_Temperature_Value: medium temperature threshold, high_Temperature_Value: the hightest temperature threshold
  * @retval none 
  * @note none 
  * @attention none 
  */
void heat_Detection(u8 low_Temperature_Value, u8 medium_Temperature_Value, u8 high_Temperature_Value)
{	
	float measured_Temperature;
	measured_Temperature= DS18B20_GetTemperture();
	printf("temperature :%.2f \n",measured_Temperature);
	if ( measured_Temperature<  low_Temperature_Value)
	{
		temperature_Status_Token = 1 ; 
		
		// no track played
	}
	else if ((low_Temperature_Value<= measured_Temperature &&measured_Temperature<medium_Temperature_Value))
	{
		// Play audio track for temperature higher low temperature 
		
		temperature_Status_Token= 2;
		
	}
	else if ((medium_Temperature_Value<= measured_Temperature &&measured_Temperature<high_Temperature_Value) )
	{
		
		temperature_Status_Token=3;
		
	}
	else if ( (high_Temperature_Value<= measured_Temperature))
	{
		//high temperature 
		temperature_Status_Token= 4;
		
	}
	
}


