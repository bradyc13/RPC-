#ifndef  _TIMER_H
#define _TIMER_H
#include "system.h" 
#include "HP9170.h"
extern u8 timer3_timeout_Flag;
void timer_Init(u16 per, u16 psc);
void timer_Init_Timer3(u16 per, u16 psc);

#endif 
