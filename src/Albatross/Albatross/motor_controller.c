/*
* @file motor_controler.c
* @brief 駆動系の制御を行います。
* @date 2017/08/21
* @author 桐林　颯
*/

#include "motor_controller.h"
#include "utils.h"

ct_sharedType  Ct;

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

