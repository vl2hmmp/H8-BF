/*
* @file parking.c
* @brief 車庫入れの制御を行います。
* @date 2017/12/22
* @author 桐林　颯
*/

#include "parking.h"
#include "motor_controller.h"

int FeedParkingFlag = 0;

void feedParking()
{
	if (FeedParkingFlag == 0)
		return;
}

void parking()
{
	FeedParkingFlag = 1;

	printf("parking start");
	traj_tracking(33, 0, 4);
	traj_tracking(5, 90, 4);
	traj_tracking(60, 0, 4);
	traj_tracking(-26, -90, 4);
	traj_tracking(-7, 0, 4);
	printf("parking end");

	FeedParkingFlag = 0;
}
