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

	while (1) {
		if (getPhotos() == 1001)
		{
			setMortorDuty(100, 100);
		}
		else if (getPhotos() == 1101)
		{
			setMortorDuty(100, 50);
		}
		else if (getPhotos() == 1011)
		{
			setMortorDuty(50, 100);
		}
		else if (getPhotos() == 1110)
		{
			setMortorDuty(100, 6);
		}
		else if (getPhotos() == 1100)
		{
			setMortorDuty(100, 10);
		}
		else if (getPhotos() == 1000)
		{
			setMortorDuty(100, 12);
		}
		else if (getPhotos() == 0111)
		{
			setMortorDuty(6, 100);
		}
		else if (getPhotos() == 0011)
		{
			setMortorDuty(10, 100);
		}
		else if (getPhotos() == 0001)
		{
			setMortorDuty(12, 100);
		}
		else if (getPhotos() == 0000)
		{
			setMortorDuty(100, 100);
		}
		else if (getPhotos() == 1111)
		{
			setMortorDuty(1, 20);
		}
	}

	FeedLineTraceFlag = 0;
}

int getPhotos()
{
	return 1000 * Photo_1 + 100 * Photo_2 + 10 * Photo_3 + 1 * Photo_4;
}
