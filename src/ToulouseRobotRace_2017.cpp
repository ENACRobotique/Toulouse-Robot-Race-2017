// Do not remove the include below
#include "ToulouseRobotRace_2017.h"

#include "SoftwareSerial.h"
#include "params.h"
#include "Wire.h"
#include "lib_us.h"
#include "commands.h"

SoftwareSerial serial(SRX, STX);
unsigned long start_sonar_time, led_blink_time;
uint16_t front_distance, left_distance, right_distance, up_distance, prev_front_distance;
int led_state;

void setup() {
    pinMode(LED,OUTPUT);
    pinMode(MOT_DIR,OUTPUT);
    pinMode(MOT_PWM,OUTPUT);
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
    serial.begin(115200);
    Wire.begin();
    led_state = 0;

    start_sonar_time = millis();
    startRange(SONAR_FRONT);
    startRange(SONAR_LEFT);
    startRange(SONAR_RIGHT);
    startRange(SONAR_UP);
    delay(80);

    serial.println("hello !");
}

void loop() {

	if (millis() - start_sonar_time >= 300){
		front_distance = getRangeResult(SONAR_FRONT);
		left_distance = getRangeResult(SONAR_LEFT);
		right_distance = getRangeResult(SONAR_RIGHT);
		up_distance = getRangeResult(SONAR_UP);

		start_sonar_time = millis();

		startRange(SONAR_FRONT);
		startRange(SONAR_LEFT);
		startRange(SONAR_RIGHT);
		startRange(SONAR_UP);

		serial.print(left_distance);
		serial.print("\t");
		serial.print(front_distance);
		serial.print("\t");
		serial.print(right_distance);
		serial.print("\t");
		serial.println(up_distance);
	}


	int turn_amp = 0;

	int speed = 0;

	if(left_distance < 200 && right_distance < 200) {
		turn_amp = (left_distance - right_distance)*2;
		if(turn_amp < 80) {
			turn_amp = 0;
		}
	}
	else if(left_distance < 200 && right_distance > 200) {
		turn_amp = -120;
	}
	else if(left_distance > 200 && right_distance < 200) {
		turn_amp = 120;
	}

	if(prev_front_distance - front_distance > 20) {
		speed = -100;
	}
	else {
		speed = min( (int) front_distance - 50, 100);
	}
	prev_front_distance = front_distance;



	setSpeed(speed);
	turn(turn_amp);


	if(millis() - led_blink_time > 500) {
		led_state ^= 1;
		digitalWrite(LED, led_state);
		led_blink_time = millis();
	}
}
