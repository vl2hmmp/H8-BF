/*
 * @file main.c
 * @brief �A�v���P�[�V�����̃��C���G���g���|�C���g�ł��B
 * @date 2017/08/21
 * @author �˗с@�D
 */

#include "main.h"
#include "line_trace.h"

/*
* @brief �A�v���P�[�V�����̃��C���G���g���|�C���g�ł��B
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
* @brief �������������܂��B
* @detail H8�}�C�R������уv���O�����̏����ݒ���s���܂��B�v���O�����̍ŏ��ɂ��̊֐����Ăяo���K�v������܂��B
*/
void initilize()
{
	// *** NMI �ݒ�i���Z�b�g�p�j***
	SYSCR.BIT.NMIEG = 0;
	set_handler(7, prg_end);

	// *** �|�[�g���o�͐ݒ� (for motor, photo-sensor) ***
	P4.DDR = 0xff;  // output (for motor)
	P5.DDR = 0x00;  // input  (for photo-sensor)

					// *** 8bit timer pwm settings (for motor) 3069�}�j���A�� pp.482 �Q�� ***
					// [��/8192 ->3] ,[��/64 ->2] ,[��/8 ->1]
	OCT_ITU0.TCR.BIT.CKS = 2;
	OCT_ITU1.TCR.BIT.CKS = 2;
	//OCT_ITU2.TCR.BIT.CKS= 2;  // �\���|�[�g

	// TCNT���R���y�A�}�b�`A�ŃN���A: CCLR0=1, CCLR1=0, pp.462
	OCT_ITU0.TCR.BIT.CCLR = 1;
	OCT_ITU1.TCR.BIT.CCLR = 1;
	//OCT_ITU2.TCR.BIT.CCLR= 1;  // �\���|�[�g

	// �R���y�A�}�b�`B��1�o��: OIS3 �� OIS2 �� 01 �ɐݒ�, pp.467
	OCT_ITU0.TCSR.BIT.OIS32 = 1;
	OCT_ITU1.TCSR.BIT.OIS32 = 1;
	//OCT_ITU2.TCSR.BIT.OIS32= 1;  // �\���|�[�g

	// �R���y�A�}�b�`A��0�o��: OS1 �� OS0 �� 10 �ɐݒ�, pp.468
	OCT_ITU0.TCSR.BIT.OS10 = 2;
	OCT_ITU1.TCSR.BIT.OS10 = 2;
	//OCT_ITU2.TCSR.BIT.OS10= 2;  // �\���|�[�g

	// Duty
	OCT_ITU0.TCORB = 0;
	OCT_ITU1.TCORB = 0;
	//OCT_ITU2.TCORB= 0;  // �\���|�[�g

	// TCRB: �^�C���R���X�^���g���W�X�^A
	// ���g��: 20MHz(Clock)/64(��)/250(TCORA)= 1.25kHz
	OCT_ITU0.TCORA = Max_duty;
	OCT_ITU1.TCORA = Max_duty;
	//OCT_ITU2.TCORA= 250;  // �\���|�[�g

	// *** ITU3 �^�C�}���荞�� 8bit (for timer) ***
	// OCT_ITU3.TCORA: �^�C���R���X�^���g���W�X�^A pp.465
	// 1�b�ԂɌĂяo�����feed�֐��̉񐔂Ɠ����D
	// 20000000:�V�X�e���N���b�N�C8192:�����l(r3069.h��197�s�� CKS:3 (CKS2=0, CKS1=1, CKS0=1))
	// �����N���b�N=�V�X�e���N���b�N/�����l pp.463
	OCT_ITU3.TCORA = (20000000 / 8192)*SampTime / 1000;
	OCT_ITU3.TCNT = 0;  // �J�E���^, TCORA��8TCNT�̒l�͏�ɔ�r����Ă���Dpp.460
	OCT_ITU3.TCSR.BYTE = 0x00;
	OCT_ITU3.TCR.BIT.CCLR = 1;  // �J�E���^�N���A 1:�R���y�A�}�b�`A�ɂ��N���A pp.462
	OCT_ITU3.TCR.BIT.CMIEA = 1;
	OCT_ITU3.TCR.BIT.CKS = 3;  // �^�C�}�[�J�n 011: CK2=0, CK1=1, CK0=1 -> ��/8192, pp.463
							   // �^�C�}�[��~ 000: CK2=0, CK1=0, CK0=0 -> pp.463
	set_handler(42, feed);

	MOTOR_0_RUN;
	MOTOR_1_RUN;
}

/*
* @brief ���̏I�������ł��B
* @detail �v���O�����̍Ō�ɂ��̊֐����Ăяo���K�v������܂��B
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
* @brief ��莞�Ԃ����Ɏ��s����鏈���ł��B
*/
#pragma interrupt
void feed()
{
	STOP_ENCO_COUNT;

	OCT_ITU3.TCSR.BIT.CMFA = 0;
}
