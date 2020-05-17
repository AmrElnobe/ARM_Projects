#include "FLITF.h"




static uint_16t x;


void main (void)
{
	static const uint_16t * Address = 0x08002000;



	Flash_Unlock();
	//Flash_Lock();
	//FLASH_ErasePage();
	Flash_ErasePage(0x08002000);
	Flash_HalfWord(Address,x);


	//Flash_MassErase();

	while(1);


}




