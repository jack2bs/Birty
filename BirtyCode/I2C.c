/*
 * I2C.c
 *
 * Created: 5/26/2022 4:28:11 PM
 *  Author: Jack2bs
 */ 

// Other includes
#include <avr/interrupt.h>
#include <stdint.h>

// Custom includes
#include "Sens.h"
#include "I2C.h"
#include "I2CInstruction.h"
#include "VirtualSerial.h"

// I2C event interrupt
ISR(TWI_vect)
{
	I2CHandle();
}

/*
The following section contains the API for the g_sensors object
*/

// This array contains a SensorData struct for each sensor on the robot
struct SensData g_sensors[NUM_OF_DIST_SENSORS + NUM_OF_OTHER_SENSORS + 1] = {{0}};

// Returns a pointer to the lastData array for the sensors
uint8_t * getSensorDataPtr(int index)
{
	return g_sensors[index].lastData;
}

// Returns the value of lastData received by the sensor combined into an int
int getSensorData(int index)
{
	return (((int)g_sensors[index].lastData[0])) | (((int)g_sensors[index].lastData[1]) << 8);								// Combines the two lastData bytes together
}

// Returns a pointer to the shouldRead variable for the sensors
uint8_t * getSensorShouldReadPtr(int index)
{
	return &g_sensors[index].shouldRead;
}

// Returns whether the sensor should be read from
int getSensorShouldRead(int index)
{
	return g_sensors[index].shouldRead;
}

// Returns the device address for the sensor
int getSensorDeviceAddress(int index)
{
	return g_sensors[index].DevAddress;
}

// Sets the sensor data using an array
void setSensorDataArr(int index, uint8_t * data)
{
	g_sensors[index].lastData[0] = *data;
	g_sensors[index].lastData[1] = *(data + 1);
}

// Sets the sensor data using an int variable
void setSensorData(int index, int data)
{
	g_sensors[index].lastData[0] = (uint8_t) data;
	g_sensors[index].lastData[1] = data >> 8;
}

// Set the shouldRead variable
void setSensorShouldRead(int index, int data)
{
	g_sensors[index].shouldRead = data;
}

// Clear the shouldRead variable
void clearSensorShouldRead(int index)
{
	g_sensors[index].shouldRead = 0;
}

// Sets the sensor's device address
void setSensorDeviceAddress(int index, int address)
{
	g_sensors[index].DevAddress = address;
}

/* End of g_sensors API*/

// Sends a start condition to the I2C bus
inline void sendStartCond()
{
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ACK_EN) | (1 << TWI_START) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Sends a stop condition to the I2C bus
inline void sendStopCond()
{
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ACK_EN) | (1 << TWI_STOP) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Enables ACK
inline void enableACK()
{
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ACK_EN) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Disables ACK
inline void disableAck()
{
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Load data into TWDR
inline void loadTWDR(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}

// Read is high on SDA, Write is low on SDA
// Loads the slave address + r/w onto the I2C bus
inline void loadAdress(uint8_t address, uint8_t r_w)
{
	loadTWDR((address << 1) | r_w);
}

// Read is high on SDA
// Loads the slave address + r onto the I2C bus
inline void loadAddressRead(uint8_t address)
{
	loadTWDR((address << 1) | 1);
}

// Write is low on SDA
// Loads the slave address + w onto the I2C bus
inline void loadAddressWrite(uint8_t address)
{
	loadTWDR(address << 1);
}

// This is high when the I2C bus is active and low when its not
static uint8_t g_state = 0;

// This handles I2C using info from the I2C-Instructions
void I2CHandle()
{	
	struct I2CInstruction * curInst;			// Holds the current instruction
	curInst = getCurrentInstruction();			// ""		""		""		""
	int curInd = curInst->dev_index;			// Holds the index of the device in the current instruction
	
	static int dataPtr = 0;						// Holds how many bytes have been written/read

	// Switch for the value of the I2C status Reg
	switch(TWSR & 0b11111000)
	{
		// Start or repeated start
		case START_TRA:
		case REP_START_TRA:
			loadAdress(getSensorDeviceAddress(curInd), curInst->readWrite);	// Load the device address and r/w
			break;
			
		// Slave address + write has been transmitted and ACK received
		case SLA_W_TRA_ACK_REC:
			loadTWDR(*(curInst->data));			// Load the first byte to write into TWDR
			dataPtr = 1;						// Update  dataPtr
			break;
			
		// Slave address + write has been transmitted and NACK received
		case SLA_W_TRA_NACK_REC:
			// Could put an error message here
			sendStopCond();						// Send a stop condition
			moveToNextInstruction();			// Move to the next instruction (could comment out)
			g_state = 0;						// set g_state to 0 (I2C ready/off)
			return;
		
		// A data byte has been transmitted and an ACK received
		case DATA_TRA_ACK_REC:
			// If all of the bytes have been transmitted
			if(dataPtr == curInst->length)
			{
				sendStopCond();					// Send a stop condition
				moveToNextInstruction();		// Move to the next instruction (could comment out)
				g_state = 0;					// set g_state to 0 (I2C ready/off)
				dataPtr = 0;					// Reset the dataPtr var
				return;
			}
			// Otherwise
			else
			{	
				loadTWDR(*((curInst->data) + dataPtr));	// Load the next byte to write into TWDR
				dataPtr++;								// Increment the dataPtr
			}
			break;
			
		// A data byte has been transmitted and a NACK received
		case DATA_TRA_NACK_REC:
			sendStopCond();					// Send a stop condition
			moveToNextInstruction();		// Move to the next instruction (could comment out)
			g_state = 0;					// set g_state to 0 (I2C ready/off)
			return;
			
		// Slave address + read transmitted and an ACK received
		case SLA_R_TRA_ACK_REC:
			// If only 1 byte is going to be read
			if(dataPtr == curInst->length - 1)
			{
				disableAck();				// Disable the ACK
			}
			// Otherwise
			else
			{
				enableACK();				// Enable the ACk
			}
			break;
		
		// Slave address + read transmitted and a NACK received
		case SLA_R_TRA_NACK_REC:
			sendStopCond();					// Send a stop condition
			moveToNextInstruction();		// Move to the next instruction (could comment out)
			g_state = 0;					// set g_state to 0 (I2C ready/off)
			return;
			
		// Data received and ACK transmitted
		case DATA_REC_ACK_TRA:
			curInst->data[dataPtr] = TWDR;		// Read in the byte
			dataPtr++;							// Increment dataPtr
			// If we've read as much as we want
			if(dataPtr == curInst->length - 1)
			{
				disableAck();					// Disable the ACK
			}
			else								// Otherwise
			{
				enableACK();					// Enable the ACK
			}
			break;
		
		// Data received and NACK transmitted
		case DATA_REC_NACK_TRA:
			curInst->data[dataPtr] = TWDR;	// Read in the byte
			dataPtr = 0;					// Reset the dataPtr var
			sendStopCond();					// Send a stop condition
			moveToNextInstruction();		// Move to the next instruction (could comment out)
			g_state = 0;					// set g_state to 0 (I2C ready/off)
			return;
			
		// If one of the other statuses pops up
		default:
			sendStopCond();					// Send a stop condition
			moveToNextInstruction();		// Move to the next instruction (could comment out)
			g_state = 0;					// set g_state to 0 (I2C ready/off)
			return;
	}
	// If we haven't returned, then make sure g_state is 1
	g_state = 1;
	vsTask();
}

// Called every loop to determine when to start I2C transaction
void I2CTask()
{
	// If g_state is low and there is an instruction available
	if(!g_state && !I2CInstrGetEmpty())
	{
		// Send a start condition and update g_state
		sendStartCond();
		g_state = 1;
	}	
}

// Initialize the I2C
void I2CInit()
{
	/*
	
	 (16000000HZ) / (16 + 2x * 4^TWPS) = 400000
	 
	 so
	 
	 (16 + 2x * 4 ^TWPS) = 40
	 
	 so
	 
	 2x * 4^TWPS = 24
	 x = 3
	 TWPS = 1
	 
	 */
	
	// Set TWBR to 3 (calculation shown above)
	TWBR = 6;
	
	// Initial TWCR settings
	TWCR = (1 << TWI_INT_FLAG) | (1 << TWI_ENABLE) | (1 << TWI_INT_EN);
}