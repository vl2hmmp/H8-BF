// *******************************************
// *** encoder 読み込みテストプログラム    ***
// ***                 produced by Y. Mori ***
// *** special thanks to A. Ruike, S. Kido ***
// *******************************************
// tftp enco.elf 192.168.1.1
// enco.elf

#include "r3069.h"
#include <mes2.h>
#include "enc_common.h"

ct_sharedType  Ct;

// --------------
// --- 関数群 ---
// --------------
static void  init_settings();
static void  initialize_para( void );
static int   enco( int no );  // このH8ﾎﾞｰﾄﾞでは，int=longの扱い


// ***************
// *** main () ***
// ***************
int
main( void )
{
	init_settings();    // H8の初期設定
	initialize_para();  // パラメータの設定

	while(1){
		printf(" %d  %d \r", enco(LW), enco(RW) );
	}

	exit(0);
}


// ------------------------
// --- H8の初期設定関数 ---
// ------------------------
static void
init_settings( void )
{
	// *** for LW ***
	// --- ITU2 位相係数モード設定 ---
	HEX_ITU.TMDR.BIT.MDF=  1;  // 位相係数モード
	HEX_ITU.TMDR.BIT.FDIR= 1;  // オーバフロー・アンダーフロー検知
	HEX_ITU2.TCNT= 0;
	HEX_ITU.TSTR.BIT.STR2= 1;  // 1:タイマスタート

	// *** for RW ***
	// --- ITU0: カウント ---
	HEX_ITU0.TCR.BIT.CCLR= 0;  // クリア禁止
	HEX_ITU0.TCR.BIT.CKEG= 2;  // 0:立ち上がり，1:立下り，2:両エッジ
	HEX_ITU0.TCR.BIT.TPSC= 6;  // 外部クロックC: TCLKC端子入力でカウント 110
	HEX_ITU0.TCNT= 0;
	HEX_ITU.TSTR.BIT.STR0= 1;  // 1:タイマスタート

	// --- ITU1: カウント ---
	HEX_ITU1.TCR.BIT.CCLR= 0;  // クリア禁止
	HEX_ITU1.TCR.BIT.CKEG= 2;  // 0:立ち上がり，1:立下り，2:両エッジ
	HEX_ITU1.TCR.BIT.TPSC= 7;  // 外部クロックD: TCLKD端子入力でカウント 111
	HEX_ITU1.TCNT= 0;
	HEX_ITU.TSTR.BIT.STR1= 1;  // 1:タイマスタート
}


// --------------------------
// --- パラメータの初期化 ---
// --------------------------
static void
initialize_para( void )
{
	int  jnt;

	// ----------------------
	// --- encoder の設定 ---
	// ----------------------
	Ct.enco[LW].present.d= 0;
	Ct.tmp_enco_val[LW]  = 0;

	Ct.enco[RW].present.d= 0;
	Ct.tmp_enco_val[RW]  = 0;
}


// ------------------------------
// --- エンコーダ値の読込関数 ---
// ------------------------------
static int
enco(int no)
{
	Ct.enco[no].last.d= Ct.enco[no].present.d;

	if( no==LW ){
		Ct.enco[no].present.d= - HEX_ITU2.TCNT;  // 4逓倍
	}
	else if( no==RW ){
		Ct.enco[no].present.d= (HEX_ITU0.TCNT - HEX_ITU1.TCNT)*2;  // 2逓倍
	}

	if( Ct.enco[no].present.d > 32767 ){
		Ct.enco[no].present.d -= 65536;
	}	

	Ct.enco[no].present.delta= Ct.enco[no].present.d - Ct.enco[no].last.d;

	if( Ct.enco[no].present.delta < -30000 ){
		Ct.enco[no].present.delta += 65536;
	}
	else if( Ct.enco[no].present.delta > 30000 ){
		Ct.enco[no].present.delta -= 65536;
	}

	Ct.tmp_enco_val[no] += Ct.enco[no].present.delta;

	return( Ct.tmp_enco_val[no] );
}
