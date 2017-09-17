// Do not remove the include below
#include "ToulouseRobotRace_2017.h"

#include "SoftwareSerial.h"
#include "params.h"
#include "Wire.h"
#include "lib_us.h"
#include "commands.h"

extern "C" {
#include "median_filter.h"
}

SoftwareSerial serial(SRX, STX);
unsigned long start_sonar_time, led_blink_time, asserv_time, joker_time;
int front_distance, left_distance, right_distance, up_distance;
int prev_error;
int led_state;
median_t front_filter;

void setup() {
    pinMode(LED,OUTPUT);
    pinMode(MOT_DIR,OUTPUT);
    pinMode(MOT_PWM,OUTPUT);
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
    serial.begin(115200);
    Wire.begin();
    led_state = 0;
    mf_init(&front_filter, 4, 50);

    startRange(SONAR_FRONT);
    startRange(SONAR_LEFT);
    startRange(SONAR_RIGHT);
    startRange(SONAR_UP);
    start_sonar_time = led_blink_time = asserv_time = millis();
    delay(80);

    serial.println("hello !");
}

void loop() {
	if (millis() - start_sonar_time >= SONAR_PERIOD){
		front_distance = (int) getRangeResult(SONAR_FRONT);
		left_distance = (int) getRangeResult(SONAR_LEFT);
		right_distance = (int) getRangeResult(SONAR_RIGHT);
		up_distance = (int) getRangeResult(SONAR_UP);

		if(front_distance == 0) {
			front_distance = 250;
		}
		if(left_distance == 0) {
			left_distance = 250;
		}
		if(right_distance == 0) {
			right_distance = 250;
		}
		if(up_distance == 0) {
			up_distance = 250;
		}

		mf_update(&front_filter, front_distance);

		startRange(SONAR_FRONT);
		startRange(SONAR_LEFT);
		startRange(SONAR_RIGHT);
		startRange(SONAR_UP);

		start_sonar_time = millis();

		serial.print(left_distance);
		serial.print("\t");
		serial.print(front_distance);
		serial.print("\t");
		serial.print(right_distance);
		serial.print("\t");
		serial.println(up_distance);
	}

	if(!joker_time && millis() - asserv_time > ASSERV_PERIOD)
	{
		int turn_amp = 0;
		int speed = 0;

		int error = left_distance - right_distance;
		int front_dist = mf_get(&front_filter);

		speed = front_dist - 20;
		turn_amp = KP * error - KD * (prev_error - error);

		prev_error = error;

		//joker turn
		if(front_dist < 40){
			setSpeed(-80);
			turn(-turn_amp*200);
			joker_time = millis();
			return;
		}

		//apply some limits and threshold
		if(abs(turn_amp) < 50) {
			turn_amp = 0;
		}
		speed = min(speed, 100);

		setSpeed(speed);
		turn(turn_amp);
	}

	if(joker_time && millis() - joker_time > JOKER_TIME)
	{
		joker_time = 0;
	}


	if(millis() - led_blink_time > BLINK_PERIOD) {
		led_state ^= 1;
		digitalWrite(LED, led_state);
		led_blink_time = millis();
	}
}
