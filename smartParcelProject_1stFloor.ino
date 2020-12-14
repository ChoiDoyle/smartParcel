#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>
//Pin declaration
const byte pinTrig = 12;
const byte pinEcho = 13;
const byte pinLMTF = 5;
const byte pinLMTB = 4;
const byte pinLMT = 3;
const byte pinRMTF = 7;
const byte pinRMTB = 6;
const byte pinRMT = 11;

//Motor parameters declaration
const byte speed=150;
const byte speedUphill = 250;
const int leftTurnPeriod = 980;
const int rightTurnPeriod = 1000;
const int leftTurnPeriod2 = 1150;
const int rightTurnPeriod2 = 1200;

//Ultrasonic sensor parameter declaration
volatile long distance = 500;

//Customized class for ultrasonic sensor
class Ultrasonic{
  byte pinTrig;
  byte pinEcho;
  unsigned long previousMillis, serialMillis, echoStartMicros, duration;
  bool echoSignal;
  //Object for ultrasonic sensor
  public:Ultrasonic(byte trigPin, byte echoPin){
    pinTrig = trigPin;
    pinEcho = echoPin;
    pinMode(pinTrig, OUTPUT);
    pinMode(pinEcho, INPUT);
    digitalWrite(pinTrig,LOW);
    echoStartMicros=0;
    duration = 0;
    echoSignal = false;
    previousMillis = 0;
  }
  //Function for checking distance every 0.2seconds
  void checkDistance(){
    if(millis()-previousMillis>200){
      previousMillis=millis();
      digitalWrite(pinTrig,HIGH);
      delayMicroseconds(10);
      digitalWrite(pinTrig,LOW);
    }
    if((digitalRead(pinEcho)==true)&(echoSignal==false)){
      echoSignal = true;
      echoStartMicros = micros();
    }
    if(echoSignal==true){
      if(digitalRead(pinEcho)==false){
        echoSignal=false;
        duration = (micros()-echoStartMicros);
        distance = duration*0.34/2;
      }
    }
    if((millis()-serialMillis)>200){
      serialMillis = millis();
      Serial.print("Distance = ");
      Serial.println(distance);
    }
  }
};

//Object declaration
Servo myServo;
SoftwareSerial btSerial(9,8);
Ultrasonic myUltrasonic(12,13);

//Function for delaying without using delay
void delayWOdelay(int duration){
  unsigned long preMillis = millis();
  while(millis()-preMillis<duration){}
}

//Functions for driving the robot
void driveFront(byte vel){
  digitalWrite(pinRMTF, HIGH);
  digitalWrite(pinRMTB, LOW);
  digitalWrite(pinLMTF, HIGH);
  digitalWrite(pinLMTB, LOW);
  analogWrite(pinRMT, vel);
  analogWrite(pinLMT, vel);
  Serial.println("driving front");
}

void driveBack(){
  digitalWrite(pinRMTF, LOW);
  digitalWrite(pinRMTB, HIGH);
  digitalWrite(pinLMTF, LOW);
  digitalWrite(pinLMTB, HIGH);
  analogWrite(pinRMT, speed);
  analogWrite(pinLMT, speed);
  Serial.println("driving back");
}

void turnLeft(){
  digitalWrite(pinRMTF, HIGH);
  digitalWrite(pinRMTB, LOW);
  digitalWrite(pinLMTF, LOW);
  digitalWrite(pinLMTB, HIGH);
  analogWrite(pinRMT, speed);
  analogWrite(pinLMT, speed);
  Serial.println("turning left");
}

void turnRight(){
  digitalWrite(pinRMTF, LOW);
  digitalWrite(pinRMTB, HIGH);
  digitalWrite(pinLMTF, HIGH);
  digitalWrite(pinLMTB, LOW);
  analogWrite(pinRMT, speed);
  analogWrite(pinLMT, speed);
  Serial.println("turning right");
}

void stopDrive(){
  digitalWrite(pinRMTF, LOW);
  digitalWrite(pinRMTB, LOW);
  digitalWrite(pinLMTF, LOW);
  digitalWrite(pinLMTB, LOW);
  analogWrite(pinRMT, 0);
  analogWrite(pinLMT, 0);
  Serial.println("Stopping driving");
}

//Function when obstacle is detected
void alternateRoute(unsigned long remain){
  digitalWrite(10,HIGH);
  turnRight();
  delayWOdelay(rightTurnPeriod);
  digitalWrite(10,LOW);
  driveFront(speed);
  delayWOdelay(1000);
  turnLeft();
  delayWOdelay(leftTurnPeriod);
  stopDrive();
  delayWOdelay(500);
  driveFront(speed);
  delayWOdelay(1000);
  stopDrive();
  delayWOdelay(500);
  turnLeft();
  delayWOdelay(leftTurnPeriod);
  stopDrive();
  delayWOdelay(500);
  driveFront(speed);
  delayWOdelay(1000);
  stopDrive();
  delayWOdelay(500);
  turnRight();
  delayWOdelay(rightTurnPeriod);
  stopDrive();
  delayWOdelay(500);
  driveFront(speed);
  delayWOdelay((remain-1000));
  stopDrive();
  delayWOdelay(500);
}

void setup(){
  //Setup for signal transfer
  btSerial.begin(9600);
  Serial.begin(9600);
  //Setup for outputs
  pinMode(pinRMTF, OUTPUT);
  pinMode(pinRMTB, OUTPUT);
  pinMode(pinRMT, OUTPUT);
  pinMode(pinLMTF, OUTPUT);
  pinMode(pinLMTB, OUTPUT);
  pinMode(pinLMT, OUTPUT);
  //Setup for interrupt signal
  pinMode(2,OUTPUT);
  pinMode(10,OUTPUT);
}

void loop(){
  digitalWrite(2, LOW);
  digitalWrite(10, LOW);
  //Get the signal every 0.1seconds
  int btValue;
  if (btSerial.available()){
    btValue = btSerial.read()-'0';
    Serial.println(btValue);
    unsigned long preMillis = millis();
    //Robot drives according to bluetooth signal
    switch(btValue){
      case 1:{
        driveFront(speed);
        while(millis()-preMillis<3100){
          myUltrasonic.checkDistance();
          if(distance<300){
            unsigned long remainMillis = millis()-preMillis;
            alternateRoute(remainMillis);
            distance=3000;
            break;
          }else{
            delayWOdelay(100);
          }
        }
        driveFront(speedUphill);
        delayWOdelay(2500);
        //Stay still until elevator goes to 2nd floor
        stopDrive();
        delayWOdelay(20000);
        //When elevator reaches 2nd floor
        driveBack();
        delayWOdelay(1000);
        turnLeft();
        delayWOdelay(leftTurnPeriod2);
        stopDrive();
        digitalWrite(2,HIGH);
        delayWOdelay(100);
        digitalWrite(2,LOW);
        delayWOdelay(5000);
        turnLeft();
        delayWOdelay(leftTurnPeriod2);
        stopDrive();
        delayWOdelay(500);
        driveBack();
        delayWOdelay(1000);
        //Stay still until elevator goes to 1st floor
        stopDrive();
        delayWOdelay(15000);
        //When elevator reaches 1st floor
        driveFront(speed);
        delayWOdelay(2000);
        stopDrive();
        break;
      }
    }
  }
  delayWOdelay(100);
}
