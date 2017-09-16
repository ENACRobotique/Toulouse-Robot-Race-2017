/*
 * commands.cpp
 *
 *  Created on: 9 août 2017
 *      Author: fabien
 */
#include "Arduino.h"
#include "params.h"

/**
 * \param magnetude : the pwm value set to the ENABLE pin of the L293D
 * Set the direction pins according to the magnetude's sign, then set pwm value.
 */
void turn(int magnetude) {
	if(magnetude == 0)
	{
		digitalWrite(DIR_A, 0);
		digitalWrite(DIR_B, 0);
		analogWrite(DIR_ENABLE, 0);
	}
	else if(magnetude > 0)
	{
		digitalWrite(DIR_B, LOW);		//never set DIR_A and DIR_B both HIGH
		digitalWrite(DIR_A, HIGH);
		analogWrite(DIR_ENABLE, min(magnetude, 255));
	}
	else if(magnetude < 0)
	{
		digitalWrite(DIR_A, LOW);
		digitalWrite(DIR_B, HIGH);
		analogWrite(DIR_ENABLE, min(-magnetude, 255));
	}
}


/**
 * Set main motor speed and direction. Must be used with a MD10C.
 */
void setSpeed(int speed) {

	int dir = HIGH;
	if(speed < 0) {
		dir = LOW;
	}
	digitalWrite(MOT_DIR, dir);

	analogWrite(MOT_PWM, min(abs(speed), MAX_SPEED));
}
