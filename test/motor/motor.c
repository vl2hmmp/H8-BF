// ****************************************
// *** motor �̓���e�X�g�v���O����     ***
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
// --- �֐��Q ---
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
    	
	init_settings();  // H8�̏����ݒ�
	
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
// --- H8�̏����ݒ�֐� ---
// ------------------------
static void
init_settings( void )
{
	// --- �|�[�g���o�͐ݒ� ---
	P4.DDR= 0xff;  // output

	// ****************
	// *** for Left ***
	// ****************
	// --- 8bit timer pwm settings 3069�}�j���A�� pp.482(��̃y�[�W) �Q�� ---
	// TCR: �^�C�}�R���g���[�����W�X�^
	// [��/8192 ->3] ,[��/64 ->2] ,[��/8 ->1]
	OCT_ITU0.TCR.BIT.CKS= 2;  // ��/64

	// TCNT���R���y�A�}�b�`A�ŃN���A: CCLR0=1, CCLR1=0, pp.462
	OCT_ITU0.TCR.BIT.CCLR= 1;  

	// TCSR: �^�C�}�R���g���[��/�X�e�[�^�X���W�X�^
	// �R���y�A�}�b�`B��0�o��: OIS3 �� OIS2 �� 01 �ɐݒ�, pp.467
	OCT_ITU0.TCSR.BIT.OIS32= 1;

	// �R���y�A�}�b�`A��1�o��: OS1 �� OS0 �� 10 �ɐݒ�, pp.468
	OCT_ITU0.TCSR.BIT.OS10= 2;

	// TCRB: �^�C���R���X�^���g���W�X�^B
	// Duty
	OCT_ITU0.TCORB= 0;			

	// TCRB: �^�C���R���X�^���g���W�X�^A
	// ���g��: 20MHz(Clock)/64(��)/250(TCORA)= 1.25kHz
	OCT_ITU0.TCORA= Max_duty; 			

	// �X�^�[�g
	MOTOR_0_RUN;

	// *****************
	// *** for Right ***
	// *****************
	OCT_ITU1.TCR.BIT.CKS=    2; // ��/64
	OCT_ITU1.TCR.BIT.CCLR=   1;  
	OCT_ITU1.TCSR.BIT.OIS32= 1;	// // 1
	OCT_ITU1.TCSR.BIT.OS10=  2;	// // 2
	OCT_ITU1.TCORB= 0;			
	OCT_ITU1.TCORA= Max_duty; 			
	MOTOR_1_RUN;
}


// ----------------------
// --- ���[�^�쓮�֐� ---
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
		MOTOR_0_DUTY= Max_duty;  // 0:�X�g�b�v�i�������~�܂�j, Max_duty:�u���[�L�i�}�Ɏ~�܂�j
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
		MOTOR_1_DUTY= Max_duty;  // 0:�X�g�b�v�i�������~�܂�j, Max_duty:�u���[�L�i�}�Ɏ~�܂�j
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
