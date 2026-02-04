#ifndef _HP9170_H
#define _HP9170_H
#define  read_DV_Signal 		PBin(11)
#define  read_HT9170_D0     PBin(12)
#define  read_HT9170_D1     PBin(13)
#define  read_HT9170_D2     PBin(14)
#define  read_HT9170_D3     PBin(15)
#define STATE_000 0x00
#define STATE_001 0x01
#define STATE_010 0x02
#define STATE_011 0x03
#define STATE_100 0x04
#include "system.h" 
#include "stdio.h"
extern u8 timeout_Flag;
extern char user_InputArray[]; // the array used to store user's password input 
extern u8 user_InputArray_Length; 

void HP9170_Init(void); // Pin initialization for HP9170
char decoding_One_Tone(void);
char decoding_One_Tone_with_DVcontrol(void);// decoding the DTMF signal 
void two_User_Input_Sequence(void);
void two_User_Input_Sequence_V2(void);// Checking the combination of DTMF to control relays
extern char sequence_Buffer[2];
void  trackPlayer_V2(void );// Playing the audio of user's relay control action 
void user_Typein_Password(void); // The user's inputing password sequence (wait for enter and stuff)
u8 password_Matches_orNot(char password_24C02[], u8 length );
u8 password_Sequence ( char password_24C02[], u8 length);// The user's sequence that defines user has two tries for password inputing each call 

#endif 
