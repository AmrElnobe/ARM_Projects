/*
 * LIN.c
 *
 *  Created on: May 8, 2020
 *      Author: DELL
 */

#include "HUART.h"

#include "LIN.h"
#include "LIN_cfg.h"

#define SYNC_BYTE 0x55

#define ID0 0x01
#define ID1 0x02
#define ID2 0x04
#define ID3 0x08
#define ID4 0x10
#define ID5 0x20
#define P0	0x40
#define P1	0x80

#define CLEAR_PARITY_BITS 0x3F

static void LIN_Runnable(void);
/*header*/
static void RxHeaderDone(void);
static void startRxHeader(void);
typedef uint_8t PID_t;

typedef struct{
	uint_8t syncByte;
	PID_t PID;
}header_t;

/*response*/
typedef struct {
	uint_8t * Data;
	uint_8t checkSum;
} response_t;

task_t LIN_Task ={.Runnable = LIN_Runnable , .periodicity = 5};

static header_t receivedHeader;

static TxCbf_t TxNotify;
static RxCbf_t RxNotify;

static uint_8t headerReceivedFlag;

extern LIN_Slavecfg_t slave_Msgs[NUMBER_OF_MSGS];
extern LIN_Mastercfg_t master_LDF[MAX_MSGS_NUM];

uint_8t LIN_Init(void)
{
	uint_8t Local_Error = OK;
	Local_Error = HUART_Init();
	HUART_SetLBDCbf(startRxHeader);
	return Local_Error;
}

static void RxHeaderDone(void)
{
	headerReceivedFlag=1;
}

static void startRxHeader(void){
	HUART_Receive((uint_8t *) &receivedHeader, 2);
	HUART_SetRxCbf(RxHeaderDone);
}

static void LIN_Runnable(void)
{
	uint_8t msgIterator;
	/*Master Task*/
#if NODE_STATE == MASTER_NODE
	static uint_8t msgCounter;
	static uint_8t delay;
	header_t currentHeader;

	if(!delay)
	{
		HUART_SendBreak();
		currentHeader.syncByte=SYNC_BYTE;
		currentHeader.PID=0;

		currentHeader.PID = (((((master_LDF[msgCounter].ID & ID0) >> 0) ^ ((master_LDF[msgCounter].ID & ID1) >> 1)) \
				^ (((master_LDF[msgCounter].ID & ID2) >> 2) ^ ((master_LDF[msgCounter].ID & ID4) >> 4)) ) << 6 ) | \
						(((((master_LDF[msgCounter].ID & ID1) >> 1) ^ ((master_LDF[msgCounter].ID & ID3) >> 3)) \
						^ (((master_LDF[msgCounter].ID & ID4) >> 4) ^ ((master_LDF[msgCounter].ID & ID5) >> 5)) ) << 7 ) \
						| master_LDF[msgCounter].ID;

		HUART_Send((uint_8t *) &currentHeader, 2);
		delay=master_LDF[msgCounter].ExecTime;
		if((master_LDF[msgCounter].ExecTime%LIN_Task.periodicity))
			delay+=((LIN_Task.periodicity) - (master_LDF[msgCounter].ExecTime%LIN_Task.periodicity));
		msgCounter++;
		delay-=LIN_Task.periodicity;
		if(msgCounter == MAX_MSGS_NUM)
			msgCounter=0;
	}
	else
	{
		delay-=LIN_Task.periodicity;
	}

#endif

	/*delay between Header and response*/

	/*Slave Task*/
	if(headerReceivedFlag)
	{
		if (SYNC_BYTE == receivedHeader.syncByte)
		{
			if(((((((receivedHeader.PID & ID0) >> 0) ^ ((receivedHeader.PID & ID1) >> 1)) \
					^ (((receivedHeader.PID & ID2) >> 2) ^ ((receivedHeader.PID & ID4) >> 4))) << 6 ) == (receivedHeader.PID & P0)) && \
					((((((receivedHeader.PID & ID1) >> 1) ^ ((receivedHeader.PID & ID3) >> 3)) \
					^ (((receivedHeader.PID & ID4) >> 4) ^ ((receivedHeader.PID & ID5) >> 5)) ) << 7 ) == (receivedHeader.PID & P1)))
			{
				receivedHeader.PID &= CLEAR_PARITY_BITS;

				for(msgIterator=0; msgIterator<NUMBER_OF_MSGS; msgIterator++)
				{
					if (receivedHeader.PID == slave_Msgs[msgIterator].ID)
					{
						if (slave_Msgs[msgIterator].msgState == LISTENER)
						{
							HUART_Receive(slave_Msgs[msgIterator].data, slave_Msgs[msgIterator].dataSize);
						}
						else if (slave_Msgs[msgIterator].msgState == OWNER)
						{
							HUART_Send(slave_Msgs[msgIterator].data, slave_Msgs[msgIterator].dataSize);
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

//uint_8t LIN_SetTxCbf(TxCbf_t TxCbf);
//
//uint_8t LIN_SetRxCbf(RxCbf_t RxCbf);

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
