/*
 * PID.h
 *
 * Created: 6/20/2022 8:12:14 PM
 *  Author: Jack2bs
 */ 


#ifndef PID_H_
#define PID_H_

#include <stdint.h>

#define NO_MOTION_STATE			0
#define MOVING_STRAIGHT_STATE	1
#define TURNING_LEFT_STATE		2
#define TURNING_RIGHT_STATE		3
#define MOVING_BACKWARDS_STATE	4

int getMotionState();
void stopMoving();
void startMovingBackwards();
void startMovingStraight();
void startLeftTurn();
void startRightTurn();
void turnRight(uint16_t degrees);
void turnLeft(uint16_t degrees);
void moveStraight(long distCM);
void moveBackwards(long distCM);
void updateMotionState();
void motionTask();


#endif /* PID_H_ */