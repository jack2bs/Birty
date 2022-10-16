/*
 * VL6180XDist.c
 *
 * Created: 5/27/2022 12:25:31 AM
 *  Author: Jack2bs
 */ 

/*

It is very likely that when there are 8 sensors, we will need to switch from
interrupt based to timer based. The switch should be pretty seamless

LMFAO it won't be

Anyway gl with that future Jack

*/

// Other includes
#include <avr/io.h>
#include <util/delay.h>

#include <stdlib.h>

// Custom includes
#include "I2CInstruction.h"
#include "Sens.h"
#include "I2C.h"
#include "ExtInt.h"
#include "VL6180XDist.h"
#include "ShiftReg.h"
#include "VirtualSerial.h"
#include "DCMotor.h"
#include "MotorDefs.h"
#include "Millis.h"

/*
	Sensor Data ADT
*/

static int g_distances[NUM_OF_DIST_SENSORS] = {0};

int getDistSensorDist(int index)
{
	return g_distances[index];
}

/*
	Sensor Data ADT
*/


// These are all private registers that you are required to initialize to these values
static uint8_t VL6180XRequiredInitData[NUM_PRIVATE_REGS][3] =
{
	{0x02, 0x07, 0x01},
	{0x02, 0x08, 0x01},
	{0x00, 0x96, 0x00},
	{0x00, 0x97, 0xfd},
	{0x00, 0xe3, 0x00},
	{0x00, 0xe4, 0x04},
	{0x00, 0xe5, 0x02},
	{0x00, 0xe6, 0x01},
	{0x00, 0xe7, 0x03},
	{0x00, 0xf5, 0x02},
	{0x00, 0xd9, 0x05},
	{0x00, 0xdb, 0xce},
	{0x00, 0xdc, 0x03},
	{0x00, 0xdd, 0xf8},
	{0x00, 0x9f, 0x00},
	{0x00, 0xa3, 0x3c},
	{0x00, 0xb7, 0x00},
	{0x00, 0xbb, 0x3c},
	{0x00, 0xb2, 0x09},
	{0x00, 0xca, 0x09},
	{0x01, 0x98, 0x01},
	{0x01, 0xb0, 0x17},
	{0x01, 0xad, 0x00},
	{0x00, 0xff, 0x05},
	{0x01, 0x00, 0x05},
	{0x01, 0x99, 0x05},
	{0x01, 0xa6, 0x1b},
	{0x01, 0xac, 0x3e},
	{0x01, 0xa7, 0x1f},
	{0x00, 0x30, 0x00}
};

// These are registers that I want to change at initialization
static uint8_t VL6180XCustomInitData[NUM_PUBLIC_REGS][3] =
{
	{0x00, 0x11, 0x10},		// Turns on GPIO1 as interrupting when data is available
	{0x01, 0x0a, 0x30},		// Sets how long it averages range measurements
	{0x00, 0x3f, 0x46},		// Sets the light and dark gain
	{0x00, 0x31, 0x64},		// Sets how often temperature measurements are done (100)
	{0x00, 0x40, 0x63},		// Set ALS integration time to 100ms
	{0x00, 0x2e, 0x01},		// Performs a single temp calibration of the ranging sensor
	{0x00, 0x1b, 0x00},		// Minimizes time between ranging measurements in continuous mode
	{0x00, 0x3e, 0x31},		// Sets the time between ALS measurements to .5 seconds
	{0x00, 0x14, 0x04},		// Configures interrupt on new sample ready threshold event for only ranging sensor
	{0x00, 0x2d, 0x00},		
	{0x00, 0x16, 0x00},	
	//{0x00, 0x10, 0x00},	// Disables XSHUT
	{0x00, 0x18, 0x03}		// Enables and initiates continuous operation
	
	
};

// Turns a 3 digit (Base 10) number to a string
void VL6180XRawToString(uint8_t raw, char* str)
{
	// Math could prob be more efficient
	*(str + 3) = '\0';						// Sets the final byte to the null terminator
	*(str + 2) = ((raw % 100) % 10) + 48;	// Sets the 1's place
	*(str + 1) = (raw % 100 / 10) + 48;		// Sets the 10's place
	*str = (raw / 100) + 48;				// Sets the 100's place
}


// This variable stores the address for an ID register on VL56180x
static uint8_t whoAmIRegAdd[2] = {0x00, 0x00};
static uint8_t addressReg[3] = {0x02, 0x12, 0x29};

// Initializes a VL6180x sensor
void VL6180XInit(int index)
{
	shiftInBit(1);
	_delay_ms(2);
	setSensorData(index, 0);								// Set the sensor lastData to 0
	setSensorDeviceAddress(index, 0x30 + index);
	setSensorShouldRead(index, 0b00000100);	
			
 	addInstruction(NUM_OF_TOTAL_SENSORS, 0, &(whoAmIRegAdd[0]), 2);		// Tell the sensor we want to read an ID reg
 	addInstruction(NUM_OF_TOTAL_SENSORS, 1, getSensorDataPtr(NUM_OF_TOTAL_SENSORS), 1);	// Read the ID reg
 	while (getSensorData(NUM_OF_TOTAL_SENSORS) != 0xb4)					// Wait until the last two instructions finish or get stuck forever
 	{
		I2CTask();							
 	}
	setSensorData(NUM_OF_TOTAL_SENSORS, 0);								// Set the sensor lastData to 0 again (to check for failed initialization)
	
	addressReg[2] = getSensorDeviceAddress(index);
	addInstruction(NUM_OF_TOTAL_SENSORS, 0, &(addressReg[0]), 3);
	addInstruction(index, 0, &(addressReg[0]), 2);
	addInstruction(index, 1, getSensorDataPtr(index), 1);
	while (getSensorData(index) != addressReg[2])
	{
		I2CTask();
	}
	setSensorData(index, 0);
	
	// For each of the private registers
	for(int i = 0; i < NUM_PRIVATE_REGS; i++)
	{
		// Write the correct values
		while(!addInstruction(index, 0, &(VL6180XRequiredInitData[i][0]), 3));
	}
	
	// For each of the public regs
	for(int i = 0; i < NUM_PUBLIC_REGS; i++)
	{
		// Write the correct values
		while(!addInstruction(index, 0, &(VL6180XCustomInitData[i][0]), 3));
	}
}

// These hold the locations for the Ranging result and interrupt clear registers
static uint8_t VL6180XRangeResultLocation[2] = {0x00, 0x62};
static uint8_t VL6180XIntClear[3] = {0x00, 0x15, 0x07};
	
// This adds the instructions for a VL6180X Read
void VL6180XAddReadInstruction(int index)
{
	while(!addInstruction(index, 0, &(VL6180XRangeResultLocation[0]), 2));
	while(!addInstruction(index, 1, getSensorDataPtr(index), 1));
	while(!addInstruction(index, 0, &VL6180XIntClear[0], 3));
}

// This location stores information about the RangeData
static uint8_t VL6180XRangeDataLocation[2] = {0x00, 0x4F};
	
// This checks for range data availability (used to poll after receiving interrupt)
void VL6180XCheckForRangeData(int index)
{
	while(!addInstruction(index, 0, &(VL6180XRangeDataLocation[0]), 2));
	while(!addInstruction(index, 1, getSensorShouldReadPtr(index), 1));
}

// Should be called each time EXTINT sets the VL6180x flag
void VL6180XInterruptTask()
{
	// Checks each dist sensor to see if we should update our range data for it
	for(int i = 0; i < NUM_OF_DIST_SENSORS; i++)
	{
		VL6180XCheckForRangeData(i);
		//setSensorShouldRead(i, 0b00000100);
	}
}

// Handles VL6180x periodic things
void VL6180XTask()
{
	static int lastMillis = 0;
	static int failureCounter = 0;
	
	if (abs(getMillis() - lastMillis) > 25)
	{
		lastMillis = getMillis();
		VL6180XInterruptTask();
	}
	
	// Adds a read instruction for each sensor that should be read
	for(int i = 0; i < NUM_OF_DIST_SENSORS; i++)
	{
		if(getSensorShouldRead(i) & 0b00000100)
		{	
			VL6180XAddReadInstruction(i);
			clearSensorShouldRead(i);
		}
		if (getSensorData(i))
		{
			g_distances[i] = getSensorData(i);
		}
	}
	
	if (getMillis() - failureCounter > 125 || failureCounter - getMillis() > getLastOVF() - 125)
	{
		failureCounter = getMillis();
		VL6180XCheckForFailures();
	}
	
}

// This MUST happen LESS than every 65 ms (right now its 200 :) )
void VL6180XCheckForFailures()
{
	for (int i = 0; i < NUM_OF_DIST_SENSORS; i++)
	{
		if(!(getSensorData(i)))
 		{
// 			fputc('0'+i, &USBSerialStream);
//			setMotorDir(BOTH_MOTORS_PASS, !getMotorDir(RIGHT_MOTOR_PASS));
			shiftRegInit();
			for (int j = 0; j < NUM_OF_DIST_SENSORS; j++)
			{
				PINC |= 0xff;		//Debugging
				VL6180XInit(j);
			}
			break;
		}
	}
	for (int i = 0; i < NUM_OF_DIST_SENSORS; i++)
	{
		setSensorData(i, 0);
	}
}