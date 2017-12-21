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
	Ki.W = 0.204f * 1.02;    // ホイール間距離 0.199   [m]
							 //Ki.r= 0.022f;  // ホイール半径   0.02125 [m]
	Ki.r = 0.02139f * 0.99;  // 計測値　0.02139 [m]
	Ct.WPulse2Rad = 2.0f*PI / 512.0f;  // エンコーダのパルスを回転角[rad]に変換, 512=128[pulse]*4[逓倍]
	Ct.Tr = 29.0f;    // モータ減速比  

					  // --- ロボットのゲインをセット ---
	Ct.jnt[LeftMotor].gain.kp = 80.0f;
	Ct.jnt[LeftMotor].gain.kd = 80.0f * RsampTime;

	Ct.jnt[RightMotor].gain.kp = 80.0f;
	Ct.jnt[RightMotor].gain.kd = 80.0f * RsampTime;

	// -------------------
	// --- motorの設定 ---
	// -------------------
	// 回転方向をセット
	Ct.mot_dir[LeftMotor] = -1;
	Ct.mot_dir[RightMotor] = 1;

	// ----------------------
	// --- encoder の設定 ---
	// ----------------------
	// 回転方向をセット
	Ct.enco_dir[LeftMotor] = -1;
	Ct.enco_dir[RightMotor] = 1;

	// 変数の初期化
	for (int jnt = LeftMotor; jnt <= RightMotor; jnt++) {
		Ct.enco[jnt].present.d = 0;
		Ct.tmp_enco_val[jnt] = 0;
		Ct.start_present_d[jnt] = 0;
	}
}

void feedMotor()
{
	if (FeedMotorFlag == 0)
		return;

	for (int jnt = LeftMotor; jnt <= RightMotor; jnt++) {

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
	int  r0_present_d, r0_last_d, r0_present_delta = 0;
	int  r1_present_d, r1_last_d, r1_present_delta = 0;

	switch (motor) {
		case LeftMotor:
			Ct.enco[motor].last.d = Ct.enco[motor].present.d;

			Ct.enco[motor].present.d = -HEX_ITU2.TCNT;  // 4逓倍

			if (Ct.enco[motor].present.d > 32767) {
				Ct.enco[motor].present.d -= 65536;
			}

			Ct.enco[motor].present.delta = Ct.enco[motor].present.d - Ct.enco[motor].last.d;

			if (Ct.enco[motor].present.delta < -30000) {
				Ct.enco[motor].present.delta += 65536;
			}
			else if (Ct.enco[motor].present.delta > 30000) {
				Ct.enco[motor].present.delta -= 65536;
			}

			Ct.tmp_enco_val[motor] += Ct.enco[motor].present.delta;
			break;
		case RightMotor:
			Ct.enco[motor].present.d= (HEX_ITU0.TCNT - HEX_ITU1.TCNT)*2;  // 2逓倍      
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

			Ct.tmp_enco_val[motor] += (r0_present_delta - r1_present_delta) * 2;
			break;
	}

	return Ct.tmp_enco_val[motor];
}

// --------------------------------------
// --- エンコーダを用いた軌道追従制御 ---
// --------------------------------------
static void
traj_tracking(float x_f, float tht_f, float t_f) {
	FeedMotorFlag = 1;
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
	}

	reset_time(&Ct.dTime);

	// 現在位置を取得
	Ct.start_present_d[LeftMotor] = Ct.jnt[LeftMotor].present.d;
	Ct.start_present_d[RightMotor] = Ct.jnt[RightMotor].present.d;

	FeedMotorFlag = 0;
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

		pos_present = a0 + a2 * j*j + a3 * j*j*j;

		if (i == imti || i == imtf) {
			Ki.in[jnt][i] = 0.0f;
		}
		else {
			Ki.in[jnt][i] = (pos_present - pos_last) / RsampTime;
		}
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

		// --- 各車輪の目標角速度を求める ---
		Ki.traj[LeftMotor][i].v = (Ki.in[DDis][i] - (Ki.W)*(Ki.in[DTht][i]) / 2.0f) / (Ki.r);
		Ki.traj[RightMotor][i].v = (Ki.in[DDis][i] + (Ki.W)*(Ki.in[DTht][i]) / 2.0f) / (Ki.r);
		Ki.traj[LeftMotor][i].d = 0;
		Ki.traj[RightMotor][i].d = 0;
		if (i >= 1) {
			Ki.traj[LeftMotor][i].d = (Ki.traj[jnt][i - 1].d) + (Ki.traj[LeftMotor][i].v)*RsampTime;
			Ki.traj[RightMotor][i].d = (Ki.traj[jnt][i - 1].d) + (Ki.traj[RightMotor][i].v)*RsampTime;
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
