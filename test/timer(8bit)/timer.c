// *********************************************
// *** タイマ割り込み の動作テストプログラム ***
// ***   8bit バージョン　　　　　　　　　　 ***
// ***                   produced by Y. Mori ***
// ***   special thanks to A. Ruike, S. Kido ***
// *********************************************
// tftp timer.elf 192.168.1.1
// timer.elf

#include "r3069.h"
#include <mes2.h>

static int  Count_feed;  // 割り込み回数，外部変数とする
static int  Count_sec;   // 1[s]間で feed()に入る回数

static void  init_settings( void );


// ***************
// *** feed () ***
// *************** 
#pragma interrupt  
void 
feed(void )
{
	load_segment(42);  // 24, ベクタ番号（大域変数を使うときに必要），pp.143

	Count_feed ++;

	// TSCR:タイマコントロールステータスレジスタ pp.464
	// CMFA:TCORA のコンペアマッチの発生を示すステータスフラグ pp.465
	OCT_ITU3.TCSR.BIT.CMFA = 0;
}


// ***************
// *** main () ***
// *************** 
int 
main(void)
{
    int i;

	init_settings();

    while (1){ 
		sleep(10);  // 複数回表示されるのを防ぐため

	    if( Count_feed % Count_sec == 0 ){
			printf(" %d [s]\r", (int)(Count_feed/Count_sec) );
		}
	}	
}

// ------------------------
// --- H8の初期設定関数 ---
// ------------------------
static void
init_settings( void )
{
	OCT_ITU3.TCNT  = 0;   // カウンタ, TCORAと8TCNTの値は常に比較されている．pp.460
	OCT_ITU3.TCORA = 49;  // タイムコンスタントレジスタA
	OCT_ITU3.TCSR.BYTE = 0x00;
	OCT_ITU3.TCR.BIT.CCLR  = 1;  // カウンタクリア 1:コンペアマッチAによりクリア pp.462
	OCT_ITU3.TCR.BIT.CMIEA = 1;
	OCT_ITU3.TCR.BIT.CKS   = 3;  // タイマー開始 011: CK2=0, CK1=1, CK0=1 -> φ/8192, pp.463
	                             // タイマー停止 000: CK2=0, CK1=0, CK0=0 -> pp.463
	set_handler( 42, feed );

	Count_feed= 0;
	Count_sec= 20000000/8192/OCT_ITU3.TCORA;  // 20MHz/8192/TCORA
}
