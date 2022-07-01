#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>

#include "VirtualSerial.h"
#include "Descriptors.h"

#include "LUFA/Drivers/USB/USB.h"

#include "Sens.h"
#include "I2C.h"
#include "I2CInstruction.h"
#include "VL6180XDist.h"
#include "ExtInt.h"
#include "DCMotor.h"
#include "Encoder.h"
#include "Stepper.h"
#include "MotorDefs.h"
#include "Millis.h"
#include "Minimu9V5.h"
#include "ShiftReg.h"
#include "PID.h"
#include "Usart.h"

extern FILE USBSerialStream;
int g_diff = 0;				// Debugging

int main(void)
{	
	MCUSR &= ~(1 << WDRF);
	wdt_disable();
	MCUCR |= (1<<JTD);
	clock_prescale_set(clock_div_1);

	I2CInit();
	millisTimerInit();
	
 	dcMotorInit();
	setMotorPWR(BOTH_MOTORS_PASS, 80);
 	encoderInit();
	usartInit();

	sei();
	
	shiftRegInit();
	
	
	_delay_ms(250);
	
	
	setSensorData(NUM_OF_TOTAL_SENSORS, 0);
	setSensorDeviceAddress(NUM_OF_TOTAL_SENSORS, 0x29);
	setSensorShouldRead(NUM_OF_TOTAL_SENSORS, 51);
	for (int i = 0; i < NUM_OF_DIST_SENSORS; i++)
	{
		VL6180XInit(i);	
	}
	minimu9V5Init(NUM_OF_DIST_SENSORS);
	
	
	int lastMillis = 0;
	int nextMove = 0;	//Testing
	while(1)
	{
		I2CTask();
		VL6180XTask();
		minimu9V5Task(NUM_OF_DIST_SENSORS);
		motionTask();
		usartTask();
		
		if(abs(getMillis() - lastMillis) > 200)
		{	
			lastMillis = getMillis();
			if (getMotionState() == NO_MOTION_STATE)
			{
				switch(nextMove)
				{
				case MOVING_STRAIGHT_STATE:
					moveStraight(10);
					nextMove++;
					break;
				case TURNING_LEFT_STATE:
					turnLeft(90);
					nextMove++;
					break;
				case TURNING_RIGHT_STATE:
					turnRight(90);
					nextMove++;
					break;
				case MOVING_BACKWARDS_STATE:
					moveBackwards(500);
					nextMove = 1;
					break;
				default:
					nextMove = 1;
				}
			}
			
			char prt[] = "    ";
			sprintf(prt, "%d\n", getDistSensorDist(FRONT_RIGHT_INDEX));
			addStringToUsartWriteBuffer(prt);
			
		}

		
	}
}