/*
* @file line_trace.c
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
	// ここに駐車用のtrajなんとかを使って車庫入れ手順を書くこと
	// trajなんとかはmotor_controller.cの中に定義済み
	FeedParkingFlag = 0;
}
