// *****************************************
// *** enc_common.h (共通変数，共通定数) ***
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

	int    enco_dir[Jnum];      // エンコーダの回転方向
	int    tmp_enco_val[Jnum];  // エンコーダカウント用仮変数

} ct_sharedType;

#endif
