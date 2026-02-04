#ifndef _PIN_CONFIG_H
#define _PIN_CONFIG_H
#include "system.h" 
#include "stdbool.h"
#include "stdio.h"

typedef enum 
{
	line_Busy=0,
	no_Reply=1,
	pickedUp=2,
}callout_User_Status ;

typedef enum 
{
	status_Overheating=0,
	status_Power_Outage=0,
	status_Previous_Task_Remaining= 0,
	status_calls_reseted= 0,
	status_Callin_hanging_Up= 0,
	status_Callin_Dectected = 1,
}rpc_Status;


void pin_Setup(void);

extern  u8 power_Outage_Event;
extern  u8 powerOutage_Task_Waiting;
u8 user_Calling_Dectection_V3_firstEdition(u8 rings);
u8 voltage_Detection_v3(u8 rings);
rpc_Status voltage_Detection_v5(u8 rings);
u8 callout_Detection(u8 rings);
u8 DV_Checking (void );
callout_User_Status callout_Detection_mod(u8 rings);
u8  DV_Checking_Simplified (void );
void power_Outage_Detection(void);


#endif 
