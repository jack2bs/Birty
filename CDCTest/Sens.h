/*
 * DistSens.h
 *
 * Created: 5/26/2022 6:35:53 PM
 *  Author: Jack2bs
 */ 


#ifndef DISTSENS_H_
#define DISTSENS_H_

#include <stdint.h>

// Device Types
#define DISTSENS_DEVTYPE 0

// SensData Struct
struct SensData
{
	uint8_t DevAddress;		// Device address var in SensData
	uint8_t lastData[2];	// Last data read from the sensor
	uint8_t shouldRead;		// Should we read from the sensor
};

#endif /* DISTSENS_H_ */