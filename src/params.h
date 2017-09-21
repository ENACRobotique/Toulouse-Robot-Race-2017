#ifndef __PARAMS_H__
#define __PARAMS_H__

#define MOT_DIR 4       //PD4
#define MOT_PWM 3       //PD3

#define DIR_ENABLE 6    //PD6
#define DIR_A 7         //PD7
#define DIR_B 8         //PB0

#define LED 13          //PB5

#define SR04_A 9        //PB1
#define SR04_B 10       //PB2

#define STX 9
#define SRX 10

#define SONAR1	0xEC >> 1
#define SONAR2	0xE2 >> 1

#define SONAR_FRONT	0x75
#define SONAR_RIGHT 0x71
#define SONAR_LEFT 0x77
#define SONAR_UP 0x70

#define MAX_SPEED 100

#define KTX -16
#define KTA -5
#define KS 0.5


#define KP 3
#define KD 2

#define SONAR_PERIOD 70
#define ASSERV_PERIOD 200
#define BLINK_PERIOD 500
#define MAX_COMMAND_TIME 2000

#define JOKER_TIME 1500

#endif  //__PARAMS_H__
