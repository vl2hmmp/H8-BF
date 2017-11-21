// *****************************************
// *** enc_common.h (���ʕϐ��C���ʒ萔) ***
// ***           produced by Y. Mori     ***
// *****************************************
#ifndef __ENC_COMMON_H
#define __ENC_COMMON_H

// ************************
// *** Common Parameter ***
// ************************

// --- Joint number ---
#define  Jnum  2
#define  LW  0  //  Left wheel
#define  RW  1  // Right wheel


// **********************
// *** For Controller ***
// **********************

// --- encoder count type ---
typedef struct{
	int  d, delta;  // pos, delta

} ct_eCountType;


// --- encoder type ---
typedef struct{
	ct_eCountType  present, last;

} ct_encoType;


// --- control shared type ---
typedef struct{
	ct_encoType   enco[Jnum];

	int    enco_dir[Jnum];      // �G���R�[�_�̉�]����
	int    tmp_enco_val[Jnum];  // �G���R�[�_�J�E���g�p���ϐ�

} ct_sharedType;

#endif
