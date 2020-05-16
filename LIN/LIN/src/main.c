#include "DRCC.h"
#include "DGPIO.h"
#include "DNVIC.h"
#include "HUART.h"
#include "DSYSTICK.h"
#include "HLED.h"
#include "Sched.h"
#include "HLCD.h"
#include "LIN.h"
void t2 (void);
void t_func (void);
	uint_8t arr1[3]={'a','b','c'};
	uint_8t arr2[3];
void main(void)
{

	RCC_SetPriephralStatus (GPIO_A_ENABLE,ON);
	RCC_SetPriephralStatus (GPIO_C_ENABLE,ON);
	RCC_SetPriephralStatus (USART_1_ENABLE,ON);
	RCC_SetPriephralStatus(DMA_1_ENABLE,ON);
	HLED_Init();
	HLED_SetLedState(LED_1,CLEAR);
	LIN_Init();
	LIN_SendData(1,arr1);
	LIN_ReceiveData(1,arr2);
	LIN_SetRxCbf(t_func);
	LIN_SetTxCbf(t2);
	Sched_Init();
	Sched_Start();
}

void t_func (void)
{
	if (arr2[0]=='a' && arr2[1]=='b' &&arr2[2]=='c' )
	{
		HLED_SetLedState(LED_1,SET);
	}

}
void t2 (void)
{
	asm("NOP");
}






