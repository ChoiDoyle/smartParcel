#include <Arduino.h>
#include <Stepper.h>

//Speed for stepping motor
#define STEPPERREV 2048
const byte landingRPM = 10;

//Pin declaration
const byte pinIn1 = 8;
const byte pinIn2 = 9;
const byte pinIn3 = 10;
const byte pinIn4 = 11;
const byte pinR = 5;
const byte pinG = 6; 
const byte pinPSV = 12;

//Frequency declaration
const int clap = 2000;
int dou = 261;
int re = 293;
int mi = 329;
int fa = 349;
int sol = 392;
int ra = 440;
int si = 494;

//Interrupt flags
int landingFlag = LOW;
int emergencyFlag = LOW;


//Object declaration
Stepper myStepper(STEPPERREV, pinIn1, pinIn3, pinIn2, pinIn4);

//Functions for RGB led
void setRED(){
	Serial.println("red");
	analogWrite(pinR, 255);
	analogWrite(pinG, 0);
}
void setGREEN(){
	Serial.println("green");
	analogWrite(pinR, 0);
	analogWrite(pinG, 255);
}

//Function for landing parcel
void landObject(){
	myStepper.step(-STEPPERREV/3);
	delayMicroseconds(100);
	myStepper.step(STEPPERREV/3);
}

//Interrupt functions
void landing(){
	landingFlag = HIGH;
}
void emergency(){
	emergencyFlag = HIGH;
}

//Function for passive buzzer
void playOneClap(int pitch, int duration){
	tone(pinPSV, pitch);
	delay(duration);
	noTone(pinPSV);
	delay(pinPSV/10);
}

//Function for playing emergency music
void playMusic(){
	playOneClap(mi, clap/8);
	playOneClap(sol, clap/8);
	playOneClap(sol, clap/8);
	noTone(pinPSV);
	delay(clap/8);
	playOneClap(mi, clap/8);
	playOneClap(sol, clap/8);
	playOneClap(sol, clap/8);
	noTone(pinPSV);
	delay(clap/8);
	playOneClap(ra, clap/8);
	playOneClap(ra, clap/8);
	playOneClap(ra, clap/8);
	playOneClap(ra, clap/8);
	playOneClap(ra, clap/4);
	noTone(pinPSV);
	delay(clap/4);
	
	playOneClap(sol, clap/8);
	playOneClap(sol, clap/8);
	playOneClap(sol, clap/8);
	playOneClap(sol, clap/8);
	playOneClap(fa, clap/8);
	playOneClap(fa, clap/8);
	playOneClap(fa, clap/8);
	playOneClap(fa, clap/8);
	playOneClap(mi, clap/8);
	playOneClap(mi, clap/8);
	playOneClap(mi, clap/8);
	playOneClap(mi, clap/8);
	playOneClap(mi, clap/4);
	noTone(pinPSV);
	delay(clap/4);	
}

void setup(){
	Serial.begin(9600);
	
	//Setup for stepping motor
	myStepper.setSpeed(landingRPM);
	
	//Interrupt declaration
	attachInterrupt(1,landing,RISING);
	attachInterrupt(0,emergency,RISING);
	
	//Setup for passive buzzer
	pinMode(pinPSV, OUTPUT);
}

void loop(){
	//1. Set led green
	setGREEN();
	//2. Check the state every 0.5 seconds
	/*2-a. When landing flag is HIGH, 
			go over landing function*/
	if(landingFlag == HIGH){
		landingFlag = LOW;
		landObject();
	}
	/*2-b. When emergency flag is HIGH, 
			go over emergency function*/
	if(emergencyFlag == HIGH){
		emergencyFlag = LOW;
		setRED();
		delay(500);
		playMusic();
		delay(500);
		setGREEN();
		delay(500);
	}
	delay(500);
}