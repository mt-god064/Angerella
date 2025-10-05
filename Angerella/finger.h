#ifndef _FINGER_H
#define _FINGER_H

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


#include <stdio.h>


#define  Finger_RST_Pin     9

#define TRUE  1
#define FALSE 0

// Packet Identify code
#define Command 										0xAA55		//Command packet    				
#define Response										0x55AA		//Response packet					
#define Command_Data 								0xA55A		//Command Data Packet	
#define Response_Data				 				0x5AA5		//Response Data Packet	


// Soruce Device ID
#define Command_SID	 								0x00			//Command package source identifier
#define Response_SID	 							0x01			//Source identifier of the response packet

// Destination Device ID 
#define Command_DID	 								0x00			//Command package target id
#define Response_DID	 							0x00			//The target identifier of the response packet


// Command Code and Response Code
#define CMD_TEST_CONNECTION					0x01			
#define CMD_SET_PARAM 							0x02			
#define CMD_GET_PARAM 							0x03			
#define CMD_DEVICE_INFO 						0x04			
#define CMD_ENTER_IAP_MODE 					0x05			
#define CMD_GET_IMAGE 							0x20			
#define CMD_FINGER_DETECT 					0x21			
#define CMD_UP_IMAGE_CODE 					0x22			 									
#define CMD_DOWN_IMAGE 							0x23			
#define CMD_STORE_CHAR 							0x40			
#define CMD_LOAD_CHAR 							0x41			
#define CMD_UP_CHAR 								0x42			
#define CMD_DOWN_CHAR 							0x43			
#define CMD_DEL_CHAR 								0x44			
#define CMD_GET_EMPTY_ID 						0x45			
#define CMD_GET_STATUS 							0x46			
#define CMD_GET_BROKEN_ID 					0x47			
#define CMD_GET_ENROLL_COUNT 				0x48			
#define CMD_GENERATE 								0x60			
#define CMD_MERGE 									0x61			
#define CMD_MATCH 									0x62			
#define CMD_SEARCH 									0x63			
#define CMD_VERIFY 									0x64			
#define CMD_SET_MODULE_SN 					0x08			 				
#define CMD_GET_MODULE_SN 					0x09			
#define CMD_GET_ENROLLED_ID_LIST 		0x49			
#define CMD_ENTER_STANDBY_STATE 		0x0C			
#define CMD_ADJUST_SENSOR 					0x25			


// Length of DATA
#define DATA_0											0x0000		
#define DATA_1											0x0001		
#define DATA_2											0x0002		
#define DATA_3											0x0003		
#define DATA_4											0x0004		
#define DATA_5											0x0005		
#define DATA_6											0x0006		


// Result Code  		
#define ERR_SUCCESS									0x00			
#define ERR_FAIL										0x01			
#define ERR_VERIFY									0x10			
#define ERR_IDENTIFY								0x11			
#define ERR_TMPL_EMPTY							0x12			
#define ERR_TMPL_NOT_EMPTY					0x13			
#define ERR_ALL_TMPL_EMPTY					0x14			
#define ERR_EMPTY_ID_NOEXIST				0x15			
#define ERR_BROKEN_ID_NOEXIST				0x16			
#define ERR_INVALID_TMPL_DATA				0x17			
#define ERR_DUPLICATION_ID					0x18			
#define ERR_BAD_QUALITY							0x19			
#define ERR_MERGE_FAIL							0x1A			
#define ERR_NOT_AUTHORIZED					0x1B			
#define ERR_MEMORY									0x1C			
#define ERR_INVALID_TMPL_NO					0x1D			
#define ERR_INVALID_PARAM						0x22			
#define ERR_GEN_COUNT								0x25			
#define ERR_TIME_OUT								0x23			
#define ERR_INVALID_BUFFER_ID				0x26			
#define ERR_FP_NOT_DETECTED					0x28			
#define ERR_FP_CANCEL								0x41			


// Command structure
#define CMD_Len  16
#define RPS_Len  14


typedef struct  
{  
	uint16_t		PREFIX ; 
	uint8_t			SID ;
	uint8_t			DID ;
	uint8_t			CMD ;
	uint16_t		LEN ;
	uint8_t			DATA[CMD_Len] ;
	uint16_t 		CKS ;
}Cmd_Packet;  


typedef struct  
{  
	uint16_t		PREFIX ; 
	uint8_t			SID ;
	uint8_t			DID ;
	uint8_t			CMD ;
	uint16_t		LEN ;
	uint8_t			RET ;
	uint8_t			DATA[RPS_Len] ;
	uint16_t 		CKS ;
}Rps_Packet;  

extern Cmd_Packet CMD; 
extern Rps_Packet RPS;


void Command_Packet_Init(void);
void Tx_cmd(void);
uint8_t Tx_Data_Process(void);
uint8_t CMDTESTCONNECTION( uint8_t back );
uint8_t CMDFINGERDETECT( uint8_t back );
uint8_t CMDGETIMAGE( uint8_t back );
uint8_t CMDGENERATE( uint8_t k , uint8_t back );
uint8_t CMDMERGE( uint8_t k , uint8_t n , uint8_t back );
uint8_t CMDSTORECHAR( uint16_t k , uint8_t n , uint8_t back );
uint8_t AddUser(void);
uint8_t ClearUser( uint8_t back );
uint8_t ScopeVerifyUser(void);
uint8_t CMDGETEMPTYID(uint8_t back);
uint8_t GetUserCount(uint8_t back);
uint32_t TX_DATA(void);

uint8_t Rx_cmd( uint8_t back );
void Rx_CMD_Process(void);
uint8_t Rx_Data_Process( uint8_t back );
uint8_t RPSTESTCONNECTION( uint8_t back );
uint8_t RPSFINGERDETECT( uint8_t back );
uint8_t RPSGETIMAGE( uint8_t back );
uint8_t RPSGENERATE( uint8_t back );
uint8_t RPSMERGE( uint8_t back );
uint8_t RPSSTORECHAR( uint8_t back );
uint8_t RPSDELCHAR(uint8_t back);
uint8_t RPSSEARCH(uint8_t back);
uint8_t RPSGETEMPTYID(uint8_t back);
uint8_t RPSGETENROLLCOUNT(uint8_t back);


void Handshake_Signal(void);
void CMD_Init(void);




#endif


