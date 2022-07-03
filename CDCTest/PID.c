/*
 * PID.c
 *
 * Created: 6/20/2022 8:11:45 PM
 *  Author: Jack2bs
 */ 

#include "Encoder.h"
#include "DCMotor.h"
#include "PID.h"
#include "MotorDefs.h"
#include "VL6180XDist.h"


static int g_motionState = NO_MOTION_STATE;
static const long g_distForTurnInMinus7PerDegree =  19548;

static long g_distThresh = 0;

int getMotionState()
{
	return g_motionState;
}

void stopMoving()
{
	g_motionState = NO_MOTION_STATE;
	setMotorPWR(BOTH_MOTORS_PASS, 0);
	g_distThresh = 0;
}

void startMovingBackwards()
{
	resetDists();
	
	g_motionState = MOVING_BACKWARDS_STATE;
	setMotorDir(BOTH_MOTORS_PASS, DIR_BACKWARD);
	setMotorPWR(BOTH_MOTORS_PASS, 80);
}

void startMovingStraight()
{
	resetDists();
	
	g_motionState = MOVING_STRAIGHT_STATE;
	setMotorDir(BOTH_MOTORS_PASS, DIR_FORWARD);
	setMotorPWR(BOTH_MOTORS_PASS, 80);
}

void startLeftTurn()
{
	
	g_motionState = TURNING_LEFT_STATE;
	
	resetDists();
	
	setMotorDir(LEFT_MOTOR_PASS, DIR_FORWARD);
	
	setRightMotorPWR(80);
	setLeftMotorPWR(0);
}

void startRightTurn()
{
	g_motionState = TURNING_RIGHT_STATE;
	
	resetDists();
	
	setMotorDir(RIGHT_MOTOR_PASS, DIR_FORWARD);
	
	setLeftMotorPWR(80);
	setRightMotorPWR(0);
}

void turnRight(uint16_t degrees)
{
	g_distThresh = g_distForTurnInMinus7PerDegree * degrees;
	startRightTurn();
}
void turnLeft(uint16_t degrees)
{
	g_distThresh = g_distForTurnInMinus7PerDegree * degrees;
	startLeftTurn();
}

void moveStraight(long distCM)
{
	g_distThresh = distCM * 100000;
	
	startMovingStraight();
}

void moveBackwards(long distCM)
{
	g_distThresh = distCM * -100000;
	
	startMovingBackwards();
}

void updateMotionState()
{
	switch (g_motionState)
	{
	case MOVING_STRAIGHT_STATE:
		if (((getRightDistAccurate() + getLeftDistAccurate()) / 2) > g_distThresh)
		{
			stopMoving();
		}
		break;
	case TURNING_LEFT_STATE:
		if ((getRightDistAccurate() - getLeftDistAccurate()) > g_distThresh)
		{
			stopMoving();
		}
		break;
	case TURNING_RIGHT_STATE:
		if ((getLeftDistAccurate() - getRightDistAccurate()) > g_distThresh)
		{
			stopMoving();
		}
		break;
	case MOVING_BACKWARDS_STATE:
		if (((getRightDistAccurate() + getLeftDistAccurate()) / 2) < g_distThresh)
		{
			stopMoving();
		}
		break;
	}
}


void equalizeSpeedsStraight()
{
	static const long equalization_factor = 50000;
	long diff = getLeftDistAccurate() - getRightDistAccurate();
	long refactoredDiff = diff / equalization_factor;
	int rmp = 80;
	int lmp = 80;
	if (g_motionState == MOVING_STRAIGHT_STATE)
	{
		rmp += refactoredDiff;
		lmp -= refactoredDiff;
	}
	if (g_motionState == MOVING_BACKWARDS_STATE)
	{
		rmp -= refactoredDiff;
		lmp += refactoredDiff;
	}
	
	if (rmp > 100)
	{
		setRightMotorPWR(100);
	}
	else if (rmp < 0)
	{
		setRightMotorPWR(0);
	}
	else
	{
		setRightMotorPWR(rmp);
	}
	
	if (lmp > 100)
	{
		setLeftMotorPWR(100);
	}
	else if (lmp < 0)
	{
		setLeftMotorPWR(0);
	}
	else
	{
		setLeftMotorPWR(lmp);
	}
}

void avoidWallsBackward()
{
	static const long equalizationFacor = 1;
	
	int ba_r = getDistSensorDist(BACK_RIGHT_INDEX);
	int ba_l = getDistSensorDist(BACK_LEFT_INDEX);
	
	if (ba_r < 40 || ba_l < 40)
	{
		stopMoving();
		return;
	}
	
	int r_fr = getDistSensorDist(RIGHT_FRONT_INDEX);
	int r_ba = getDistSensorDist(RIGHT_BACK_INDEX);
	int l_fr = getDistSensorDist(LEFT_FRONT_INDEX);
	int l_ba = getDistSensorDist(LEFT_BACK_INDEX);
	
	if (r_ba < 65)
	{
		int diff = (r_fr - r_ba) / equalizationFacor;
		setRightMotorPWR(80 - diff);
		setLeftMotorPWR(80 + diff);
	}
	else if (l_ba < 65)
	{
		int diff = (l_fr - l_ba) / equalizationFacor;
		setRightMotorPWR(80 + diff);
		setLeftMotorPWR(80 - diff);
	}
	else
	{
		equalizeSpeedsStraight();
	}
	
}

void avoidWallsForward()
{
	static const long equalizationFacor = 1;
	
	int fr_r = getDistSensorDist(FRONT_RIGHT_INDEX);
	int fr_l = getDistSensorDist(FRONT_LEFT_INDEX);
	
	if (fr_r < 55 || fr_l < 55)
	{
		stopMoving();
		return;
	}
	
	int r_fr = getDistSensorDist(RIGHT_FRONT_INDEX);
	int r_ba = getDistSensorDist(RIGHT_BACK_INDEX);
	int l_fr = getDistSensorDist(LEFT_FRONT_INDEX);
	int l_ba = getDistSensorDist(LEFT_BACK_INDEX);
	
	if (r_fr < 65)
	{
		int diff = (r_ba - r_fr) / equalizationFacor;
		setRightMotorPWR(80 + diff);
		setLeftMotorPWR(80 - diff);
	}
	else if (l_fr < 65)
	{
		int diff = (l_ba - l_fr) / equalizationFacor;
		setRightMotorPWR(80 - diff);
		setLeftMotorPWR(80 + diff);
	}
	else
	{
		equalizeSpeedsStraight();
	}
	
}

void updateMotors()
{
	switch (g_motionState)
	{
	case NO_MOTION_STATE:
		break;
	case MOVING_STRAIGHT_STATE:
		avoidWallsForward();
		break;
	case MOVING_BACKWARDS_STATE:
		avoidWallsBackward();
		break;
	default:
		break;
	}
}

void motionTask()
{
	if (g_motionState != NO_MOTION_STATE)
	{
		updateMotionState();
	}
	updateMotors();
}