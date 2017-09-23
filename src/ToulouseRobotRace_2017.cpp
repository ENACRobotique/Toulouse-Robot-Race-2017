// Do not remove the include below
#include "ToulouseRobotRace_2017.h"

#include "SoftwareSerial.h"
#include "params.h"
#include "Wire.h"
#include "lib_us.h"
#include "commands.h"
#include "lib_intensity_cny70.h"

extern "C" {
#include "median_filter.h"
}

SoftwareSerial serial(SRX, STX);

unsigned long start_sonar_time, led_blink_time, asserv_time, time_intensity;
int front_distance, left_distance, right_distance, up_distance;
int prev_error;
int led_state;
int position_radar;
median_t front_filter;
int turn_amp, speed;
FloorNature floor_nature[8];
median_t intensity[8];
RobotPosition robotPosition;

//int offsets[8] = {3493,3266,3955,3674,3238,3632,3697,4259};
int offsets[8] = {255,28,716,436,0,394,459,1021};
void setup() {
    pinMode(LED,OUTPUT);
    pinMode(MOT_DIR,OUTPUT);
    pinMode(MOT_PWM,OUTPUT);
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
    led_state = 0;
    serial.begin(115200);
    start_sonar_time = asserv_time = led_blink_time = time_intensity = millis();
    Wire.begin();

    for(int i=0; i<8; i++) {
		floor_nature[i] = BLANC;
		mf_init(&(intensity[i]), 5, 3000);//
	}

    robotPosition = CENTER;

    mf_init(&front_filter, 4, 50);//
    startRange(SONAR_FRONT);
    startRange(SONAR_LEFT);
    startRange(SONAR_RIGHT);
    startRange(SONAR_UP);
    delay(80);

    serial.println("hello !");
}

void loop() {
	if(true || millis() - time_intensity > INTENSITY_PERIOD) {
		for(int i=0; i<8; i++) {
			int intensity_meas = (int) getIntensity(i) - offsets[i];
			mf_update(&(intensity[i]), intensity_meas);
			if(mf_get(&(intensity[i])) > INTENSITY_LIMIT) {
				floor_nature[i] = NOIR;
			}
			else {
				floor_nature[i] = BLANC;
			}

			serial.print(i);
			serial.print(": ");
			serial.print(mf_get(&(intensity[i])));
			serial.print("  floor:");
			serial.println(floor_nature[i]);
		}

		time_intensity = millis();
	}

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

//		serial.print(left_distance);
//		serial.print("\t");
//		serial.print(front_distance);
//		serial.print("\t");
//		serial.print(right_distance);
//		serial.print("\t");
//		serial.println(up_distance);

		position_radar = (100 * (left_distance - right_distance)) / (left_distance + right_distance);
		//serial.print("position_radar =");
		//serial.println(position_radar);
	}



	bool isThereWhite;
	for(int i=0; i<8; i++) {
		if(floor_nature[i] == BLANC) {
			isThereWhite = true;
			break;
		}
	}

	if(isThereWhite) {
		int intensity_balance = floor_nature[0] + floor_nature[1] + floor_nature[2] + floor_nature[3]
					          - floor_nature[4] - floor_nature[5] - floor_nature[6] - floor_nature[7];

		if(intensity_balance > 0) {
			robotPosition = LEFT;
		}
		if(intensity_balance > 0) {
			robotPosition = RIGHT;
		}

		turn_amp = intensity_balance * K_INTENSITY;
	}
	else {
		if(robotPosition == LEFT) {
			turn_amp = -255;
		}
		else if(robotPosition == RIGHT) {
			turn_amp = 255;
		}
	}




	turn(turn_amp);
	setSpeed(50);



	//joker turn
//	if(mf_get(&front_filter) < 40){
//		setSpeed(-50);
//		turn(-turn_amp*200);
//		joker_time = millis();
//		return;
//	}

	//setSpeed(50);

	if(millis() - led_blink_time > BLINK_PERIOD) {
		led_state ^= 1;
		digitalWrite(LED, led_state);
		led_blink_time = millis();
	}
}
