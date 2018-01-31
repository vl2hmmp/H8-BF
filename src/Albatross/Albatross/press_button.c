/*
* @file press_button.c
* @brief ボタン押しの制御を行います。
* @date 2017/12/22
* @author 桐林　颯
*/

#include "press_button.h"
#include "motor_controller.h"

int FeedPressButtonFlag = 0;

void feedPressButton()
{
	if (FeedPressButtonFlag == 0)
		return;
}

void pressButton()
{
	FeedPressButtonFlag = 1;

	traj_tracking(80, 0, 4);
	traj_tracking(0, 90, 4);
	traj_tracking(-30, 0, 4);

	AD.ADCSR.BYTE = 0x18;
	AD.ADCSR.BIT.ADST = 1;
	int input, base;

	while(AD.ADCSR.BIT.ADF == 0);
	base = AD.ADDRA * 2;

	P4.DR.BIT.B7 = 1;
	while(1){
		while(AD.ADCSR.BIT.ADF == 0);
		input = AD.ADDRA / (base / 5);
		if (input == 0)
			setMortorDuty(15, 15);
		if (input == 1)
			setMortorDuty(-15, -15);
		if (input == 2)
			setMortorDuty(0, 0);
		if (input == 3)
			setMortorDuty(-15, 15);
		if (input == 4)
			setMortorDuty(15, -15);
		AD.ADCSR.BIT.ADF = 0;
		//printf("%d\r\n",AD.ADDRA);
	}

	FeedPressButtonFlag = 0;
}
