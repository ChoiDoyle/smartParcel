#include <HX711.h>
#include <Servo.h>

//Pin declaration
const byte pinServo = 5;
const byte pinMTF = 7;
const byte pinMTB = 6;
const byte pinMT = 9;

//Angle declaration for servo motor
const byte closeAngle = 50;
const byte openAngle = 140;

//Pin declaration for 7segment display
const byte pins[] = {13, 12, 11, 4, 3};
const byte pinsNum = 5;
const byte digitForNum[2][5] = {{1,0,1,1,1},{0,1,0,0,0}};

//Elevator speed for floor change
const byte oneFloorUp = 250;
const byte oneFloorDown = 150;

//Object declaration
HX711 scale(A1, A0); 
Servo myServo;

//Function for floor change
void goToFloor(int floor){
  if(floor == 2){
    digitalWrite(7, LOW);
    digitalWrite(6, HIGH);
    digitalWrite(9, oneFloorUp);
  }else if(floor == 1){
    digitalWrite(7, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(9, oneFloorDown);
  }else if(floor == 0){
    digitalWrite(7, LOW);
    digitalWrite(6, LOW);
    digitalWrite(9, 0);
  }
}

//Function for 7segment
void showNum(byte num){
  switch(num){
    case 1:{
      for(byte i = 0 ; i<pinsNum ; i++){
        digitalWrite(pins[i], digitForNum[0][i]);
      }
      break;
    }
    case 2:{
      for(byte i = 0 ; i<pinsNum ; i++){
        digitalWrite(pins[i], digitForNum[1][i]);
      }
      break;
    }
  }
}

void setup(){
  //Setup for servo motor(starting with door opened)
  myServo.attach(pinServo);
  myServo.write(openAngle);

  //Setup for load cell
  scale.set_scale(2280.f); 
  scale.tare();
  Serial.begin(9600);

  //Setup for 7segment
  for(byte i = 0 ; i<pinsNum ; i++){
    pinMode(pins[i], OUTPUT);
  }
}

void loop(){
  //1. Show the state via 7segment display
  showNum(1);
  //2. Check whether the robot has come in using load cell
  while(1){
    float a = scale.get_units();
    Serial.println(a);
    if(a<-50 || a>50){
      break;
    }
    scale.power_down();
    delay(1000);
    scale.power_up();
  }
  delay(1000);
  //3. Close the blocking door
  myServo.write(closeAngle);
  delay(1000);
  //4. Lift the elevator towards 2nd floor
  goToFloor(2);
  delay(20000);
  goToFloor(0);
  //5. Show the state via 7segment display
  showNum(2);
  delay(3000);
  //6. Open the blocking door
  myServo.write(openAngle);
  delay(1000);
  //7. Check whether the robot has gone out using load cell
  while(scale.get_units()<-50 || scale.get_units()>50){
  scale.power_down();
  delay(1000);
  scale.power_up();
  }
  delay(5000);
  //8. Check whether the robot has come in using load cell
  while(scale.get_units()>-40){
    scale.power_down();
    delay(1000);
    scale.power_up();
  }
  delay(3000);
  //9. Close the blocking door
  myServo.write(closeAngle);
  delay(1000);
  //10. Move the elevator to 1st floor
  goToFloor(1);
  delay(15000);
  goToFloor(0);
  //11. Show the state via 7segment display
  showNum(1);
  delay(3000);
  //12. Open the blocking door
  myServo.write(openAngle);
  delay(10000);
}
