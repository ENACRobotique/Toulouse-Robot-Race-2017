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

//#define SONAR
#define VIDEO

SoftwareSerial serial(SRX, STX);
unsigned long start_sonar_time, led_blink_time, asserv_time, joker_time, last_command_time;
int front_distance, left_distance, right_distance, up_distance;
int prev_error;
int led_state;
int position_radar;
median_t front_filter;
int turn_amp, speed;
float alpha;
int angle_camera, x_camera;

int s, t, state;

enum {
	AV,
	AV_G,
	AV_D,
	N,
	AR,
	AR_G,
	AR_D
};

void setup() {
    pinMode(LED,OUTPUT);
    pinMode(MOT_DIR,OUTPUT);
    pinMode(MOT_PWM,OUTPUT);
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
    led_state = 0;
    led_blink_time = last_command_time = millis();
    serial.begin(115200);
    alpha = 1;
    start_sonar_time = asserv_time = millis();
    angle_camera = x_camera = 0;
    Wire.begin();
    mf_init(&front_filter, 4, 50);//
    startRange(SONAR_FRONT);
    startRange(SONAR_LEFT);
    startRange(SONAR_RIGHT);
    startRange(SONAR_UP);
    delay(80);


    serial.println("hello !");
    s = t = 0;
    state = N;
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

		position_radar = (100 * (left_distance - right_distance)) / (left_distance + right_distance);
		serial.print("position_radar =");
		serial.println(position_radar);
//		alpha = min(alpha + 0.1, 1);
	}
/*
	if(!joker_time && millis() - asserv_time > ASSERV_PERIOD)
	{
		int turn_amp = 0;
		int speed = 0;

		int error = left_distance - right_distance;
		int front_dist = mf_get(&front_filter);

		speed = front_dist - 20;
		turn_amp = KP * error - KD * (prev_error - error);

		prev_error = error;



		//apply some limits and threshold
		if(abs(turn_amp) < 50) {
			turn_amp = 0;
		}
		speed = min(speed, 100);

		setSpeed(speed);
		turn(turn_amp);
	}
*/


	if(serial.available() >= 2)
	{
//		alpha = 0;
		x_camera = (int)serial.read() - 50;		//offset used for the uart communication
		angle_camera = (int)serial.read() - 90;	//set zero degrees in front of the robot
		serial.print("info : position_radar =");
		serial.print(position_radar);
		serial.print("  angle=");
		serial.println(angle_camera);

//		last_command_time = millis();
	}

	//joker turn
	if(mf_get(&front_filter) < 40){
		setSpeed(-50);
		turn(-turn_amp*200);
		joker_time = millis();
		return;
	}


	if(!joker_time) {
		turn_amp = alpha * KTR * position_radar + (1-alpha) * KTA * angle_camera;
		turn_amp = CLAMP(-255, turn_amp, 255);
		speed = KS * (300-abs(turn_amp));
		speed = CLAMP(0, speed, 100);

		serial.print("turn :");
		serial.print(turn_amp);
		serial.print("  speed :");
		serial.println(speed);
		turn(turn_amp);
		setSpeed(speed);
	}

	if(joker_time && millis() - joker_time > JOKER_TIME)
	{
		joker_time = 0;
	}



	/*if(millis() - last_command_time > MAX_COMMAND_TIME) {
		turn(0);
		setSpeed(0);
	}*/


	if(millis() - led_blink_time > BLINK_PERIOD) {
		led_state ^= 1;
		digitalWrite(LED, led_state);
		led_blink_time = millis();
	}
}
