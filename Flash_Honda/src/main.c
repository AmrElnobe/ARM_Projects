#include "FLITF.h"






void main (void)
{
	static const uint_16t * Address = 0x08002000;
	uint_8t arr[1024] ={0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9} ;

	arr[1023]=0x55;
	Flash_Unlock();
	Flash_MassErase();
	//Flash_ProgramWrite(Address,arr,1024);
	Flash_ErasePage(0x08002008);
	asm ("NOP");




	while(1);


}




