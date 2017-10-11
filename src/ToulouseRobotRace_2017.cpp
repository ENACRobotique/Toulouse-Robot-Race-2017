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

unsigned long start_sonar_time, led_blink_time, asserv_time, time_intensity, grab_time, arch_time;
int front_distance, left_distance, right_distance, up_distance;
int prev_error;
int led_state;
int position_radar;
median_t front_filter;
int turn_amp, speed;
FloorNature floor_nature[4];
median_t intensity[4];
RobotPosition robotPosition;
int count_portique;
int cote_grab;

void setup() {
    pinMode(LED,OUTPUT);
    pinMode(MOT_DIR,OUTPUT);
    pinMode(MOT_PWM,OUTPUT);
    pinMode(DIR_A, OUTPUT);
    pinMode(DIR_B, OUTPUT);
    led_state = 0;
    count_portique = 0;
    grab_time = 0;
    serial.begin(115200);
    start_sonar_time = asserv_time = led_blink_time = time_intensity = arch_time = millis();
    Wire.begin();

    for(int i=0; i<4; i++) {
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
    setSpeed(50);
}

void loop() {
	if(millis() - time_intensity > INTENSITY_PERIOD) {
		for(int i=0; i<4; i++) {
			//int intensity_meas = (int) getIntensity(i) - offsets[i];
			int intensity_meas = (int) getIntensity(i+2);
			mf_update(&(intensity[i]), intensity_meas);
			if(mf_get(&(intensity[i])) < INTENSITY_LIMIT) {
				floor_nature[i] = BLANC;
			}
			else {
				floor_nature[i] = NOIR;
			}

			//serial.print(mf_get(&(intensity[i])));
			//serial.print("  ");
		}

		time_intensity = millis();



		int intensity_balance = floor_nature[0] + floor_nature[1]//+ floor_nature[2] + floor_nature[3]
							  - floor_nature[2] - floor_nature[3];// - floor_nature[6] - floor_nature[7];

		int intensity_sum = floor_nature[0] + floor_nature[1] + floor_nature[2] + floor_nature[3];
									  //+ floor_nature[4] + floor_nature[5] + floor_nature[6] + floor_nature[7];

//		serial.print(intensity_balance);
//		serial.print("  ");

		if(intensity_balance < 0)	//ligne noire à droite
		{
			switch (robotPosition) {
				case LEFT:
					robotPosition = LINE_LL;
					grab_time = millis();
					cote_grab = GRAB_LEFT;
					break;
				case LINE_LL:
					//ok
					break;
				case LINE_L:
					robotPosition = LINE_LL;
					break;
				case CENTER_L:
					//????
					break;
				case CENTER:
					robotPosition = CENTER_R;
					break;
				case CENTER_R:
					//ok
					break;
				case LINE_R:
					robotPosition = CENTER_R;
					break;
				case LINE_RR:
					robotPosition = CENTER_R;
					//bof
					break;
				case RIGHT:
					robotPosition = CENTER_R;
					//aïe
					break;
			}
		}
		else if(intensity_balance > 0)	//ligne noire à gauche
		 {
			switch (robotPosition) {
				case LEFT:
					robotPosition = CENTER_L;
					//aïe
					break;
				case LINE_LL:
					robotPosition = CENTER_L;
					//bof
					break;
				case LINE_L:
					robotPosition = CENTER_L;
					break;
				case CENTER_L:
					//ok
					break;
				case CENTER:
					robotPosition = CENTER_L;
					break;
				case CENTER_R:
					///????
					break;
				case LINE_R:
					robotPosition = LINE_RR;
					break;
				case LINE_RR:
					//ok
					break;
				case RIGHT:
					robotPosition = LINE_RR;
					grab_time = millis();
					cote_grab = GRAB_RIGHT;
					break;
			}
		 }
		else {
			switch (robotPosition) {
				case LEFT:
					if(intensity_sum == NOIR) {
						serial.print("  WTF L ");
					}
					else {
						//ok
					}
					break;
				case LINE_LL:
					if(intensity_sum == NOIR) {
						robotPosition = LINE_L;
					}
					else {
						robotPosition = LEFT;
					}
					break;
				case LINE_L:
					if(intensity_sum == NOIR) {
						//ok
					}
					else {
						serial.print("  WTF LINE_L ");
					}
					break;
				case CENTER_L:
					if(intensity_sum == NOIR) {
						robotPosition = LINE_L;
					}
					else {
						robotPosition = CENTER;
					}
					break;
				case CENTER:
					if(intensity_sum == NOIR) {
						serial.print("  WTF CENTER ");
					}
					else {
						//ok
					}
					break;
				case CENTER_R:
					if(intensity_sum == NOIR) {
						robotPosition = LINE_R;
					}
					else {
						robotPosition = CENTER;
					}
					break;
				case LINE_R:
					if(intensity_sum == NOIR) {
						//ok
					}
					else {
						serial.print("  WTF LINE_R ");
					}
					break;
				case LINE_RR:
					if(intensity_sum == NOIR) {
						robotPosition = LINE_R;
					}
					else {
						robotPosition = RIGHT;
					}
					break;
				case RIGHT:
					if(intensity_sum == NOIR) {
						serial.print("  WTF R ");
					}
					else {
						//ok
					}
					break;
			}
		}

		if(grab_time) {
			if(millis() - grab_time < GRAB_TIME) {
				if(cote_grab == GRAB_LEFT) {
					turn(TURN_LEFT);
				}
				else {
					turn(TURN_RIGHT);
				}
			} else {
				grab_time = 0;
			}

		} else {
			switch (robotPosition) {
				case LEFT:
					turn(TURN_RIGHT);
					break;
				case LINE_LL:
					turn(TURN_RIGHT);
					break;
				case LINE_L:
					turn(TURN_RIGHT);	//ou 0
					break;
				case CENTER_L:
					turn(0);
					break;
				case CENTER:
					turn(0);
					break;
				case CENTER_R:
					turn(0);
					break;
				case LINE_R:
					turn(TURN_LEFT);	//ou 0
					break;
				case LINE_RR:
					turn(TURN_LEFT);
					break;
				case RIGHT:
					turn(TURN_LEFT);
					break;
			}
		}
//		serial.print(stateName[robotPosition]);
//		serial.println("");
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
//		serial.print("up distance =");
		serial.println(up_distance);

		position_radar = (100 * (left_distance - right_distance)) / (left_distance + right_distance);
		//serial.print("position_radar =");
		//serial.println(position_radar);
	}

	if(mf_get(&front_filter) < 40) {
		if(position_radar > 0) {
			turn(TURN_RIGHT);
		} else {
			turn(TURN_LEFT);
		}
		setSpeed(-50);
		delay(500);
		setSpeed(0);
	}


	if(millis() - led_blink_time > BLINK_PERIOD) {
		led_state ^= 1;
		digitalWrite(LED, led_state);
		led_blink_time = millis();
	}

	if(up_distance < 200) {
		if(millis() - arch_time > ARCH_DELAY) {
			count_portique++;
			if(count_portique > 1) {
				setSpeed(0);
				while(true){
					serial.println("cource finie !");
				}
			}
		}

	}
}
