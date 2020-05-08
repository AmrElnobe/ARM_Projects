#ifndef LIN_H_
#define LIN_H_

typedef unsigned char uint_8t;
typedef unsigned short int uint_16t;
typedef unsigned long int uint_32t;
typedef unsigned long long uint_64t;
typedef signed char int_8t;
typedef signed short int int_16t;
typedef signed short int int_32t;

#define OK       0
#define NOT_OK   1

typedef void(*TxCbf_t)(void);
typedef void(*RxCbf_t)(void);

uint_8t LIN_Init(void);
uint_8t LIN_SendResponse();
uint_8t LIN_ReceiveResponse();

uint_8t LIN_SetTxCbf(TxCbf_t TxCbf);
uint_8t LIN_SetRxCbf(RxCbf_t RxCbf);

#endif
