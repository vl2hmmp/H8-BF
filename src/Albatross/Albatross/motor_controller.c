/*
* @file motor_controler.c
* @brief 駆動系の制御を行います。
* @date 2017/08/21
* @author 桐林　颯
*/

#include "motor_controller.h"
#include "utils.h"

int FeedMotorFlag = 0;

ki_sharedType  Ki;
ct_sharedType  Ct;

void initializeMotor()
{
	// --- 初期時間をセット ---
	Ct.dTime.startVal = 0;

	Ct.jnt[LeftMotor].present.d = 0;
	Ct.jnt[RightMotor].present.d = 0;

	// --- ハードウェアパラメータをセット ---
	// ホイールが一回転回るとき (Ki.r)*2*PI [cm] <-> (Ct.Tr)*(128*4) [pulse] 
	// 0.0013725 [cm/pulse]
	//Ki.W= 0.2f;    // ホイール間距離 0.199   [m]
	Ki.W = 0.171f * 1.00;    // ホイール間距離 0.199   [m]
							 //Ki.r= 0.022f;  // ホイール半径   0.02125 [m]
	Ki.r = 0.0215f * 1.00;  // 計測値　0.02139 [m]
	Ct.WPulse2Rad = 2.0f*PI / 512.0f;  // エンコーダのパルスを回転角[rad]に変換, 512=128[pulse]*4[逓倍]
	Ct.Tr = 29.0f;    // モータ減速比  

					  // --- ロボットのゲインをセット ---
	Ct.jnt[LeftMotor].gain.kp = 80.0f;
	Ct.jnt[LeftMotor].gain.kd = 80.0f * RsampTime;

	Ct.jnt[RightMotor].gain.kp = 80.0f;
	Ct.jnt[RightMotor].gain.kd = 80.0f * RsampTime;

	// ----------------------
	// --- encoder の設定 ---
	// ----------------------
	// 回転方向をセット
	Ct.enco_dir[LeftMotor] = 1;
	Ct.enco_dir[RightMotor] = 1;

	// 変数の初期化
	Ct.enco[LeftMotor].present.d = 0;
	Ct.tmp_enco_val[LeftMotor] = 0;
	Ct.start_present_d[LeftMotor] = 0;
	Ct.enco[RightMotor].present.d = 0;
	Ct.tmp_enco_val[RightMotor] = 0;
	Ct.start_present_d[RightMotor] = 0;

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

void feedMotor()
{
	if (FeedMotorFlag == 0)
		return;

	// --- 車輪の回転角を計測 ---
	Ct.jnt[LeftMotor].last.d = Ct.jnt[LeftMotor].present.d;
	Ct.jnt[LeftMotor].present.d = (float)Ct.enco_dir[LeftMotor] * getEncorder(LeftMotor) * Ct.WPulse2Rad / Ct.Tr;
	Ct.jnt[LeftMotor].present.v = (Ct.jnt[LeftMotor].present.d - Ct.jnt[LeftMotor].last.d) / RsampTime;
	Ct.jnt[RightMotor].last.d = Ct.jnt[RightMotor].present.d;
	Ct.jnt[RightMotor].present.d = (float)Ct.enco_dir[RightMotor] * getEncorder(RightMotor) * Ct.WPulse2Rad / Ct.Tr;
	Ct.jnt[RightMotor].present.v = (Ct.jnt[RightMotor].present.d - Ct.jnt[RightMotor].last.d) / RsampTime;

	// --- 車輪の制御 ---
	Ct.jnt[LeftMotor].delta = Ct.start_present_d[LeftMotor] + Ki.traj[LeftMotor][Ct.dTime.val].d - Ct.jnt[LeftMotor].present.d;
	Ct.jnt[RightMotor].delta = Ct.start_present_d[RightMotor] + Ki.traj[RightMotor][Ct.dTime.val].d - Ct.jnt[RightMotor].present.d;

	// --- 出力電圧を計算 ---
	Ct.pwm[LeftMotor] = (int)(Ct.jnt[LeftMotor].gain.kp * Ct.jnt[LeftMotor].delta - Ct.jnt[LeftMotor].gain.kd * Ct.jnt[LeftMotor].present.v);
	Ct.pwm[RightMotor] = (int)(Ct.jnt[RightMotor].gain.kp * Ct.jnt[RightMotor].delta - Ct.jnt[RightMotor].gain.kd * Ct.jnt[RightMotor].present.v);

	setMortorDuty(Ct.pwm[LeftMotor], Ct.pwm[RightMotor]);

	// --- データの保存 ---
	Ct.data[LeftMotor][Ct.dTime.val].d = Ct.jnt[LeftMotor].present.d - Ct.start_present_d[LeftMotor];
	Ct.data[RightMotor][Ct.dTime.val].d = Ct.jnt[RightMotor].present.d - Ct.start_present_d[RightMotor];

	// --- 時間を進める ---
	Ct.timeFlag = inc_time(&Ct.dTime);
}

/*
* @brief モータにduty比を設定します。
* @param leftDuty 設定する値
* @param rightDuty 設定する値
*/
void setMortorDuty(int leftDuty, int rightDuty)
{
	leftDuty *= -Ratio_duty;
	leftDuty = clamp(leftDuty, -Max_duty, Max_duty);

	MOTOR_0_DUTY = abs(leftDuty);
	if (0 < leftDuty) {
		MOTOR_0_CW;
	}
	else if (leftDuty < 0) {
		MOTOR_0_CCW;
	}
	else {
		MOTOR_0_DUTY = Max_duty;
		MOTOR_0_BREAK;
	}

	rightDuty *= Ratio_duty;
	rightDuty = clamp(rightDuty, -Max_duty, Max_duty);

	MOTOR_1_DUTY = abs(rightDuty);
	if (0 < rightDuty) {
		MOTOR_1_CW;
	}
	else if (rightDuty < 0) {
		MOTOR_1_CCW;
	}
	else {
		MOTOR_1_DUTY = Max_duty;
		MOTOR_1_BREAK;
	}
}


/*
* @brief エンコーダの値を取得します。
* @param motor 取得するモータ
* @return エンコーダの値
*/
int getEncorder(enum Motor motor)
{
	Ct.enco[motor].last.d = Ct.enco[motor].present.d;

	if(motor == LeftMotor){
		Ct.enco[motor].present.d= HEX_ITU2.TCNT;  // 4逓倍
	}
	else if(motor == RightMotor){
		Ct.enco[motor].present.d= (HEX_ITU0.TCNT - HEX_ITU1.TCNT)*2;  // 2逓倍
	}

	if( Ct.enco[motor].present.d > 32767 ){
		Ct.enco[motor].present.d -= 65536;
	}	

	Ct.enco[motor].present.delta= Ct.enco[motor].present.d - Ct.enco[motor].last.d;

	if( Ct.enco[motor].present.delta < -30000 ){
		Ct.enco[motor].present.delta += 65536;
	}
	else if( Ct.enco[motor].present.delta > 30000 ){
		Ct.enco[motor].present.delta -= 65536;
	}

	Ct.tmp_enco_val[motor] += Ct.enco[motor].present.delta;

	return Ct.tmp_enco_val[motor];
}

// --------------------------------------
// --- エンコーダを用いた軌道追従制御 ---
// --------------------------------------
void traj_tracking(float x_f, float tht_f, float t_f) {
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
	FeedMotorFlag = 1;

	while (Ct.timeFlag != Reached) {
printf("enco : %d, %d\r\n", (int)(getEncorder(LeftMotor)), (int)(getEncorder(RightMotor)));
printf("duty : %d, %d\r\n", (int)(Ct.pwm[LeftMotor]), (int)(Ct.pwm[RightMotor]));
	}

	STOP_ENCO_COUNT;
	FeedMotorFlag = 0;

	reset_time(&Ct.dTime);

	// 現在位置を取得
	Ct.start_present_d[LeftMotor] = Ct.jnt[LeftMotor].present.d;
	Ct.start_present_d[RightMotor] = Ct.jnt[RightMotor].present.d;

	setMortorDuty(0, 0);
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

		j = (float)(i - imti) * RsampTime;

		pos_present = a0 + a2 * j*j + a3 * j*j*j;

		Ki.in[jnt][i] = (pos_present - pos_last) / RsampTime;
		if (i == imti || i == imtf)
			Ki.in[jnt][i] = 0.0f;
	}
}


// ----------------------------
// --- 目標の車輪軌道を生成 ---
// ----------------------------
static void
make_traj(float t_i, float t_f)
{
	int  i;
	int  imti, imtf;

	imti = (int)(t_i / RsampTime);
	imtf = (int)(t_f / RsampTime);

	for (i = imti; i <= imtf; i++) {

		// --- 各車輪の目標角速度を求める ---
		Ki.traj[LeftMotor][i].v = (Ki.in[DDis][i] - Ki.W * Ki.in[DTht][i] / 2.0f) / (Ki.r);
		Ki.traj[RightMotor][i].v = (Ki.in[DDis][i] + Ki.W * Ki.in[DTht][i] / 2.0f) / (Ki.r);
		Ki.traj[LeftMotor][i].d = 0;
		Ki.traj[RightMotor][i].d = 0;
		if (i >= 1) {
			Ki.traj[LeftMotor][i].d = Ki.traj[LeftMotor][i - 1].d + Ki.traj[LeftMotor][i].v * RsampTime;
			Ki.traj[RightMotor][i].d = Ki.traj[RightMotor][i - 1].d + Ki.traj[RightMotor][i].v * RsampTime;
		}
	}
}

void start_time(ct_timeType *p)
{
	p->val = p->startVal;
	p->inc = UntilInc;
}


void reset_time(ct_timeType *p)
{
	p->val = p->startVal;
	p->inc = Stopped;
	Ct.timeFlag = Stopped;
}


int inc_time(ct_timeType *p)
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
