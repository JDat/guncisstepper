#define calibrateSpeed 1500
#define calibrateAccel 3000

#define scanSpeed 1000
#define scanAccel 2000

#define scannerOffset 7500

#include "MsTimer2.h"
#include "sensors.h"
//#include "Stepper.h"

#include "AccelStepper.h"

#define scanPin A5

#define buttonPin 11
#define buttonHomePin 12

#define in1 6
#define in2 7
#define in3 8
#define in4 9
#define ena1 5
#define ena2 10

#define right 1
#define left 0

long totalSteps;
//const int stepsPerRevolution = 200;
//Stepper myStepper(stepsPerRevolution, in4, in3, in2, in1);
AccelStepper myStepper(AccelStepper::FULL4WIRE, in4, in3, in2, in1);

sensors sens;

void setup() {
 Serial.begin(9600);
 pinMode(scanPin,OUTPUT);
 
 pinMode(buttonPin,INPUT_PULLUP);

  pinMode(ena1,OUTPUT);
  pinMode(ena2,OUTPUT);
  
 MsTimer2::set(1, sens.irModulate);
 MsTimer2::start();
 //digitalWrite(ledDrive,HIGH);

 myStepper.setMaxSpeed(1000);
 myStepper.setSpeed(50);  
 
 delay(100);
 //Serial.print("Calibrate: ");
 //Serial.println(calibrate());
 calibrate();
}

void loop() { 
  if (scanButton()){
    scan();
  }
  if (homeButton()){
    moveLeftEnd();
  }

}

uint8_t scanButton(){
  return !digitalRead(buttonPin);
}

uint8_t homeButton(){
  return !digitalRead(buttonHomePin);
}

long scan(){
  long countSteps=0;
  long paperStartSteps=0;
  long paperEndSteps=0;
  long centerSteps=0;
  long centerScannerSteps=0;
  
  moveLeftEnd();
  centerSteps=totalSteps/2;
  //Serial.println(totalSteps);
  //Serial.println(centerSteps);
  centerScannerSteps=centerSteps+scannerOffset;
  
  myStepper.setCurrentPosition(0);
  myStepper.setMaxSpeed(scanSpeed);
  myStepper.setAcceleration(scanAccel);
  myStepper.moveTo(totalSteps);
  myStepper.enableOutputs();
  enableStepper();
  
  //Search paper
  while(true){
    if (!sens.rightResult()){
      halt();
    }
    myStepper.run();
    countSteps++;
    if (sens.paperResult()){
      paperStartSteps=countSteps+scannerOffset ;
      break;  //paper found. Break
    }
  }
  //run while scanner is at Paper start
  while (countSteps<paperStartSteps){
    if (!sens.rightResult()){
      halt();
    }    
    myStepper.run();
    countSteps++;
  }
  //turn scanner on
  scannerOn();
  //calculate center pos
//  while(countSteps<centerSteps){
//    if (!sens.rightResult()){
//      halt();
//    }
//    myStepper.run();
//    countSteps++;
//  }
  //calculate paper end pos  
  //run until paper end
  //run until scanner end of paper
  paperEndSteps=paperStartSteps+2*(totalSteps/2-paperStartSteps)+scannerOffset*2;
  while(countSteps<paperEndSteps){
    if (!sens.rightResult()){
      halt();
    }
    myStepper.run();
    countSteps++;
  }
  scannerOff();
  
  delay(100);
  
  //return to home  
  myStepper.setCurrentPosition(0);
  myStepper.setMaxSpeed(calibrateSpeed);
  myStepper.setAcceleration(calibrateAccel);
  myStepper.moveTo(-10000000);
  myStepper.enableOutputs();
  while(sens.leftResult()){
    if (!sens.rightResult()){
      halt();
    }
    myStepper.run();
  }
  disableStepper();
  return countSteps;  
}

void halt(){
  while(true){ //halt        
    scannerOn();
    delay(200);
    scannerOff();
    delay(200);
  }
}
void scannerOn(){
  digitalWrite(scanPin,HIGH);
}

void scannerOff(){
  digitalWrite(scanPin,LOW);
}

long calibrate(){
  moveRightEnd();
  delay(100);
  totalSteps=moveLeftEnd();
  return totalSteps;
}
long moveRightEnd(){
  long countSteps=0;
  myStepper.setCurrentPosition(0);
  myStepper.setMaxSpeed(calibrateSpeed);
  myStepper.setAcceleration(calibrateAccel);
  myStepper.moveTo(1000000);
  myStepper.enableOutputs();
  enableStepper();
  while(sens.rightResult()){
    myStepper.run();
    countSteps++;
  }
  disableStepper();
  return countSteps;
}

long moveLeftEnd(){
  long countSteps=0;
  myStepper.setCurrentPosition(0);
  myStepper.setMaxSpeed(calibrateSpeed);
  myStepper.setAcceleration(calibrateAccel);
  myStepper.moveTo(-1000000);
  myStepper.enableOutputs();
  enableStepper();
  while(sens.leftResult()){
    myStepper.run();
    countSteps++;
  }
  disableStepper();
  return countSteps;
}

void enableStepper(){
  digitalWrite(ena1,HIGH);
  digitalWrite(ena2,HIGH);    
}
void disableStepper(){
  digitalWrite(ena1,LOW);
  digitalWrite(ena2,LOW);
}
