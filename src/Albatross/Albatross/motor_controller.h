#ifndef MOTOR_CONTROLER_H
#define MOTOR_CONTROLER_H

#include "main.h"

void setMortorDuty(int leftDuty, int rightDuty);
int getEncorder(enum Motor motor);

#endif
