#ifndef _DF_PLAYER_H
#define _DF_PLAYER_H
#define start_Byte 0x7E
#define DF_Player_Version 0xFF
#define command_Length 0x06
#define no_Feedback 0x00
#define yes_Feedback 0x01
#define play_Specific_Track 0x0F
#define end_Byte 0xEF
#include "system.h" 
void DF_Player_Init(void);
void play_One_Track(u8 number_Of_Folder, u8 number_Of_Track);
void adjust_Volume(void);
void trackPlayer(char user_Dialed);
#endif 
