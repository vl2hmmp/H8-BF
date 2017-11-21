// *******************************************
// *** Port 読み込みテストプログラム       ***
// ***                 produced by Y. Mori ***
// *** special thanks to A. Ruike, S. Kido ***
// *******************************************
#include "r3069.h"
#include <mes2.h>

// ***************
// *** main () ***
// ***************
int 
main(void){

	P5.DDR=0x00;

	while(1){
		printf(" P5: 0=%d  1=%d  2=%d  3=%d \r",
			P5.DR.BIT.B0, P5.DR.BIT.B1, P5.DR.BIT.B2, P5.DR.BIT.B3 );
	}
}

