/*
* @file line_trace.c
* @brief �Ԍɓ���̐�����s���܂��B
* @date 2017/12/22
* @author �˗с@�D
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
	// �����ɒ��ԗp��traj�Ȃ�Ƃ����g���ĎԌɓ���菇����������
	// traj�Ȃ�Ƃ���motor_controller.c�̒��ɒ�`�ς�
	FeedParkingFlag = 0;
}
