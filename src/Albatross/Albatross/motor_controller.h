#ifndef MOTOR_CONTROLER_H
#define MOTOR_CONTROLER_H

#include "main.h"

void initializeMotor();
void feedMotor();
void setMortorDuty(int leftDuty, int rightDuty);
int getEncorder(enum Motor motor);

void traj_tracking(float x_f, float tht_f, float t_f);
static void traj_plan_3(int jnt, float x_i, float x_f, float t_i, float t_f);
static void make_traj(float t_i, float t_f);

void start_time(ct_timeType *p);
void reset_time(ct_timeType *p);
int inc_time(ct_timeType *p);

#endif
