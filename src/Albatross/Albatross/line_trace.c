/*
* @file line_trace.c
* @brief ���C���g���[�X�̐�����s���܂��B
* @date 2017/12/22
* @author �˗с@�D
*/

#include "line_trace.h"
#include "motor_controller.h"

/*
* @brief ���C���g���[�X�𐧌䂵�܂��B
*/
void trace()
{

	while (1) {
		if (
			Photo_1 == White &&  //��������
			Photo_2 == Black &&
			Photo_3 == Black &&
			Photo_4 == White)
		{
			setMortorDuty(100, 100);
		}
		else if (
			Photo_1 == White &&  //��������
			Photo_2 == White &&
			Photo_3 == Black &&
			Photo_4 == White)
		{
			setMortorDuty(100, 50);
		}
		else if (
			Photo_1 == White &&  //��������
			Photo_2 == Black &&
			Photo_3 == White &&
			Photo_4 == White)
		{
			setMortorDuty(50, 100);
		}
		else if (
			Photo_1 == White &&  //��������
			Photo_2 == White &&
			Photo_3 == White &&
			Photo_4 == Black)
		{
			setMortorDuty(100, 6);
		}
		else if (
			Photo_1 == White &&  //��������
			Photo_2 == White &&
			Photo_3 == Black &&
			Photo_4 == Black)
		{
			setMortorDuty(100, 10);
		}
		else if (
			Photo_1 == White &&  //��������
			Photo_2 == Black &&
			Photo_3 == Black &&
			Photo_4 == Black)
		{
			setMortorDuty(100, 12);
		}
		else if (
			Photo_1 == Black &&  //��������
			Photo_2 == White &&
			Photo_3 == White &&
			Photo_4 == White)
		{
			setMortorDuty(6, 100);
		}
		else if (
			Photo_1 == Black &&  //��������
			Photo_2 == Black &&
			Photo_3 == White &&
			Photo_4 == White)
		{
			setMortorDuty(10, 100);
		}
		else if (
			Photo_1 == Black &&  //��������
			Photo_2 == Black &&
			Photo_3 == Black &&
			Photo_4 == White)
		{
			setMortorDuty(12, 100);
		}
		else if (
			Photo_1 == Black &&  //��������
			Photo_2 == Black &&
			Photo_3 == Black &&
			Photo_4 == Black)
		{
			setMortorDuty(100, 100);
		}
		else if (
			Photo_1 == White &&  //��������
			Photo_2 == White &&
			Photo_3 == White &&
			Photo_4 == White)
		{
			setMortorDuty(1, 20);
		}
	}
}