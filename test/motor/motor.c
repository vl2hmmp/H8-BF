// ****************************************
// *** motor の動作テストプログラム     ***
// ***    produced by Y. Mori           ***
// ***       special thanks to A. Ruike ***
// ****************************************
// tftp motor.elf 192.168.1.1
// motor.elf
#include "r3069.h"
#include <mes2.h>

// --- motor ---
#define  MOTOR_0_CW     P4.DR.BIT.B0= 1; P4.DR.BIT.B2= 0;
#define  MOTOR_0_CCW    P4.DR.BIT.B0= 0; P4.DR.BIT.B2= 1;
#define  MOTOR_0_RUN    P4.DR.BIT.B0= 0; P4.DR.BIT.B2= 0;
#define  MOTOR_0_BREAK  P4.DR.BIT.B0= 1; P4.DR.BIT.B2= 1;
#define  MOTOR_0_DUTY   OCT_ITU0.TCORB

#define  MOTOR_1_CW     P4.DR.BIT.B1= 1; P4.DR.BIT.B3= 0;
#define  MOTOR_1_CCW    P4.DR.BIT.B1= 0; P4.DR.BIT.B3= 1;
#define  MOTOR_1_RUN    P4.DR.BIT.B1= 0; P4.DR.BIT.B3= 0;
#define  MOTOR_1_BREAK  P4.DR.BIT.B1= 1; P4.DR.BIT.B3= 1;
#define  MOTOR_1_DUTY   OCT_ITU1.TCORB

#define  Max_duty    250
#define  Limit_duty  250
#define  Ratio_duty (Max_duty/Limit_duty)

// --- Joint number ---
#define  LW  0  //  Left  wheel
#define  RW  1  //  Right wheel


// --------------
// --- 関数群 ---
// --------------
static void  init_settings();
static void  motor( int no, int duty );


// ***************
// *** main () ***
// ***************
int
main( void )
{
	int jnt;
	int duty;
	int pwm= 0;
	int num;
	int confirm= 1;
	int i;
    	
	init_settings();  // H8の初期設定
	
	for (i=0; i<2; i++){
		motor(i, 0);
	}  

	while( confirm != 'q' ){
	
		printf(" q key: quit      \r");
		printf(" s key: pwm= 0    \r");	
		printf(" p key: speed + 1 \r");
		printf(" u key: speed +10 \r");
		printf(" n key: speed - 1 \r");
		printf(" d key: speed -10 \r");
        
		confirm= getchar();

		if(confirm == 'p'){ 
			pwm ++;
		}
		else if (confirm == 's'){
			pwm = 0;
		}
		else if (confirm == 'u'){ 
			pwm += 10;
		}
		else if (confirm == 'n'){
			pwm --;
		}
		else if (confirm == 'd'){
			pwm -= 10;
		}
		
		if( pwm >= Max_duty ){
			pwm = Max_duty;
		}
		else if( pwm <= -Max_duty ){
			pwm = -Max_duty;
		}
        
		printf("\n");
		printf(" motor[%d]-> %3d \r",num, pwm);
        
		motor (LW, pwm);
		motor (RW, pwm);
	}

	for(i=0; i<2; i++){
		motor(i, 0);
	}

	exit(0);
}


// ------------------------
// --- H8の初期設定関数 ---
// ------------------------
static void
init_settings( void )
{
	// --- ポート入出力設定 ---
	P4.DDR= 0xff;  // output

	// ****************
	// *** for Left ***
	// ****************
	// --- 8bit timer pwm settings 3069マニュアル pp.482(上のページ) 参照 ---
	// TCR: タイマコントロールレジスタ
	// [φ/8192 ->3] ,[φ/64 ->2] ,[φ/8 ->1]
	OCT_ITU0.TCR.BIT.CKS= 2;  // φ/64

	// TCNTをコンペアマッチAでクリア: CCLR0=1, CCLR1=0, pp.462
	OCT_ITU0.TCR.BIT.CCLR= 1;  

	// TCSR: タイマコントロール/ステータスレジスタ
	// コンペアマッチBで0出力: OIS3 と OIS2 を 01 に設定, pp.467
	OCT_ITU0.TCSR.BIT.OIS32= 1;

	// コンペアマッチAで1出力: OS1 と OS0 を 10 に設定, pp.468
	OCT_ITU0.TCSR.BIT.OS10= 2;

	// TCRB: タイムコンスタントレジスタB
	// Duty
	OCT_ITU0.TCORB= 0;			

	// TCRB: タイムコンスタントレジスタA
	// 周波数: 20MHz(Clock)/64(φ)/250(TCORA)= 1.25kHz
	OCT_ITU0.TCORA= Max_duty; 			

	// スタート
	MOTOR_0_RUN;

	// *****************
	// *** for Right ***
	// *****************
	OCT_ITU1.TCR.BIT.CKS=    2; // φ/64
	OCT_ITU1.TCR.BIT.CCLR=   1;  
	OCT_ITU1.TCSR.BIT.OIS32= 1;	// // 1
	OCT_ITU1.TCSR.BIT.OS10=  2;	// // 2
	OCT_ITU1.TCORB= 0;			
	OCT_ITU1.TCORA= Max_duty; 			
	MOTOR_1_RUN;
}


// ----------------------
// --- モータ駆動関数 ---
// ----------------------
static void 
motor( int no, int duty )
{
	int  real_duty= 0;

	switch( no ){

	// for Left
	case LW:
	if( duty > 0 ){
		real_duty= duty*Ratio_duty;

		if( real_duty >= Max_duty ){
			MOTOR_0_CW;
			MOTOR_0_DUTY= Max_duty;
		}
		else{
			MOTOR_0_CW;
			MOTOR_0_DUTY= real_duty;
		}
	}
	else if(duty == 0){
		MOTOR_0_DUTY= Max_duty;  // 0:ストップ（ゆっくり止まる）, Max_duty:ブレーキ（急に止まる）
		MOTOR_0_BREAK;
	}
	else{
		real_duty= - duty*Ratio_duty;

		if( real_duty >= Max_duty ){
			MOTOR_0_CCW;
			MOTOR_0_DUTY= Max_duty;
		}
		else{
		   	MOTOR_0_CCW;
			MOTOR_0_DUTY= real_duty;
		}
	}
	break;

	// for Right
	case RW:
	if( duty > 0 ){
		real_duty= duty*Ratio_duty;

		if( real_duty >= Max_duty ){
			MOTOR_1_CW;
			MOTOR_1_DUTY= Max_duty;
		}
		else{
			MOTOR_1_CW;
			MOTOR_1_DUTY= real_duty;
		}
	}
	else if(duty == 0){
		MOTOR_1_DUTY= Max_duty;  // 0:ストップ（ゆっくり止まる）, Max_duty:ブレーキ（急に止まる）
		MOTOR_1_BREAK;
	}
	else{
		real_duty= - duty*Ratio_duty;

		if( real_duty >= Max_duty ){
			MOTOR_1_CCW;
			MOTOR_1_DUTY= Max_duty;
		}
		else{
		   	MOTOR_1_CCW;
			MOTOR_1_DUTY= real_duty;
		}
	}
	break;

	default: break;
	}
}
