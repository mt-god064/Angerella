#include <SoftwareSerial.h>
#include "finger.h"

String comdata = "";

uint8_t cmd[26] = {0x55, 0xAA ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00, 0x00 ,0x01};
uint8_t rps[26] = {0};
uint8_t Tx_flag = 0;

Cmd_Packet CMD;
Rps_Packet RPS;


SoftwareSerial fingerSerial(8, 7); // RX, TX    
/***************************************************************************
* @brief      Initialize the command structure
****************************************************************************/
void Cmd_Packet_Init(void)
{
	CMD.PREFIX = Command;
	CMD.SID = Command_SID;
	CMD.DID = Command_DID;
	CMD.CMD = CMD_TEST_CONNECTION;
	CMD.LEN = DATA_0;
	for(int i = 0 ; i <CMD_Len ; i++)
		CMD.DATA[i] = 0x00;
}

/***************************************************************************
* @brief      Build the command array and send it
****************************************************************************/
void Tx_cmd(void)
{
	uint16_t CKS = 0 ;
	cmd[0] = CMD.PREFIX & 0xff;
	cmd[1] = (CMD.PREFIX & 0xff00) >> 8;
	cmd[2] = CMD.SID;
	cmd[3] = CMD.DID;
	cmd[4] = CMD.CMD ;
	cmd[5] = 0x00 ;
	cmd[6] = CMD.LEN & 0xff;
	cmd[7] = (CMD.LEN & 0xff00) >> 8;
	for(int i = 0 ; i < CMD.LEN ; i++)
		cmd[8+i] = CMD.DATA[i];
	for(int i = 0 ; i < 24 ; i++)
		CKS = CKS + cmd[i];
	cmd[24] = CKS & 0xff;
	cmd[25] = (CKS & 0xff00) >> 8;
	fingerSerial.write(cmd,26); 
    
}

/***************************************************************************
* @brief      Receive user commands and process them
****************************************************************************/
uint8_t Tx_Data_Process(void)
{
	comdata = "";
	while(1)
	{
    while (Serial.available() > 0)  
	  {
	    comdata += char(Serial.read());
	  }
		if( comdata.length() > 3 )        
		{
			if((comdata[0] == 'C') && (comdata[1] == 'M') && (comdata[2] == 'D'))
			{
				Serial.println(comdata);
				switch(comdata[3])
				{
					case '0': CMDTESTCONNECTION( 0 ) ; break;
					case '1': CMDFINGERDETECT( 0 ) ; break;
					case '2': AddUser( ) ; break;
					case '3': ClearUser( 0 ) ; break;
					case '4': ScopeVerifyUser( ) ; break;
					case '5': CMDGETEMPTYID( 0 ) ; break;
					case '6': GetUserCount( 1 ) ; break;
				}
        comdata = "";
				break;
			}
			Serial.println("Please enter the correct command.");
      comdata = "";
		}
		// else
		// {
		// 	Serial.println("Please enter the correct command2.");
		// 	comdata = "";
		// }	
	}
	return 0;
}



/***************************************************************************
* @brief      Connect the test
* @param      back: 0 print return status   
										1 Do not print return status
* @return     0			Instruction processing succeeded
						rests		Instruction processing failure	
****************************************************************************/
uint8_t CMDTESTCONNECTION( uint8_t back )
{
	CMD.CMD = CMD_TEST_CONNECTION;
	CMD.LEN = DATA_0;
	Tx_cmd();
	return Rx_cmd(back);
}

/***************************************************************************
* @brief      To detect
* @param      back: 0 print return status   
										1 Do not print return status
* @return     0			Instruction processing succeeded
						rests		Instruction processing failure	
****************************************************************************/
uint8_t CMDFINGERDETECT( uint8_t back )
{
	CMD.CMD = CMD_FINGER_DETECT;
	CMD.LEN = DATA_0;
	Tx_cmd();
	return Rx_cmd(back);
}

/***************************************************************************
* @brief      Capture fingerprint image
* @param      back: 0 print return status   
					1 Do not print return status
* @return     0			Instruction processing succeeded
						rests		Instruction processing failure	
****************************************************************************/
uint8_t CMDGETIMAGE( uint8_t back )
{
	CMD.CMD = CMD_GET_IMAGE;
	CMD.LEN = DATA_0;
	Tx_cmd();
	return Rx_cmd(back);
}

/***************************************************************************
* @brief      Generates a template from a fingerprint image that is temporarily stored in the ImageBuffer
* @param      k		Stored in the RamBuffer 0 to 10
			  back	0 print return status   
					1 Do not print return status

* @return     0		Instruction processing succeeded
					rests		Instruction processing failure	
****************************************************************************/
uint8_t CMDGENERATE( uint8_t k , uint8_t back )
{
	CMD.CMD = CMD_GENERATE;
	CMD.LEN = DATA_2;
	CMD.DATA[0] = k ;
	CMD.DATA[1] = 0x00 ;
	Tx_cmd();
	return Rx_cmd(back);
}

/***************************************************************************
* @brief      Generates a template from a fingerprint image that is temporarily stored in the ImageBuffer
* @param      k		The synthesized fingerprint is stored in the RamBuffer 0~10
			  n		Synthesize several RamBuffer templates 2 or 3
			back	0 print return status   
					1 Do not print return status

* @return     0		Instruction processing succeeded
					rests		Instruction processing failure	
****************************************************************************/
uint8_t CMDMERGE( uint8_t k , uint8_t n , uint8_t back )
{
	CMD.CMD = CMD_MERGE;
	CMD.LEN = DATA_3;
	CMD.DATA[0] = k ;
	CMD.DATA[1] = 0x00 ;
	CMD.DATA[2] = n ;
	Tx_cmd();
	return Rx_cmd(back);
}

/***************************************************************************
* @brief      Save the finger print template data to the fingerprint database of the module
* @param      k	Save the fingerprint to storage location K	 1~3000
							n			Save the fingerprint template of the NTH RamBuffer
							back	0 print return status   
									1 Do not print return status

* @return     0			Instruction processing succeeded
						rests		Instruction processing failure	
****************************************************************************/
uint8_t CMDSTORECHAR( uint16_t k , uint8_t n , uint8_t back )
{
	CMD.CMD = CMD_STORE_CHAR;
	CMD.LEN = DATA_4;
	CMD.DATA[0] = k ;
	CMD.DATA[1] = 0x00 ;
	CMD.DATA[2] = n ;
	CMD.DATA[3] = 0x00 ;
	Tx_cmd();
	return Rx_cmd(back);
}


/***************************************************************************
* @brief      Register fingerprint
* @param      back	0 print return status   
					1 Do not print return status

* @return     0			Instruction processing succeeded
						rests		Instruction processing failure	
****************************************************************************/
uint8_t AddUser( void )
{
	uint8_t a = 0;
	uint8_t i = 0;
	uint32_t Data = 0 ;
	uint16_t data = 0 ;
	uint16_t k = 0;
	
	Tx_flag = 1;
	CMD.CMD = CMD_GET_EMPTY_ID;
	CMD.LEN = DATA_4;
	CMD.DATA[0] = 0x01 ;
	CMD.DATA[1] = 0x00;
	CMD.DATA[2] = 0xB8 ;
	CMD.DATA[3] = 0x0B ;
	Tx_cmd();
	Rx_cmd(1);
	data = RPS.DATA[0] + RPS.DATA[1] * 0x0100; 
	
	Serial.print("The recommended registration number is: ");
	Serial.println(data,DEC);
	
	Data = TX_DATA();
	k = (Data & 0xffff0000) >> 16; 
	for( a=0 ; a <3 ; a++ )
	{
		for( i=0 ; i < 3 ; i++ )
		{
			if(!CMDFINGERDETECT(1))
				Serial.println("Please move your finger away");
			while( !CMDFINGERDETECT(1) )
			{
				delay(1);
			}
				Serial.println("Please press your finger");
			while( CMDFINGERDETECT( 1 ) )
			{
				delay(1);
			}
			if ( !CMDFINGERDETECT( 1 ) )
			{
				if( !CMDGETIMAGE( 1 ) )
				{
					if( !CMDGENERATE(a, 1) )
						break;				
				}
			}
		}
		if( i == 3 )
		{
			Serial.println("Fingerprint entry failure");
			return 1;
		}
	}
	
	if( !CMDMERGE(0,3,1) )
		if( !CMDSTORECHAR(k,0,0) )
    	{
			Serial.println("The fingerprint is saved successfully");
    	}
	
	return 0;
}

/***************************************************************************
* @brief      Clear fingerprints
* @param      back	0 print return status   
					1 Do not print return status

* @return     0		Instruction processing succeeded
					rests	Instruction processing failure	
****************************************************************************/
uint8_t  ClearUser( uint8_t back )
{
	uint32_t data = 0;
	uint16_t k = 1;
	uint16_t n = 5;
	
	Tx_flag = 0;

	data = TX_DATA();
	k = (data & 0xffff0000) >> 16; 
	n = data & 0xffff;
	
	CMD.CMD = CMD_DEL_CHAR;
	CMD.LEN = DATA_4;
	CMD.DATA[0] = k & 0xff ;
	CMD.DATA[1] = (k & 0xff00) >> 8;
	CMD.DATA[2] = n & 0xff ;
	CMD.DATA[3] = (n & 0xff00) >> 8 ;
	Tx_cmd();
	return Rx_cmd(back);
}

/***************************************************************************
* @brief      Range fingerprint matching
* @param      back	0 print return status   
					1 Do not print return status

* @return     0		Instruction processing succeeded
			rests	Instruction processing failure	
****************************************************************************/
uint8_t ScopeVerifyUser(void)
{
	uint32_t data = 0;
	uint16_t k = 1;
	uint16_t n = 5;
	uint8_t i =0;

	// タイムアウト設定（15秒）
    const unsigned long authTimeout = 15000;
    unsigned long startTime = millis();
	
	Tx_flag = 0;

	data = TX_DATA();
	k = (data & 0xffff0000) >> 16; 
	n = data & 0xffff;
	
	 
	for( i=0 ; i < 3 ; i++ )
	{
		//タイムアウト処理
		if(millis() - startTime > authTimeout){
			Serial.println("指紋認証タイムアウト");
			return -1;
		}
		if(!CMDFINGERDETECT(1))
			Serial.println("指を離してください");
		while( !CMDFINGERDETECT(1) ){
			delay(1);
			if(millis() - startTime > authTimeout){
				Serial.println("指紋認証タイムアウト");
				return -1;
			}
		}
			Serial.println("指紋センサーに触れてください");
		while( CMDFINGERDETECT( 1 ) ){
			delay(1);
			if(millis() - startTime > authTimeout){
				Serial.println("指紋認証タイムアウト");
				return -1;
			}
		}
		if ( !CMDFINGERDETECT( 1 ) )
		{
			if( !CMDGETIMAGE( 1 ) )
			{
				if( !CMDGENERATE(0, 1) )
					break;				
			}
		}
	}
	if( i == 3 )
	{
		Serial.println("Fingerprint entry failure");
		return -1;
	}
	
	CMD.CMD = CMD_SEARCH;
	CMD.LEN = DATA_6;
	CMD.DATA[0] = 0x00;
	CMD.DATA[1] = 0x00;
	CMD.DATA[2] = k & 0xff ;
	CMD.DATA[3] = (k & 0xff00) >> 8;
	CMD.DATA[4] = n & 0xff ;
	CMD.DATA[5] = (n & 0xff00) >> 8;
	Tx_cmd();

	// Rx_cmdの結果を受け取る
    uint8_t result = Rx_cmd(0);

	// Rx_cmdが失敗（1）を返したら-1を返す
	return (result == 1) ? -1 : result;
}


/***************************************************************************
* @brief      Gets the first number in a range of numbers that can be registered
* @param      back	0 print return status   
					1 Do not print return status

* @return     0			Instruction processing succeeded
						rests		Instruction processing failure	
****************************************************************************/
uint8_t CMDGETEMPTYID(uint8_t back)
{
	uint32_t data = 0;
	uint16_t k = 1;
	uint16_t n = 5;
	
	Tx_flag = 0;
	
	data = TX_DATA();
	k = (data & 0xffff0000) >> 16; 
	n = data & 0xffff;
	
	CMD.CMD = CMD_GET_EMPTY_ID;
	CMD.LEN = DATA_4;
	CMD.DATA[0] = k & 0xff ;
	CMD.DATA[1] = (k & 0xff00) >> 8;
	CMD.DATA[2] = n & 0xff ;
	CMD.DATA[3] = (n & 0xff00) >> 8 ;
	Tx_cmd();
	return Rx_cmd(back);
}

/***************************************************************************
* @brief      Query the number of existing fingerprints
* @param      back	0 print return status   
					1 Do not print return status

* @return     0		Instruction processing succeeded
			rests	Instruction processing failure	
****************************************************************************/
uint8_t GetUserCount(uint8_t back)
{
	uint32_t data = 0;
	uint16_t k = 1;
	uint16_t n = 5;
	
	Tx_flag = 0;
	
	data = TX_DATA();
	k = (data & 0xffff0000) >> 16; 
	n = data & 0xffff;
	
	CMD.CMD = CMD_GET_ENROLL_COUNT;
	CMD.LEN = DATA_4;
	CMD.DATA[0] = k & 0xff ;
	CMD.DATA[1] = (k & 0xff00) >> 8;
	CMD.DATA[2] = n & 0xff ;
	CMD.DATA[3] = (n & 0xff00) >> 8 ;
	Tx_cmd();
	return Rx_cmd(!back);
}


/***************************************************************************
* @brief      Range set receive
* @return     Range
****************************************************************************/
uint32_t TX_DATA(void)
{
	char cc = '\0';
	uint8_t a = 0;
	uint8_t i = 0;
	uint16_t data_start = 0;
	uint16_t data_end = 0;
	uint32_t Data = 0 ;
  comdata = "";
  if(Tx_flag)
		Serial.println("Enter an address from 1 to 3000");
	else
		//Serial.println("Enter an address between 1 and 3000 (Example: 1,100).");
	while(1)
	{
		
	  // while(Serial.available() > 0)  
	  // {
		// 	cc = char(Serial.read());
	  //   comdata += char(cc);
		//   Serial.print(cc);
	  //   a++;
	  //   delay(1);
	  // }

		comdata = "1,1000";
		//Serial.print(comdata);
		a = 6;

		if(comdata.length() > 0)       
		{
			//Serial.println();
			data_start = 0 ;
			data_end = 0 ;
			if(Tx_flag)
			{
				if(a > 4)
				{
					Serial.println("please input again");
					a=0;
					continue;
				}
				else
				{				
					for(i = 0 ; i < a ; i++)
					{
						data_start = data_start * 10 + (comdata[i] - 0x30);
					}
					if( (data_start > 3000) || (data_start < 1) )
					{
						Serial.println("please input again");
						break;
					}
					break;
				}
			}
			else
			{
				if(a > 9)
				{
					Serial.println("please input again");
					a=0;
					continue;
				}
				else
				{
					for(i = 0 ; comdata[i] != ',' ; i++)
					{
						data_start = data_start * 10 + (comdata[i] - 0x30);
						if(i>3)
							break;
					}
					if( (data_start > 3000) || (data_start < 1) || (i>3))
					{
						Serial.println("please input again");
						break;
					}
					for(i = i+1 ; i < a ; i++)
					{
						data_end = data_end * 10 + (comdata[i] - 0x30);
					}
					if( (data_end > 3000) || (data_end < 1) )
					{
						Serial.println("please input again");
						break;
					}
					break;
				}
			}
		    a = 0;
		    comdata = "";
		}
		a = 0;
		comdata = "";
	}
	Data = data_start * 0x10000 + data_end;
	return Data;
}


/***************************************************************************
* @brief      Receive response array
* @param      back	0 print return status   
					1 Do not print return status

* @return     0		Instruction processing succeeded
			rests	Instruction processing failure	
****************************************************************************/
uint8_t Rx_cmd( uint8_t back )
{
	uint8_t a=1;
  	uint8_t i = 0 ;
	uint16_t CKS = 0;
	while(a)
	{
	    i = 0;
	    while (fingerSerial.available() > 0)  
	    {
				delay(1);
	      rps[i++] = fingerSerial.read();
				delay(1);
	    }
	    if (i > 25)
	    {
	  		a = 0;
	  		CKS = 0;
	  		if(rps[4] == 0xff)
	  			return 1; 			
	  		Rx_CMD_Process();	
	  		for(int i=0 ; i<24 ; i++)
	  			CKS = CKS + rps[i];
	  		if(CKS == RPS.CKS)
	  			return Rx_Data_Process(back);
	  		i = 0;
	    }
	}
	return 1;
}

/***************************************************************************
* @brief      Place the response data in the structure
****************************************************************************/
void Rx_CMD_Process(void)
{
	RPS.PREFIX = rps[0] + rps[1] * 0x100;
	RPS.SID = rps[2];
	RPS.DID = rps[3];
	RPS.CMD = rps[4] + rps[5] * 0x100;
	RPS.LEN = rps[6] + rps[7] * 0x100;
	RPS.RET = rps[8] + rps[9] * 0x100;
	for(int i=0 ; i<RPS_Len ; i++)
		RPS.DATA[i] = rps[10 +i];
	RPS.CKS = rps[24] + rps[25] * 0x100;
}

/***************************************************************************
* @brief      Process the response data
* @param      back	0 print return status   
					1 Do not print return status

* @return     0		Instruction processing succeeded
			rests	Instruction processing failure	
****************************************************************************/
uint8_t Rx_Data_Process( uint8_t back )
{
	uint8_t a = 0;
	switch(RPS.CMD)
	{
		case CMD_TEST_CONNECTION: a = RPSTESTCONNECTION(back); break;
		case CMD_FINGER_DETECT: a = RPSFINGERDETECT(back) ; break;
		case CMD_GET_IMAGE: a = RPSGETIMAGE(back); break;
		case CMD_GENERATE: a = RPSGENERATE(back); break;
		case CMD_MERGE: a = RPSMERGE(back); break;
		case CMD_DEL_CHAR : a = RPSDELCHAR(back); break;
		case CMD_STORE_CHAR: a =RPSSTORECHAR(back) ; break;
		case CMD_SEARCH: a = RPSSEARCH(back) ; break;
		case CMD_GET_EMPTY_ID : a = RPSGETEMPTYID(back); break;
		case CMD_GET_ENROLL_COUNT : a = RPSGETENROLLCOUNT(back); break;
	}
	return a;
}

/***************************************************************************
* @brief      Response and error code list
****************************************************************************/
uint8_t RPS_RET(void)
{
	switch(RPS.RET)
	{
		case ERR_FAIL: Serial.println("Instruction processing failure"); break;
		case ERR_INVALID_BUFFER_ID: Serial.println("The RamBuffer number is invalid"); break;
		case ERR_BAD_QUALITY: Serial.println("Poor fingerprint image quality"); break;
		case ERR_GEN_COUNT: Serial.println("Invalid number of combinations"); break;
		case ERR_INVALID_TMPL_NO: Serial.println("The specified Template number is invalid"); break;
		case ERR_DUPLICATION_ID: Serial.print("The fingerprint has been registered, and the id is: " );Serial.println(RPS.DATA[0]+RPS.DATA[1]*0x100,DEC); break;
		case ERR_INVALID_PARAM: Serial.println("Specified range invalid"); break;
		case ERR_TMPL_EMPTY: Serial.println("Template is not registered in the specified range"); break;
		case ERR_IDENTIFY: break;
		//case ERR_IDENTIFY: Serial.println("Fingerprint comparison failed for the specified range"); break;
	}
	return RPS.RET;
}


/***************************************************************************
* @brief      Connection Test
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSTESTCONNECTION( uint8_t back )
{
	if(back)
		return RPS.RET;
	else
		if( RPS.RET )
			return RPS_RET();
		else
		{
			Serial.println("Connection successful");
			return RPS.RET;
		}		
}

/***************************************************************************
* @brief      To detect
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSFINGERDETECT( uint8_t back )
{
	if(back)
	{
		if( !RPS.RET )
			return (!RPS.DATA[0]);
	}
	else
	{
		if( RPS.RET )
			return RPS_RET();
		else
		{
			switch(RPS.DATA[0])
			{
				case 0: Serial.println("No finger detected"); break;
				case 1: Serial.println("We got a finger on it"); break;
			}
			return (!RPS.DATA[0]);
		}
	}
	return 2;
}

/***************************************************************************
* @brief      Capture fingerprint image
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSGETIMAGE( uint8_t back )
{
	if(back)
		return RPS.RET;
	else
		return RPS_RET();
}

/***************************************************************************
* @brief      Generates a template from a fingerprint image that is temporarily stored in the ImageBuffer
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSGENERATE( uint8_t back )
{
	if(back)
		return RPS.RET;
	else
		return RPS_RET();
}

/***************************************************************************
* @brief      Synthetic fingerprint template data is used for storing
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSMERGE( uint8_t back )
{
	if(back)
		return RPS.RET;
	else
		return RPS_RET();
}

/***************************************************************************
* @brief      Save the fingerprint template data to the fingerprint database of the module
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSSTORECHAR( uint8_t back )
{
	if(back)
		return RPS.RET;
	else
		if( RPS.RET )
			return RPS_RET();
		else
			return RPS.RET;
}

/***************************************************************************
* @brief      Erase fingerprints
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSDELCHAR(uint8_t back)
{
	if(back)
		return RPS.RET;
	else
  {
    if( RPS.RET )
      return RPS_RET();
    else
    {
      Serial.println("successfully delete");
      return RPS.RET;
    }
  }
		
}

/***************************************************************************
* @brief      Check the prints in the range
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSSEARCH(uint8_t back)
{
	uint16_t data = 0;
	if(back)
		return RPS.RET;
	else
	{
		if( RPS.RET )
			return RPS_RET();
		else
		{
			data = RPS.DATA[0] + RPS.DATA[1] * 0x0100;
			Serial.print("あなたのID: ");
      Serial.println(data,DEC);
			return RPS.RET;
		}
	}
	return 2;
}     

/***************************************************************************
* @brief      Gets the first number in a range of numbers that can be registered
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSGETEMPTYID(uint8_t back)
{
	uint16_t data = 0;
	if(back)
		return RPS.RET;
	else
	{
		if( RPS.RET )
			return RPS_RET();
		else
		{
			data = RPS.DATA[0] + RPS.DATA[1] * 0x0100;
			Serial.print("The first number you can register for is: ");
      		Serial.println(data,DEC);
			return RPS.RET;
		}	
	}
	return 2;
}

/***************************************************************************
* @brief      Gets the total number of registered fingerprints in the number range
* @param      back: 0 print return status   
					1 Do not print return status
* @return     RPS.RET  Response and error code
****************************************************************************/
uint8_t RPSGETENROLLCOUNT(uint8_t back)
{
	uint16_t data = 0;
	if(back)
		return RPS.RET;
	else
	{
		if( RPS.RET )
			return RPS_RET();
		else
		{
			data = RPS.DATA[0] + RPS.DATA[1] * 0x0100;
			Serial.print("The total number of fingerprint registrations is:  ");
      		Serial.println(data,DEC);
			return RPS.RET;
		}	
	}
	return 2;
}





/***************************************************************************
* @brief     Wait for the response from the fingerprint module
****************************************************************************/
void Handshake_Signal(void)
{
	uint8_t a = 0;
// Select a power-on waiting mode
#define CMD_DELAY 
	#ifdef CMD_DELAY
		delay(500);
	#else
		digitalWrite(Finger_RST_Pin , LOW);
		delay(300); 
	    digitalWrite(Finger_RST_Pin , HIGH);
		while(1)
		{
			while(fingerSerial.available() <= 0) ;
			a = char(fingerSerial.read());
			if(a == 0x55)
				break;
			delay(1);
		}
	#endif
#undef CMD_DELAY 
}

/***************************************************************************
* @brief     Init
****************************************************************************/
void CMD_Init(void)
{
	uint8_t i = 0;
	fingerSerial.begin(115200);
	//fingerSerial.begin(57600);
	Cmd_Packet_Init();
	Handshake_Signal();
	while(1)
	{
		Tx_cmd();
		if( Rx_cmd(0) )
		{
//			Serial.println("Connection closed by server");
			Serial.println("Try to reconnect");
			if(i++ > 3)
			{
				Serial.println("Power on the device again");
				while(1);
			}
		}
		else
			break;
		delay(1000);
	}
}








