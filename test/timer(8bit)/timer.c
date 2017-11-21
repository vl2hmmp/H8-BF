// *********************************************
// *** �^�C�}���荞�� �̓���e�X�g�v���O���� ***
// ***   8bit �o�[�W�����@�@�@�@�@�@�@�@�@�@ ***
// ***                   produced by Y. Mori ***
// ***   special thanks to A. Ruike, S. Kido ***
// *********************************************
// tftp timer.elf 192.168.1.1
// timer.elf

#include "r3069.h"
#include <mes2.h>

static int  Count_feed;  // ���荞�݉񐔁C�O���ϐ��Ƃ���
static int  Count_sec;   // 1[s]�Ԃ� feed()�ɓ����

static void  init_settings( void );


// ***************
// *** feed () ***
// *************** 
#pragma interrupt  
void 
feed(void )
{
	load_segment(42);  // 24, �x�N�^�ԍ��i���ϐ����g���Ƃ��ɕK�v�j�Cpp.143

	Count_feed ++;

	// TSCR:�^�C�}�R���g���[���X�e�[�^�X���W�X�^ pp.464
	// CMFA:TCORA �̃R���y�A�}�b�`�̔����������X�e�[�^�X�t���O pp.465
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
		sleep(10);  // ������\�������̂�h������

	    if( Count_feed % Count_sec == 0 ){
			printf(" %d [s]\r", (int)(Count_feed/Count_sec) );
		}
	}	
}

// ------------------------
// --- H8�̏����ݒ�֐� ---
// ------------------------
static void
init_settings( void )
{
	OCT_ITU3.TCNT  = 0;   // �J�E���^, TCORA��8TCNT�̒l�͏�ɔ�r����Ă���Dpp.460
	OCT_ITU3.TCORA = 49;  // �^�C���R���X�^���g���W�X�^A
	OCT_ITU3.TCSR.BYTE = 0x00;
	OCT_ITU3.TCR.BIT.CCLR  = 1;  // �J�E���^�N���A 1:�R���y�A�}�b�`A�ɂ��N���A pp.462
	OCT_ITU3.TCR.BIT.CMIEA = 1;
	OCT_ITU3.TCR.BIT.CKS   = 3;  // �^�C�}�[�J�n 011: CK2=0, CK1=1, CK0=1 -> ��/8192, pp.463
	                             // �^�C�}�[��~ 000: CK2=0, CK1=0, CK0=0 -> pp.463
	set_handler( 42, feed );

	Count_feed= 0;
	Count_sec= 20000000/8192/OCT_ITU3.TCORA;  // 20MHz/8192/TCORA
}
