/*
 * LIN.c
 *
 *  Created on: May 8, 2020
 *      Author: DELL
 */

#include "HUART.h"
#include "LIN.h"
#include "LIN_cfg.h"
#include "Sched.h"


typedef uint_8t PID_t;
typedef struct{

	uint_8t reserved;   /*To receive the extra Byte ely htl3 3enak :D*/
	uint_8t syncByte;	/*0x55*/
	PID_t PID;			/*MSG_ID + Parity bits*/
}header_t;

/*Synch. Byte value*/
#define SYNC_BYTE 0x55
/*ID bits masks*/
#define ID0 0x01
#define ID1 0x02
#define ID2 0x04
#define ID3 0x08
#define ID4 0x10
#define ID5 0x20
/*Parity bits masks*/
#define P0	0x40
#define P1	0x80

#define CLEAR_PARITY_BITS 0x3F

static void LIN_Runnable(void);       /*Main Task (Runnable)for the LIN. Contains master and slave tasks*/
static void startRxHeader(void);      /*Indicate that the Break is detected and start receiving the Synch and ID bytes*/
static void RxHeaderDone(void);       /*Indicate that the header is received completely for the slave task to start its process*/
static void ReceiveCheckSum(void);    /*CBF for receinving data,
										After receiving the Data bytes, we need to receive extra checksum byte*/
static void ReceivedDone(void);       /*Final Receive CBF after receiving the check sum*/
static void TransmitCheckSum(void);   /**/

task_t LIN_Task ={.Runnable = LIN_Runnable , .periodicity = 5}; /*LIN Task Config*/
static header_t receivedHeader;		   /*Struct which contains the received header*/


static uint_8t headerReceivedFlag;  /*Flag to indicate that a new header was received*/

/*Messages configurations for slave task*/
extern LIN_Slavecfg_t slave_Msgs[NUMBER_OF_MSGS]; /*Slave-task messages*/

#if  NODE_STATE == MASTER_NODE
extern LIN_Mastercfg_t master_LDF[MAX_MSGS_NUM];
#endif

static uint_8t CheckSum ;		   /*Transmitted/Received checksum*/
static uint_8t CurrentMsgID ;	   /*Contain index of the msg in slave messages array For checking the checksum*/

uint_8t LIN_Init(void)
{
	uint_8t Local_Error = OK;
	Local_Error = HUART_Init();
	HUART_SetLBDCbf(startRxHeader);
	return Local_Error;
}
uint_8t LIN_SendData(uint_8t msgID, uint_8t * data) {
	uint_8t LocalError = NOT_OK;
	uint_8t nodeMsgNum;
	if (data) {
		for (nodeMsgNum = 0; nodeMsgNum < NUMBER_OF_MSGS; nodeMsgNum++)
		{
			if ((slave_Msgs[nodeMsgNum].ID == msgID)&& (slave_Msgs[nodeMsgNum].msgState == OWNER))
			{
				slave_Msgs[nodeMsgNum].data = data;
				LocalError = OK;
			}
			else
			{
				/*MISRA*/
			}
		}
	}
	else
	{
		/*MISRA*/
	}
	return LocalError;
}
uint_8t LIN_ReceiveData(uint_8t msgID, uint_8t * data) {
	uint_8t LocalError = NOT_OK;
	uint_8t nodeMsgNum;
	if (data) {
		for (nodeMsgNum = 0; nodeMsgNum < NUMBER_OF_MSGS; nodeMsgNum++)
		{
			if ((slave_Msgs[nodeMsgNum].ID == msgID) && (slave_Msgs[nodeMsgNum].msgState == LISTENER))
			{
				slave_Msgs[nodeMsgNum].data = data;
				LocalError = OK;
			}
			else
			{
				/*MISRA*/
			}
		}
	}
	else
	{
		/*MISRA*/
	}
	return LocalError;
}
uint_8t LIN_SetNotifyCbf(uint_8t msgID,NotifyCbf_t NotCbf)
{
	uint_8t Local_Error=OK;
	uint_8t MsgIterator;
	if (NotCbf)
	{
		for (MsgIterator =0 ; MsgIterator < NUMBER_OF_MSGS;MsgIterator ++)
		{
			if (msgID == slave_Msgs [MsgIterator].ID)
			{
				slave_Msgs [MsgIterator].MsgCbf =NotCbf;
			}
			else
			{
				/*Do Nothing*/
			}
		}
	}
	else
	{
		Local_Error=NOT_OK;
	}
	return Local_Error;
}

static void LIN_Runnable(void)
{
	uint_8t msgIterator;
	/*Master Task*/
#if NODE_STATE == MASTER_NODE		/*Master Task
 	 Header--> Break - Delim - Synch byte - ID&parity */

	static header_t currentHeader;  /*Current header which is going to be transmitted*/
	static uint_8t msgCounter;		/*Index for the current header*/
	static uint_8t delay; /*Delay between Msgs */

	if(!delay)
	{
		HUART_SendBreak(); /*#1 & #2*/
		currentHeader.syncByte=SYNC_BYTE;
		currentHeader.PID=0;

		currentHeader.PID = (((((master_LDF[msgCounter].ID & ID0) >> 0) ^ ((master_LDF[msgCounter].ID & ID1) >> 1)) \
				^ (((master_LDF[msgCounter].ID & ID2) >> 2) ^ ((master_LDF[msgCounter].ID & ID4) >> 4)) ) << 6 ) | \
						(((((master_LDF[msgCounter].ID & ID1) >> 1) ^ ((master_LDF[msgCounter].ID & ID3) >> 3)) \
								^ (((master_LDF[msgCounter].ID & ID4) >> 4) ^ ((master_LDF[msgCounter].ID & ID5) >> 5)) ) << 7 ) \
								| master_LDF[msgCounter].ID; /*Refer to LIN documentation for the calculation of the parity bits*/
		/*Now the current header object contains all the needed info*/
		HUART_Send((uint_8t *) (&currentHeader)+1, 2); /*#3 & #4*/
		delay=master_LDF[msgCounter].ExecTime;
		if((master_LDF[msgCounter].ExecTime%LIN_Task.periodicity))/*To ensure delay is a multiple times of the task period*/
			delay+=((LIN_Task.periodicity) - (master_LDF[msgCounter].ExecTime%LIN_Task.periodicity));
		msgCounter++;
		delay-=LIN_Task.periodicity;/*Subtract Extra period*/
		if(msgCounter == MAX_MSGS_NUM) /*After reaching the final message, the master task will repeat again the operation*/
			msgCounter=0;
	}
	else
	{
		delay-=LIN_Task.periodicity; /*Each time the tasks execute it will subtract a period from the delay*/
	}

#endif


	/*Slave Task*/
	/*Response: Data Bytes - Checksum*/
	if(headerReceivedFlag)
	{
		headerReceivedFlag=0;
		if (SYNC_BYTE == receivedHeader.syncByte) /*Check on the synch byte*/
		{
			/*Check on parity bits for the received ID*/
			if(((((((receivedHeader.PID & ID0) >> 0) ^ ((receivedHeader.PID & ID1) >> 1)) \
					^ (((receivedHeader.PID & ID2) >> 2) ^ ((receivedHeader.PID & ID4) >> 4))) << 6 ) == (receivedHeader.PID & P0)) && \
					((((((receivedHeader.PID & ID1) >> 1) ^ ((receivedHeader.PID & ID3) >> 3)) \
							^ (((receivedHeader.PID & ID4) >> 4) ^ ((receivedHeader.PID & ID5) >> 5)) ) << 7 ) == (receivedHeader.PID & P1)))
			{
				receivedHeader.PID &= CLEAR_PARITY_BITS;/*Now the PID will contains the ID only without the parity*/
				/*Search for the received ID whether you are interested in it or not*/
				for(msgIterator=0; msgIterator<NUMBER_OF_MSGS; msgIterator++)
				{
					if (receivedHeader.PID == slave_Msgs[msgIterator].ID)
					{
						if (slave_Msgs[msgIterator].msgState == LISTENER) /*Listener : Receive the response*/
						{
							HUART_Receive(slave_Msgs[msgIterator].data, slave_Msgs[msgIterator].dataSize);
							CurrentMsgID = msgIterator ; /*Store the index of the message for checksum calculation*/
							HUART_SetRxCbf(ReceiveCheckSum);
						}
						else if (slave_Msgs[msgIterator].msgState == OWNER) /*Owner: Send the response*/
						{
							HUART_Send(slave_Msgs[msgIterator].data, slave_Msgs[msgIterator].dataSize);
							CurrentMsgID = msgIterator ;/*Store the index of the message for checksum calculation*/
							HUART_SetTxCbf(TransmitCheckSum);
						}
						else
						{
							/*MISRA*/
						}
					}
					else
					{
						/*MISRA*/
					}

				}
			}
			else
			{

			}
		}
		else
		{
			/*MISRA*/
		}
	}
	else
	{
		/*MISRA*/
	}
}

static void startRxHeader(void){
	HUART_Receive((uint_8t *) &receivedHeader, 3);
	HUART_SetRxCbf(RxHeaderDone);
}
static void RxHeaderDone(void)
{
	headerReceivedFlag=1;
}


static void TransmitCheckSum(void)
{
	uint_8t CalculatedCheckSum=0 , iterator ;
	for (iterator = 0 ; iterator < slave_Msgs[CurrentMsgID].dataSize ; iterator ++ )
{
		CalculatedCheckSum += slave_Msgs[CurrentMsgID].data[iterator] ;
	}
#if LIN_VERSION == ENHANCED_LIN
	CalculatedCheckSum += slave_Msgs[CurrentMsgID].ID ;
#endif
	CalculatedCheckSum = ~(CalculatedCheckSum) ;

	CheckSum = CalculatedCheckSum ;
	HUART_Send(&CheckSum,1);
	if (slave_Msgs[CurrentMsgID].MsgCbf)
	{
	HUART_SetTxCbf(slave_Msgs[CurrentMsgID].MsgCbf);
	}
	else
	{
		/*Do Nothing*/
	}
}
static void ReceiveCheckSum(void)
{
	HUART_Receive(&CheckSum,1);
	HUART_SetRxCbf(ReceivedDone);

}
static void ReceivedDone(void)
{
	uint_8t CalculatedCheckSum=0 , iterator ;
#if LIN_VERSION == CLASSIC_LIN
	for (iterator = 0 ; iterator < slave_Msgs[CurrentMsgID].dataSize ; iterator ++ )
	{
		CalculatedCheckSum += slave_Msgs[CurrentMsgID].data[iterator] ;
	}
#elif LIN_VERSION == ENHANCED_LIN
	CalculatedCheckSum += slave_Msgs[CurrentMsgID].ID ;
#endif
	CalculatedCheckSum = ~(CalculatedCheckSum) ;
	if(CalculatedCheckSum == CheckSum)
	{
		/* Received correctly */
		if ( slave_Msgs[CurrentMsgID].MsgCbf)
		{
			slave_Msgs[CurrentMsgID].MsgCbf();
		}
	}
	else
	{
		/* Received incorrect */
	}
}
