#include "DGPIO.h"
#include "HLED_config.h"
#include "HLED.h"


const HLED_map LED[LED_NUM] = {
	
	{LED_1,PORT_C,PIN_13,RESET},
	{LED_2,PORT_A,PIN_1,RESET}
};


const GPIO_t LED_Init[LED_NUM] = {

		{PIN_13,MODE_PIN13_OP_PP,SPEED_PIN13_10MHZ,PORT_C},
		{PIN_1,MODE_PIN1_OP_PP,SPEED_PIN1_10MHZ,PORT_A}
};
