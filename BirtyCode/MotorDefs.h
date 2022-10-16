/*
 * MotorDefs.h
 *
 * Created: 5/29/2022 9:06:52 PM
 *  Author: Jack2bs
 */ 


#ifndef MOTORDEFS_H_
#define MOTORDEFS_H_

// Motor Direction Defines
#define DIR_BACKWARD	0
#define DIR_FORWARD		1

/* Motor right left defines. USE THESE*/

// Defines for passing motor sides as arguments
#define RIGHT_MOTOR_PASS		0b01
#define LEFT_MOTOR_PASS			0b10
#define BOTH_MOTORS_PASS		0b11

// Defines for choosing motor in arrays
#define RIGHT_MOTOR_INDEX		(RIGHT_MOTOR_PASS - 1)
#define LEFT_MOTOR_INDEX		(LEFT_MOTOR_PASS - 1)

#endif /* MOTORDEFS_H_ */