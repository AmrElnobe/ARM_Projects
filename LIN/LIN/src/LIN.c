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

static void LIN_Runnable(void);
/*header*/
typedef uint_8t PID_t;

typedef struct{
uint_8t syncByte;
PID_t PID;
}header_t;

task_t LIN_Task ={.Runnable = LIN_Runnable , .periodicity = 5};

/*response*/
typedef struct {
	uint_8t * Data;
	uint_8t checkSum;
} response_t;

static TxCbf_t TxNotify;
static RxCbf_t RxNotify;

extern LIN_Slavecfg_t slave_Msgs[NUMBER_OF_MSGS];
extern LIN_Mastercfg_t master_LDF[MAX_MSGS_NUM];

uint_8t LIN_Init(void)
{
	uint_8t Local_Error = OK;
	Local_Error = HUART_Init();

	return Local_Error;
}


static void LIN_Runnable(void)
{
	/*Master Task*/
#if NODE_STATE == MASTER_NODE
	static uint_8t msgCounter;
	static uint_8t delay;
	header_t currentHeader;

	if(!delay)
	{
		HUART_SendBreak();
		currentHeader.syncByte=SYNC_BYTE;
		currentHeader.PID=
		HUART_Send();
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
