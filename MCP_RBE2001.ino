#include <Servo.h>
#include <TimerOne.h>
#include <BluetoothClient.h>
#include <BluetoothMaster.h>
//all pins are temporary and need changing
#define leftDrivePin 24
#define rightDrivePin 23
#define fourBarPin 25
#define rackMotorPin 26
#define grabberServoPin 27
#define limitPin A0
#define lineSensePin1 A1
#define lineSensePin2 A2
#define lineSensePin3 A3
#define lineSensePin4 A4
#define potPin A5

#define leftFWD 180
#define rightFWD 0
#define leftBWD 0
#define rightBWD 180

char reactor; //reactor type
int lineCount; 
int lineFlag; //flag when line detected
int lightThreshold = 600; //threshold for if a line sensor is on light or dark, above threshold = dark
int measError; //difference in line tracker sensor values
int error, leftSpeed, rightSpeed; 
int heartBeatCounter = 0;


Servo leftDrive;
Servo rightDrive;
Servo fourBarMotor;
Servo rackMotor;
Servo grabberServo;

enum State{
	findReactor,
	grabSpent,
	findDisposal,
	placeSpent,
	findSupply,
	grabSupply,
	returnToReactor,
	placeSupply
};

State state;

void setup(){
	leftDrive.attach(leftDrivePin,1000,2000);
	rightDrive.attach(rightDrivePin,1000,2000);
	fourBarMotor.attach(fourBarPin,1000,2000);
	rackMotor.attach(rackMotorPin,1000,2000);
	grabberServo.attach(grabberServoPin);

	pinMode(lineSensePin1, INPUT);
	pinMode(lineSensePin2, INPUT);
	pinMode(lineSensePin3, INPUT);
	pinMode(lineSensePin4, INPUT);
	pinMode(potPin, INPUT);
	pinMode(limitPin, INPUT);
	state = findReactor;

	reactor = 'A'; //reactor type
	lineCount = 0; //counts the amount of lines
	lineFlag = 0; //flag for when line detected 

	Timer1.initialize(100000);
	Timer1.attachInterrupt(100msISR);
}

void loop(){
  
 	stateMachine();

}
//master state machine
void stateMachine(){
	switch (state) {
	    case findReactor: 
	      if(reactor == 'A'){ //for reactor A
	      	followLine();	
	      	if(lineHit(1)){ //changes depends on where we start
	      		if(digitalRead(limitPin) == LOW){
	      		state = grabSpent;
	      		}
	      	}
	      }
	      if(reactor == 'B'){ //for reactor B
	      	if(lineHit(4)){ //might need to change depending on where it turns around
	      		if(digitalRead(limitPin) == LOW){
	      		state = grabSpent;
	      		}
	      	}
	      }
	      break;
	    case grabSpent:
	      // do something
	      break;
	    case findDisposal:
	    	//stuff
	    	break;
	    case placeSpent:
	    	//stuff
	    	break;
	    case findSupply:
	    	//stuff
	    	break;
	    case grabSupply:
	    	//stuff
	    	break;
	    case returnToReactor:
	    	//stuff
	    	break;
	    case placeSupply:
	    	//stuff
	    	break;
	}

}

//returns boolean if a certain amount of lines have been hit
boolean lineHit(int x){
	if((lineFlag == 0) && (crossHit())){
		lineCount++;
		lineFlag = 1; 
	}else if((lineFlag == 1) && (!crossHit()){
		lineFlag = 0;			
	}				
	if(lineCount == x){
		return true;
	}else{
		return false;
	}
}

void forward()
{
	leftDrive.write(leftFWD);
	rightDrive.write(rightFWD);
}

void HundredMsISR() 
{
	fetchBluetooth();
	heartBeatCounter++;
	if(heartBeatCounter == 20)
	{
		doHeartBeat();
		heartBeatCounter = 0;
	}
}

void fetchBluetooth()
{
	
}

//method to do line tracking until the robot drives over a line
void followLine() 
{
	while(crossHit() != true)
	{
		getError();
		leftDrive.write(leftSpeed);
		rightDrive.write(leftSpeed);
	}
}

//finds difference in line sensor values, sets that value to a useable motor speeds
void getError()
{
 	measError = analogRead(lineSensePin1) - analogRead(lineSensePin3);
	error = map(measError, -1023, 1023, 0, 180);
	leftSpeed = error;
	rightSpeed = 180 - error;
}

//method to see if the far line sensor and at least one other line sensor on on a line or not indicating a cross
//returns true if a cross is hit, false otherwise 
boolean crossHit() 
{
	if(overLine(lineSensePin4) && (overLine(lineSensePin1) || overLine(lineSensePin2)|| overLine(lineSensePin3)))
  	{
  		return true;
  	} 
  	else 
  	{
  		return false;
  	}
}

//method to check if a line sensor is on the line or not
//returns true if it's on dark, false otherwise
boolean overLine(int lineSensorPin) 
{
	if (analogRead(lineSensorPin) >= lightThreshold))
	{
		return true;
	}
	else
	{
		return false;
	}
}