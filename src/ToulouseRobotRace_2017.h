// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _ToulouseRobotRace_2017_H_
#define _ToulouseRobotRace_2017_H_
#include "Arduino.h"
//add your includes for the project ToulouseRobotRace_2017 here


//end of add your includes here


//add your function definitions for the project ToulouseRobotRace_2017 here

#define CLAMP(m, n, M) min(max((m), (n)), (M))


typedef enum {
	NOIR = 0,
	BLANC = 1,
}FloorNature;

typedef enum {
	LEFT,
	LINE_LL,
	LINE_L,
	CENTER_L,
	CENTER,
	CENTER_R,
	LINE_R,
	LINE_RR,
	RIGHT
}RobotPosition;

enum {
	GRAB_LEFT,
	GRAB_RIGHT
};

#define TURN_LEFT 255
#define TURN_RIGHT -255

char* stateName[9] = {
	"LEFT",
	"LINE_LL",
	"LINE_L",
	"CENTER_L",
	"CENTER",
	"CENTER_R",
	"LINE_R",
	"LINE_RR",
	"RIGHT"
};

//Do not add code below this line
#endif /* _ToulouseRobotRace_2017_H_ */
