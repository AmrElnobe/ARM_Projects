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
typedef struct
{
	uint_8t Data[8];
	uint_8t checkSum;
}response_t;

static TxCbf_t TxNotify;
static RxCbf_t RxNotify;

uint_8t LIN_Init(void)
{
	uint_8t Local_Error=OK;
	Local_Error=HUART_Init();

	return Local_Error;
}

static void LIN_Task(void)
{

}

uint_8t LIN_SetTxCbf(TxCbf_t TxCbf);

uint_8t LIN_SetRxCbf(RxCbf_t RxCbf);

