


//---------------------------------NON USER GLOBALS-----------------------------------------------
//The following variables aren't user settings. They are counters, etc 

const byte maxAxes=4; //None of our stages will ever have more than 4 axes. 


long values[maxAxes]; // array holding values for all the received fields from the seria1 port

// buttonStageLocations
// The buttons will be used to store 4 different stage locations. We store these in a 2-D
// array, where the first dimension is button and the second is axis:
float buttonStageLocations[4][maxAxes] = {{0,0,0,0},
                                          {0,0,0,0},
                                          {0,0,0,0},
                                          {0,0,0,0}};

// locationStored
// zero indicates that no position was stored. As positions are stored we update these to 1. 
// This is important, as it minimises the possibility of the user double-clicking a button 
// unbound to a location as it may make the stage perform a large motion to back to zero.
bool locationStored[4]={0,0,0,0};


// SPEEDMAT stores the motor speed for each hat-stick value at each speed mode. These numbers
// are calculated once in the setup function and never change during execution. Pre-calculating 
// them is a good way of improving performance during hat-stick motions. 
float SPEEDMAT[128][4];


// stagePosition
// The current stage position in microns. 
float stagePosition[maxAxes]={0,0,0,0}; 


// stepperPreviousPos
// The previous position of the steppers. We need to keep track of this in a counter in order to 
// figure out how far we've pushed the stage each pass through the main loop. The stage position 
// will be updated whenever pollPS3 is called. Note that stepperPreviousPos relates to stepper 
// motor counts (which are integers) not the actual stage position (which a float and measured 
// in microns).
long stepperPreviousPos[4]={0,0,0,0}; 


// thisStep
// How far the last step has pushed the stage in microns. This is used to update the stage postion.
float thisStep[maxAxes][4];


float currentSpeed[maxAxes]; //An array defining the speed of each axis 

//------------------------------------------------------------------------------------------------













