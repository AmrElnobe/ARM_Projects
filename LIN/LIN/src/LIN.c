/*
 * LIN.c
 *
 *  Created on: May 8, 2020
 *      Author: DELL
 */

#include "HUART.h"

#include "LIN.h"
#include "LIN_cfg.h"

static void LIN_Task(void);
/*header*/
typedef uint_8t PID_t;

/*response*/
typedef struct {
	uint_8t * Data;
	uint_8t checkSum;
} response_t;

static TxCbf_t TxNotify;
static RxCbf_t RxNotify;
extern LIN_Slavecfg_t slave_Msgs[NUMBER_OF_MSGS];
uint_8t LIN_Init(void) {
	uint_8t Local_Error = OK;
	Local_Error = HUART_Init();

	return Local_Error;
}

static void LIN_Task(void) {

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
