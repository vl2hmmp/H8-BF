#include "main.h"

static int  End_flag;   // プログラムの終了フラグ
static int  Feed_flag;  // フィードバック関数内での制御フラグ
static int  All_black_flag, All_white_flag;  // ライントレース用フラグ

ki_sharedType  Ki;
ct_sharedType  Ct;


// --------------
// --- 関数群 ---
// --------------
static void  line_trace(void);

static void  traj_tracking(float x_f, float tht_f, float t_f);
static void  traj_plan_3(int jnt, float x_i, float x_f, float t_i, float t_f);
static void  make_traj(float t_i, float t_f);

static void  init_settings(void);
static void  initialize_para(void);
static void  cool_down(void);

static void  start_time(ct_timeType *p);
static void  reset_time(ct_timeType *p);
static int   inc_time(ct_timeType *p);

static void  motor(int no, int duty);
static int   enco(int no);  // 3069 H8ﾎﾞｰﾄﾞでは，int は long の扱い

static void  save_data(int file_number);


// ************************
// *** 割り込み処理関数 ***
// *** リセットボタン用 ***
// ************************
#pragma interrupt
void prg_end(void)
{
	STOP_FEED;  // feedback関数のクロック入力禁止

	load_segment(7);
	cool_down();
	End_flag = 1;
}


// ************************
// *** 割り込み処理関数 ***
// *** feedback 関数    ***
// ************************
#pragma interrupt
void
feed(void)
{
	int jnt;

	load_segment(42);  // ベクタ番号（大域変数を使うときに必要），pp.143

					   // --------------------------------------
					   // --- エンコーダを用いた閉ループ制御 ---
					   // --------------------------------------	
	if (Feed_flag == ENCO) {

		for (jnt = LW; jnt <= RW; jnt++) {

			// --- 車輪の回転角を計測 ---
			Ct.jnt[jnt].last.d = Ct.jnt[jnt].present.d;
			Ct.jnt[jnt].present.d = (float)Ct.enco_dir[jnt] * enco(jnt)*(Ct.WPulse2Rad) / (Ct.Tr);
			Ct.jnt[jnt].present.v = (Ct.jnt[jnt].present.d - Ct.jnt[jnt].last.d) / RsampTime;

			// --- 車輪の制御 ---
			Ct.jnt[jnt].delta = (Ct.start_present_d[jnt] + Ki.traj[jnt][Ct.dTime.val].d)
				- (Ct.jnt[jnt].present.d);

			// --- 出力電圧を計算 ---
			Ct.pwm[jnt] = (int)((Ct.jnt[jnt].gain.kp)*(Ct.jnt[jnt].delta)
				- (Ct.jnt[jnt].gain.kd)*(Ct.jnt[jnt].present.v));

			motor(jnt, (Ct.mot_dir[jnt])*Ct.pwm[jnt]);

			// --- データの保存 ---
			Ct.data[jnt][Ct.dTime.val].d = Ct.jnt[jnt].present.d - Ct.start_present_d[jnt];
		}

		// --- 時間を進める ---
		Ct.timeFlag = inc_time(&Ct.dTime);
	}
	// ----------------------------------------------
	// --- フォトインタラプタを用いた開ループ制御 ---
	// ----------------------------------------------	
	else if (Feed_flag == PHOTO) {

		// エンコーダのカウント停止
		STOP_ENCO_COUNT;

		line_trace();
	}
	// ----------------
	// --- 静止状態 ---
	// ----------------
	else if (Feed_flag == STOP) {

		// モータ停止
		for (jnt = LW; jnt <= RW; jnt++) {
			motor(jnt, 0);
		}

		// エンコーダのカウント停止
		STOP_ENCO_COUNT;
	}

	// TSCR:タイマコントロールステータスレジスタ pp.464
	// CMFA:TCORA のコンペアマッチの発生を示すステータスフラグ pp.465
	OCT_ITU3.TCSR.BIT.CMFA = 0;
}


// *****************
// *** main 関数 ***
// *****************
int
main(void)
{
	int  lap_flag;
	int  jnt;

	init_settings();    // H8の初期設定
	initialize_para();  // パラメータの設定

	START_FEED;	 // フィードバック関数スタート（割り込み処理開始）

	printf("\r Press Start Button.\r\n");
	while (P6.DR.BIT.B0);

	// --------------------
	// --- 軌道追従制御 ---
	// --------------------
	// traj_tracking( x_f[cm],  tht_f[deg],  t_f[s] );
	Feed_flag = ENCO;
	traj_tracking(65.0f, 0.0f, 3.0f);
	Feed_flag = STOP;
#if 1
	save_data(1);  // データのセーブ
#endif	

	Feed_flag = ENCO;
	// ( x_f[cm],  tht_f[deg],  t_f[s] );
	traj_tracking(-26.0f, -90.0f, 2.0f);
	Feed_flag = STOP;
#if 1
	save_data(2);  // データのセーブ
#endif

	Feed_flag = ENCO;
	// ( x_f[cm],  tht_f[deg],  t_f[s] );
	traj_tracking(-25.0f, 0.0f, 2.0f);
	Feed_flag = STOP;
#if 1
	save_data(3);  // データのセーブ
#endif

	cool_down();
	exit(0);
}


// ----------------------
// --- ライントレース ---
// ----------------------
static void
line_trace(void) {
}


// --------------------------------------
// --- エンコーダを用いた軌道追従制御 ---
// --------------------------------------
static void
traj_tracking(float x_f, float tht_f, float t_f) {

	int  confirm;
	int  i, jnt, d;
	float  x_i, tht_i, t_i;

	static unsigned short  start_times[150];
	static unsigned short    end_times[150];

	x_i = 0.0f;
	tht_i = 0.0f;
	t_i = 0.0f;

	traj_plan_3(DDis, x_i, x_f*0.01f, t_i, t_f);
	traj_plan_3(DTht, tht_i, tht_f*Deg2Rad, t_i, t_f);

	make_traj(t_i, t_f);

	Ct.dTime.stopVal = (int)((t_f - t_i) * 1000 / SampTime);
	start_time(&Ct.dTime);

	START_ENCO_COUNT;  // エンコーダのカウント開始

	while (Ct.timeFlag != Reached) {

#if 0
		printf("\n");
		printf("%d[ms]  LW_traj=%d[mm]  LW=%d[mm]  RW_traj=%d[mm]  RW=%d[mm] \r",
			(Ct.dTime.val)*SampTime,
			(int)(1000 * (Ki.traj[LW][Ct.dTime.val].d)*Ki.r),
			(int)(1000 * (Ct.jnt[LW].present.d - Ct.start_present_d[LW])*Ki.r),
			(int)(1000 * (Ki.traj[RW][Ct.dTime.val].d)*Ki.r),
			(int)(1000 * (Ct.jnt[RW].present.d - Ct.start_present_d[RW])*Ki.r)
		);
#endif

		if (End_flag == 1) {
			exit(0);
		}
	}

	reset_time(&Ct.dTime);

	// 現在位置を取得
	for (jnt = LW; jnt <= RW; jnt++) {
		Ct.start_present_d[jnt] = Ct.jnt[jnt].present.d;
	}

#if 1
	printf(" traj_tracking_END: (x_f, tht_f)=(%d, %d)\n\r", (int)x_f, (int)tht_f); // 正常終了
#endif
}


// --------------------------------
// --- 目標の本体速度軌道を生成 ---
// --------------------------------
static void
traj_plan_3(int jnt, float x_i, float x_f, float t_i, float t_f)
{
	char  buf[150];
	int   i;
	int   imti, imtf;
	float j;
	float a0, a2, a3;
	float pos_present = 0.0f, pos_last = 0.0f;

	a0 = x_i;
	a2 = 3.0f*(x_f - x_i) / ((t_f - t_i)*(t_f - t_i));
	a3 = -2.0f*(x_f - x_i) / ((t_f - t_i)*(t_f - t_i)*(t_f - t_i));

	imti = (int)(t_i / RsampTime);
	imtf = (int)(t_f / RsampTime);

	// 速度の積分値がちょうど目標位置になるように，あえて直接速度を求めない．
	// ただし，速度から求めた位置は，ごくわずかにずれることが確認された．
	for (i = imti; i <= imtf; i++) {
		pos_last = pos_present;

		j = ((float)i - (float)imti)*RsampTime;

		pos_present = a0 + a2*j*j + a3*j*j*j;

		if (i == imti || i == imtf) {
			Ki.in[jnt][i] = 0.0f;
		}
		else {
			Ki.in[jnt][i] = (pos_present - pos_last) / RsampTime;
		}

		// for debug
#if 0
		if (jnt == DDis) {
			printf("%d \n\r", (int)(1000 * pos_present));
		}
#endif
	}
}


// ----------------------------
// --- 目標の車輪軌道を生成 ---
// ----------------------------
static void
make_traj(float t_i, float t_f)
{
	int  i, jnt;
	int  imti, imtf;

	imti = (int)(t_i / RsampTime);
	imtf = (int)(t_f / RsampTime);

	for (i = imti; i <= imtf; i++) {

		for (jnt = LW; jnt <= RW; jnt++) {

			// --- 各車輪の目標角速度を求める ---
			if (jnt == LW) {
				Ki.traj[jnt][i].v =
					(Ki.in[DDis][i] - (Ki.W)*(Ki.in[DTht][i]) / 2.0f) / (Ki.r);
			}
			else if (jnt == RW) {
				Ki.traj[jnt][i].v =
					(Ki.in[DDis][i] + (Ki.W)*(Ki.in[DTht][i]) / 2.0f) / (Ki.r);
			}
			if (i >= 1) {
				Ki.traj[jnt][i].d = (Ki.traj[jnt][i - 1].d) + (Ki.traj[jnt][i].v)*RsampTime;
			}
			else {
				Ki.traj[jnt][i].d = 0;
			}

			// for debug
#if 0
			if (jnt == LW) {
				printf(" %d[ms]  %d[mm] \r", i*SampTime, (int)(1000 * Ki.traj[jnt][i].d*(Ki.r) + 0.5f));
			}
#endif
		}
	}
}


// ------------------------
// --- H8の初期設定関数 ---
// ------------------------
static void init_settings(void)
{
	// *** NMI 設定（リセット用）***
	SYSCR.BIT.NMIEG = 0;
	set_handler(7, prg_end);

	// *** ITU2 位相係数モード設定 (for Left encoder) ***
	HEX_ITU.TMDR.BIT.MDF = 1;  // 位相係数モード
	HEX_ITU.TMDR.BIT.FDIR = 1;  // オーバフロー・アンダーフロー検知
	HEX_ITU.TSTR.BIT.STR2 = 1;  // 1:タイマスタート, 0:ストップ
	HEX_ITU2.TCNT = 0;

	// *** ITU0, ITU1 カウンタ (for Right encoder) ***
	// --- ITU0: カウント ---
	HEX_ITU0.TCR.BIT.CCLR = 0;  // クリア禁止
	HEX_ITU0.TCR.BIT.CKEG = 2;  // 0:立ち上がり，1:立下り，2:両エッジ
	HEX_ITU0.TCR.BIT.TPSC = 6;  // 外部クロックC: TCLKC端子入力でカウント 110
	HEX_ITU0.TCNT = 0;
	HEX_ITU.TSTR.BIT.STR0 = 1;  // 1:タイマスタート，0:ストップ
								// --- ITU1: カウント ---
	HEX_ITU1.TCR.BIT.CCLR = 0;  // クリア禁止
	HEX_ITU1.TCR.BIT.CKEG = 2;  // 0:立ち上がり，1:立下り，2:両エッジ
	HEX_ITU1.TCR.BIT.TPSC = 7;  // 外部クロックD: TCLKD端子入力でカウント 111
	HEX_ITU1.TCNT = 0;
	HEX_ITU.TSTR.BIT.STR1 = 1;  // 1:タイマスタート，0:ストップ

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
}


// --------------------------
// --- パラメータの初期化 ---
// --------------------------
static void
initialize_para(void)
{
	int  jnt;

	// --- フラグの初期化 ---
	End_flag = 0;

	Feed_flag = STOP;

	All_black_flag = 0;
	All_white_flag = 0;

	// --- 初期時間をセット ---
	Ct.dTime.startVal = 0;

	// --- 変数の初期化 ---
	for (jnt = LW; jnt <= RW; jnt++) {
		Ct.jnt[jnt].present.d = 0;
	}

	// --- ハードウェアパラメータをセット ---
	// ホイールが一回転回るとき (Ki.r)*2*PI [cm] <-> (Ct.Tr)*(128*4) [pulse] 
	// 0.0013725 [cm/pulse]
	//Ki.W= 0.2f;    // ホイール間距離 0.199   [m]
	Ki.W = 0.204f * 1.02;    // ホイール間距離 0.199   [m]
							 //Ki.r= 0.022f;  // ホイール半径   0.02125 [m]
	Ki.r = 0.02139f * 0.99;  // 計測値　0.02139 [m]
	Ct.WPulse2Rad = 2.0f*PI / 512.0f;  // エンコーダのパルスを回転角[rad]に変換, 512=128[pulse]*4[逓倍]
	Ct.Tr = 29.0f;    // モータ減速比  

					  // --- ロボットのゲインをセット ---
	Ct.jnt[LW].gain.kp = 80.0f;
	Ct.jnt[LW].gain.kd = 80.0f * RsampTime;

	Ct.jnt[RW].gain.kp = 80.0f;
	Ct.jnt[RW].gain.kd = 80.0f * RsampTime;

	// -------------------
	// --- motorの設定 ---
	// -------------------
	// 回転方向をセット
	Ct.mot_dir[LW] = -1;
	Ct.mot_dir[RW] = 1;

	MOTOR_0_RUN;
	MOTOR_1_RUN;

	// ----------------------
	// --- encoder の設定 ---
	// ----------------------
	// 回転方向をセット
	Ct.enco_dir[LW] = -1;
	Ct.enco_dir[RW] = 1;

	// 変数の初期化
	for (jnt = LW; jnt <= RW; jnt++) {
		Ct.enco[jnt].present.d = 0;
		Ct.tmp_enco_val[jnt] = 0;
		Ct.start_present_d[jnt] = 0;
	}
}


// ------------------------------
// --- プログラム終了時の処理 ---
// ------------------------------
static void
cool_down(void)
{
	STOP_FEED;  // feedback関数のクロック入力禁止  pp.462

	motor(LW, 0);
	motor(RW, 0);

	MOTOR_0_BREAK;
	MOTOR_1_BREAK;
}


// ------------------------
// --- 時間スタート関数 ---
// ------------------------
static void
start_time(ct_timeType *p)
{
	p->val = p->startVal;
	p->inc = UntilInc;
}


// ----------------------
// --- 時間をリセット ---
// ----------------------
static void
reset_time(ct_timeType *p)
{
	p->val = p->startVal;
	p->inc = Stopped;
}


// --------------------
// --- 時間更新関数 ---
// --------------------
static int
inc_time(ct_timeType *p)
{
	int ret;

	ret = !Reached;

	if (p->inc != Stopped) {
		if (p->val < p->stopVal) {
			p->val++;
		}
		else {
			ret = Reached;
		}
	}
	return(ret);
}


// ----------------------
// --- モータ駆動関数 ---
// ----------------------
static void
motor(int no, int duty)
{
	int  real_duty = 0;

	switch (no) {

		// for Left
	case LW:
		if (duty > 0) {
			real_duty = duty*Ratio_duty;

			if (real_duty >= Max_duty) {
				MOTOR_0_CW;
				MOTOR_0_DUTY = Max_duty;
			}
			else {
				MOTOR_0_CW;
				MOTOR_0_DUTY = real_duty;
			}
		}
		else if (duty == 0) {
			MOTOR_0_DUTY = Max_duty;  // 0:ストップ（ゆっくり止まる）, Max_duty:ブレーキ（急に止まる）
			MOTOR_0_BREAK;
		}
		else {
			real_duty = -duty*Ratio_duty;

			if (real_duty >= Max_duty) {
				MOTOR_0_CCW;
				MOTOR_0_DUTY = Max_duty;
			}
			else {
				MOTOR_0_CCW;
				MOTOR_0_DUTY = real_duty;
			}
		}
		break;

		// for Right
	case RW:
		if (duty > 0) {
			real_duty = duty*Ratio_duty;

			if (real_duty >= Max_duty) {
				MOTOR_1_CW;
				MOTOR_1_DUTY = Max_duty;
			}
			else {
				MOTOR_1_CW;
				MOTOR_1_DUTY = real_duty;
			}
		}
		else if (duty == 0) {
			MOTOR_1_DUTY = Max_duty;  // 0:ストップ（ゆっくり止まる）, Max_duty:ブレーキ（急に止まる）
			MOTOR_1_BREAK;
		}
		else {
			real_duty = -duty*Ratio_duty;

			if (real_duty >= Max_duty) {
				MOTOR_1_CCW;
				MOTOR_1_DUTY = Max_duty;
			}
			else {
				MOTOR_1_CCW;
				MOTOR_1_DUTY = real_duty;
			}
		}
		break;

	default: break;
	}
}


// ------------------------------
// --- エンコーダ値の読込関数 ---
// ------------------------------
static int
enco(int no)
{
	static int  r0_present_d, r0_last_d, r0_present_delta = 0;
	static int  r1_present_d, r1_last_d, r1_present_delta = 0;

	if (no == LW) {
		Ct.enco[no].last.d = Ct.enco[no].present.d;

		Ct.enco[no].present.d = -HEX_ITU2.TCNT;  // 4逓倍

		if (Ct.enco[no].present.d > 32767) {
			Ct.enco[no].present.d -= 65536;
		}

		Ct.enco[no].present.delta = Ct.enco[no].present.d - Ct.enco[no].last.d;

		if (Ct.enco[no].present.delta < -30000) {
			Ct.enco[no].present.delta += 65536;
		}
		else if (Ct.enco[no].present.delta > 30000) {
			Ct.enco[no].present.delta -= 65536;
		}

		Ct.tmp_enco_val[no] += Ct.enco[no].present.delta;
	}

	else if (no == RW) {

		//Ct.enco[no].present.d= (HEX_ITU0.TCNT - HEX_ITU1.TCNT)*2;  // 2逓倍      
		// -------------
		// --- r0 用 ---
		// -------------
		r0_last_d = r0_present_d;

		r0_present_d = HEX_ITU0.TCNT;

		if (r0_present_d > 32767) {
			r0_present_d -= 65536;
		}

		r0_present_delta = r0_present_d - r0_last_d;

		if (r0_present_delta < -30000) {
			r0_present_delta += 65536;
		}
		else if (r0_present_delta > 30000) {
			r0_present_delta -= 65536;
		}

		// -------------
		// --- r1 用 ---
		// -------------
		r1_last_d = r1_present_d;

		r1_present_d = HEX_ITU1.TCNT;

		if (r1_present_d > 32767) {
			r1_present_d -= 65536;
		}

		r1_present_delta = r1_present_d - r1_last_d;

		if (r1_present_delta < -30000) {
			r1_present_delta += 65536;
		}
		else if (r1_present_delta > 30000) {
			r1_present_delta -= 65536;
		}

		Ct.tmp_enco_val[no] += (r0_present_delta - r1_present_delta) * 2;
	}

	return(Ct.tmp_enco_val[no]);
}


// -------------------------- //
// --- データセーブ用関数 --- //
// -------------------------- //
// ・tftp - 192.168.1.1 data*.txt (*は，file_number) で，
// 　PCのプログラムのあるフォルダにファイルが転送される．
// ・dir で，H8の中のファイルが見える．
// ・type data*.txt で，data*.txtの内容を参照できる．
static void
save_data(int file_number)
{
	static int   fd;
	static char  buf[256];  // static をつけないと，サイズが大きいときにコンパイルエラーが起きる
	int  i;
	char filename[20];

	sprintf(filename, "data%d.txt", file_number);
	fd = open(filename, OptWrite);

	if (fd == -1) {
		printf(" File open error! \n");
		exit(-1);
	}

	sprintf(buf, "time[ms]  LW_traj[mm]  LW[mm]  RW_traj[mm]  RW[mm] \r\n");
	write(fd, buf, strlen(buf));

	for (i = 0; i <= Ct.dTime.stopVal; i++) {
		sprintf(buf, "%d  %d  %d  %d  %d \r\n",
			i*SampTime,
			(int)(1000 * (Ki.traj[LW][i].d)*Ki.r),
			(int)(1000 * (Ct.data[LW][i].d)*Ki.r),
			(int)(1000 * (Ki.traj[RW][i].d)*Ki.r),
			(int)(1000 * (Ct.data[RW][i].d)*Ki.r)
		);
		write(fd, buf, strlen(buf));
	}

	close(fd);
}
