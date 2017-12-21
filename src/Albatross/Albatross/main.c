/*
 * @file main.c
 * @brief アプリケーションのメインエントリポイントです。
 * @date 2017/08/21
 * @author 桐林　颯
 */

#include "main.h"
#include "line_trace.h"

/*
* @brief アプリケーションのメインエントリポイントです。
*/
int main()
{
	initilize();

	START_FEED;

	printf("\r Press Start Button.\r\n");
	while(P6.DR.BIT.B0);

	//trace();
}

/*
* @brief 環境を初期化します。
* @detail H8マイコンおよびプログラムの初期設定を行います。プログラムの最初にこの関数を呼び出す必要があります。
*/
void initilize()
{
	// *** NMI 設定（リセット用）***
	SYSCR.BIT.NMIEG = 0;
	set_handler(7, prg_end);

	// *** ポート入出力設定 (for motor, photo-sensor) ***
	P4.DDR = 0xff;  // output (for motor)
	P5.DDR = 0x00;  // input  (for photo-sensor)

					// *** 8bit timer pwm settings (for motor) 3069マニュアル pp.482 参照 ***
					// [φ/8192 ->3] ,[φ/64 ->2] ,[φ/8 ->1]
	OCT_ITU0.TCR.BIT.CKS = 2;
	OCT_ITU1.TCR.BIT.CKS = 2;
	//OCT_ITU2.TCR.BIT.CKS= 2;  // 予備ポート

	// TCNTをコンペアマッチAでクリア: CCLR0=1, CCLR1=0, pp.462
	OCT_ITU0.TCR.BIT.CCLR = 1;
	OCT_ITU1.TCR.BIT.CCLR = 1;
	//OCT_ITU2.TCR.BIT.CCLR= 1;  // 予備ポート

	// コンペアマッチBで1出力: OIS3 と OIS2 を 01 に設定, pp.467
	OCT_ITU0.TCSR.BIT.OIS32 = 1;
	OCT_ITU1.TCSR.BIT.OIS32 = 1;
	//OCT_ITU2.TCSR.BIT.OIS32= 1;  // 予備ポート

	// コンペアマッチAで0出力: OS1 と OS0 を 10 に設定, pp.468
	OCT_ITU0.TCSR.BIT.OS10 = 2;
	OCT_ITU1.TCSR.BIT.OS10 = 2;
	//OCT_ITU2.TCSR.BIT.OS10= 2;  // 予備ポート

	// Duty
	OCT_ITU0.TCORB = 0;
	OCT_ITU1.TCORB = 0;
	//OCT_ITU2.TCORB= 0;  // 予備ポート

	// TCRB: タイムコンスタントレジスタA
	// 周波数: 20MHz(Clock)/64(φ)/250(TCORA)= 1.25kHz
	OCT_ITU0.TCORA = Max_duty;
	OCT_ITU1.TCORA = Max_duty;
	//OCT_ITU2.TCORA= 250;  // 予備ポート

	// *** ITU3 タイマ割り込み 8bit (for timer) ***
	// OCT_ITU3.TCORA: タイムコンスタントレジスタA pp.465
	// 1秒間に呼び出されるfeed関数の回数と同じ．
	// 20000000:システムクロック，8192:分周値(r3069.hの197行目 CKS:3 (CKS2=0, CKS1=1, CKS0=1))
	// 内部クロック=システムクロック/分周値 pp.463
	OCT_ITU3.TCORA = (20000000 / 8192)*SampTime / 1000;
	OCT_ITU3.TCNT = 0;  // カウンタ, TCORAと8TCNTの値は常に比較されている．pp.460
	OCT_ITU3.TCSR.BYTE = 0x00;
	OCT_ITU3.TCR.BIT.CCLR = 1;  // カウンタクリア 1:コンペアマッチAによりクリア pp.462
	OCT_ITU3.TCR.BIT.CMIEA = 1;
	OCT_ITU3.TCR.BIT.CKS = 3;  // タイマー開始 011: CK2=0, CK1=1, CK0=1 -> φ/8192, pp.463
							   // タイマー停止 000: CK2=0, CK1=0, CK0=0 -> pp.463
	set_handler(42, feed);

	MOTOR_0_RUN;
	MOTOR_1_RUN;
}

/*
* @brief 環境の終了処理です。
* @detail プログラムの最後にこの関数を呼び出す必要があります。
*/
#pragma interrupt
void prg_end()
{
	STOP_FEED;
	load_segment(7);
	MOTOR_0_BREAK;
	MOTOR_1_BREAK;
}


/*
* @brief 一定時間おきに実行される処理です。
*/
#pragma interrupt
void feed()
{
	STOP_ENCO_COUNT;

	OCT_ITU3.TCSR.BIT.CMFA = 0;
}
