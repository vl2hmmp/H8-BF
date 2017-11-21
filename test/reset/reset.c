// ****************************************
// *** リセットスイッチ動作確認         ***
// ***    produced by Y. Mori           ***
// ***       special thanks to A. Ruike ***
// ****************************************
#include <mes2.h>
#include "r3069.h"

static int  End_flag;


// ************************
// *** 割り込み処理関数 ***
// *** リセット用       ***
// ************************
#pragma interrupt
void 
prg_end(void)
{
	load_segment( 7 );
	End_flag= 1;
}


/* ************ */
/* *** main *** */
/* ************ */
int 
main(void)
{
	End_flag = 0;

	// NMI 割り込みの設定（リセット用）
	SYSCR.BIT.NMIEG = 0;
	set_handler( 7, prg_end );

	while(1) {
		printf(" Now program is running ... \r");
		if( End_flag == 1){
			exit(0);
		}
	}
}
