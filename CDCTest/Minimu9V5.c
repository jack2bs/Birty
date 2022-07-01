/*
 * Minimu9V5.c
 *
 * Created: 6/8/2022 9:17:12 PM
 *  Author: Jack2bs
 */ 

// Other includes
#include <avr/io.h>

// Custom includes
#include "I2CInstruction.h"
#include "Sens.h"
#include "I2C.h"
#include "ExtInt.h"
#include "VL6180XDist.h"
#include "Millis.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*

	Heading

*/

#define GYRO_SCALE_FACTOR 133.7

static double g_heading = 0;

double updateHeading(int index, int dt)
{
	if (getSensorData(index) > 750 || getSensorData(index) < -750)
		g_heading += (getSensorData(index) / GYRO_SCALE_FACTOR) * (dt / 1000.0);
	return g_heading;
}

double getHeading()
{
	return g_heading;
}

int getHeadingInt()
{
	return (int)g_heading;
}

void resetHeading()
{
	g_heading = 0;
}

/*

	End Heading

*/

// Turns a 5 digit (Base 10) number to a string
void GYRORawToString(int raw, char* str)
{
	if(strlen(str) < 5)
	{
		return;
	}
	
	sprintf(str, "%d", raw);
}

static uint8_t minimu9V5_gyroXReg[1] = {0x22};
void addReadGyroXInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_gyroXReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_gyroYReg[1] = {0x24};
void addReadGyroYInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_gyroYReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_gyroZReg[1] = {0x26};
void addReadGyroZInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_gyroZReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_accXReg[1] = {0x28};
void addReadAccXInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_accXReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_accYReg[1] = {0x2a};
void addReadAccYInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_accYReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_accZReg[1] = {0x2c};
void addReadAccZInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_accZReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_magXReg[1] = {0x28};
void addReadMagXInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_magXReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_magYReg[1] = {0x2a};
void addReadMagYInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_magYReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_magZReg[1] = {0x2c};
void addReadMagZInstr(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_magZReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t minimu9V5_IMUWHOMIReg[1] = {0x0f};
void addReadIMUwhoAmI(int index)
{
	addInstruction(index, I2C_WRITE, &minimu9V5_IMUWHOMIReg[0], 1);
	addInstruction(index, I2C_READ, getSensorDataPtr(index), 2);
}

static uint8_t LSM6DS33CustomInitData[2][2] =
{
	{0x11, 0b01010000},		// Set gyro to update 204hz
	{0x16, 0b01000000}
};

void IMUinit(int index)
{
	setSensorData(index, 0);
	setSensorDeviceAddress(index, 0x6b); // LSM6DS33 (gyro and accelerometer)
	addReadIMUwhoAmI(index);
	for (int i = 0; i < 2; i++)
	{
		while(!addInstruction(index, 0, &(LSM6DS33CustomInitData[i][0]), 2));
	}
}


static int g_minimu9V5TaskFlag = 0;

void setMinimu9V5TaskFlag()
{
	g_minimu9V5TaskFlag = 1;
}
void clearMinimu9V5TaskFlag()
{
	g_minimu9V5TaskFlag = 0;
}
int getMinimu9V5TaskFlag()
{
	return g_minimu9V5TaskFlag;
}

void minimu9V5Task(int index)
{
	static int lastTime = 0;
	
	int currTime = getMillis();
	if (abs(currTime - lastTime) > 25)
	{
		addReadGyroZInstr(index);
		if (currTime > lastTime)
		{
			updateHeading(index, currTime - lastTime);
		}
		else
		{
			updateHeading(index, getLastOVF() - lastTime + currTime);
		}
		lastTime = currTime;
	}
	
}

void magInit(int index)
{
	setSensorData(index + 1, 0);
	setSensorDeviceAddress(index, 0x1E); // LIS3MDL (Magnetometer)
}

void minimu9V5Init(int index)
{
	IMUinit(index);	
	magInit(index + 1);
}