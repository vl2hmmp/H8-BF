// *******************************************
// *** encoder �ǂݍ��݃e�X�g�v���O����    ***
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
// --- �֐��Q ---
// --------------
static void  init_settings();
static void  initialize_para( void );
static int   enco( int no );  // ����H8�ް�ނł́Cint=long�̈���


// ***************
// *** main () ***
// ***************
int
main( void )
{
	init_settings();    // H8�̏����ݒ�
	initialize_para();  // �p�����[�^�̐ݒ�

	while(1){
		printf(" %d  %d \r", enco(LW), enco(RW) );
	}

	exit(0);
}


// ------------------------
// --- H8�̏����ݒ�֐� ---
// ------------------------
static void
init_settings( void )
{
	// *** for LW ***
	// --- ITU2 �ʑ��W�����[�h�ݒ� ---
	HEX_ITU.TMDR.BIT.MDF=  1;  // �ʑ��W�����[�h
	HEX_ITU.TMDR.BIT.FDIR= 1;  // �I�[�o�t���[�E�A���_�[�t���[���m
	HEX_ITU2.TCNT= 0;
	HEX_ITU.TSTR.BIT.STR2= 1;  // 1:�^�C�}�X�^�[�g

	// *** for RW ***
	// --- ITU0: �J�E���g ---
	HEX_ITU0.TCR.BIT.CCLR= 0;  // �N���A�֎~
	HEX_ITU0.TCR.BIT.CKEG= 2;  // 0:�����オ��C1:������C2:���G�b�W
	HEX_ITU0.TCR.BIT.TPSC= 6;  // �O���N���b�NC: TCLKC�[�q���͂ŃJ�E���g 110
	HEX_ITU0.TCNT= 0;
	HEX_ITU.TSTR.BIT.STR0= 1;  // 1:�^�C�}�X�^�[�g

	// --- ITU1: �J�E���g ---
	HEX_ITU1.TCR.BIT.CCLR= 0;  // �N���A�֎~
	HEX_ITU1.TCR.BIT.CKEG= 2;  // 0:�����オ��C1:������C2:���G�b�W
	HEX_ITU1.TCR.BIT.TPSC= 7;  // �O���N���b�ND: TCLKD�[�q���͂ŃJ�E���g 111
	HEX_ITU1.TCNT= 0;
	HEX_ITU.TSTR.BIT.STR1= 1;  // 1:�^�C�}�X�^�[�g
}


// --------------------------
// --- �p�����[�^�̏����� ---
// --------------------------
static void
initialize_para( void )
{
	int  jnt;

	// ----------------------
	// --- encoder �̐ݒ� ---
	// ----------------------
	Ct.enco[LW].present.d= 0;
	Ct.tmp_enco_val[LW]  = 0;

	Ct.enco[RW].present.d= 0;
	Ct.tmp_enco_val[RW]  = 0;
}


// ------------------------------
// --- �G���R�[�_�l�̓Ǎ��֐� ---
// ------------------------------
static int
enco(int no)
{
	Ct.enco[no].last.d= Ct.enco[no].present.d;

	if( no==LW ){
		Ct.enco[no].present.d= - HEX_ITU2.TCNT;  // 4���{
	}
	else if( no==RW ){
		Ct.enco[no].present.d= (HEX_ITU0.TCNT - HEX_ITU1.TCNT)*2;  // 2���{
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
