// ****************************************
// *** ＡＤポートの動作確認             ***
// ***    produced by Y. Mori           ***
// ***       special thanks to A. Ruike ***
// ****************************************
// ● H8ボードの JP2, JP3を配線すること

#include "r3069.h"
#include <mes2.h>

/* ************ */
/* *** main *** */
/* ************ */
int 
main(void)
{
	int  ad_data, ad_volt;

	// ADポートの初期化
	AD.ADCSR.BYTE = 0x08;  
	// 00001000
	// ||||||||_AN0
	// |||||_	変換時間 高速モード
	// ||||_	単一モード
	// |||_		A/D変換停止
	// ||_		A/D変換割り込み禁止
	// |_		A/D変換終了フラグ
	
	while (1){
		AD.ADCSR.BIT.ADST = 1;			// AD変換スタート
		while (AD.ADCSR.BIT.ADF == 0);	// 変換終了まで待つ
		ad_data = AD.ADDRA >> 6;		// ビットシフト
		AD.ADCSR.BIT.ADF = 0;

		ad_volt = (int)((5.13f/1024.0f)*1000.0f*ad_data + 0.5f);  //5.13[V]は入力電圧（実測)

		printf(" %4d   %4d [mV]\r", ad_data, ad_volt);
	}
}
