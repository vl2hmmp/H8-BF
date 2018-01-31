/*
* @file line_trace.c
* @brief ライントレースの制御を行います。
* @date 2017/12/22
* @author 桐林　颯
*/

#include "line_trace.h"
#include "motor_controller.h"

int FeedLineTraceFlag = 0;

void feedLineTrace()
{
	if (FeedLineTraceFlag == 0)
		return;

	STOP_ENCO_COUNT;
}

/*
* @brief ライントレースを制御します。
*/
void trace()
{
	FeedLineTraceFlag = 1;

	int lap = 0, cooldown = 0, courseout = 0;

	while (1) {
		if (getPhotos() == 1001)
		{
			setMortorDuty(100, 100);
		}
		if (getPhotos() == 1101)
		{
			setMortorDuty(100, 100);
		}
		if (getPhotos() == 1011)
		{
			setMortorDuty(100, 100);
		}
		if (getPhotos() == 1110)
		{
			setMortorDuty(100, 8);
		}
		if (getPhotos() == 1100)
		{
			setMortorDuty(100, 12);
		}
		if (getPhotos() == 1000)
		{
			setMortorDuty(100, 15);
		}
		if (getPhotos() == 111)
		{
			setMortorDuty(8, 100);
		}
		if (getPhotos() == 11)
		{
			setMortorDuty(12, 100);
		}
		if (getPhotos() == 1)
		{
			setMortorDuty(15, 100);
		}
		if (getPhotos() == 0)
		{
			setMortorDuty(100, 100);
			if (cooldown <= 0){
				lap++;
				cooldown = 100;
				if (3 <=lap)
					break;
			}
		}
		if (getPhotos() == 1111)
		{
			courseout++;
			double throttle = 0.08 * courseout;
			throttle = 10 < throttle ? 10 : throttle;
			setMortorDuty((int)throttle, 100);
			//setMortorDuty(1, 20);
		}
		else
		{
			courseout = 0;
		}

		if (0 < cooldown)
			cooldown--;
	}

	setMortorDuty(0, 0);

	FeedLineTraceFlag = 0;
}

int getPhotos()
{
	return 1000 * Photo_1 + 100 * Photo_2 + 10 * Photo_3 + 1 * Photo_4;
}
