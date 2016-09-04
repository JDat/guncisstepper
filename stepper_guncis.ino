// User settable parameters

//speed and acceleration while calibrating
#define calibrateSpeed 3500     //practical maximum: 4000-5000. Can set 100000 if want, but it's useless
#define calibrateAccel 15000    //can set more, but useless
#define scanCalibrateTime 1000   //scanner button press time while calibrating 1000= 1 sec

//speed and acceleration while scaning
#define scanSpeed 2500          //practical maximum: 4000-5000. Can set 100000 if want, but it's useless
#define scanAccel 5000         //can set more, but useless
#define magicSteps 15           //magic! compensate motor prblems by adding some steps to total move
#define deltaScan 350            // delta in steps between sensor and scanner optics center. Positive only!!!

//scanner sensor invert?
#define scanActive LOW
#define scanInactive HIGH

// Pin definition
//limit sensors
#define homePin 2
#define endPin 3
//motor pins
#define stepPin 4
#define directionPin 12
#define enaPin 11

//other
#define scanSolenoid 13
#define scanButton 9
#define scanSensor 8

#include <AccelStepper.h>

AccelStepper stepper(AccelStepper::DRIVER, stepPin, directionPin); // Defaults to AccelStepper::DRIVER

long totalSteps=0;      //total steps betwenn limit switches

//initialise I/O pins
void initPins(){
  pinMode(homePin,INPUT_PULLUP);
  pinMode(endPin,INPUT_PULLUP);
  pinMode(scanButton,INPUT_PULLUP);
  pinMode(scanSensor,INPUT_PULLUP);
  pinMode(scanSolenoid,OUTPUT);  
}

void initStepper(){
  stepper.setEnablePin(enaPin);
  stepper.setPinsInverted(true);
  stepper.disableOutputs();  
}

//do this only one time while booting
void setup() {
  //Serial.begin(115200);     //debug
  initPins();
  initStepper();
  
  solenoidOff();
  calibrate();
}

//repeat this forever
void loop() {

  //wait while scan button pressed
  while(digitalRead(scanButton)){    
  }
  //do scanning
  scan();

}

void scan(){
  long scanStart;
  long scanStop;
  byte scanState;
  byte noReload;
  
  scanState=false;
  noReload=false;
  scanStop=totalSteps;
  scanStart=scanStop;
  stepper.setMaxSpeed(scanSpeed);
  stepper.setAcceleration(scanAccel);
  stepper.moveTo(totalSteps+magicSteps);
  stepper.enableOutputs();
  while((gotEnd()==false) && stepper.isRunning()){
    if ((digitalRead(scanSensor)==scanActive) && (scanState==false) && (noReload==false)){
      scanStart=stepper.currentPosition()+deltaScan;
      scanState=true;
      noReload=true;
    }

    if ((digitalRead(scanSensor)==scanInactive) &&(scanState==true)){
      scanStop=stepper.currentPosition()+deltaScan;
      scanState=false;
    }    

    if ((stepper.currentPosition()>=scanStart) && (stepper.currentPosition()<=scanStop) ){
      solenoidOn();      
    }
    else {
      solenoidOff();
    }
    if (stepper.currentPosition()>=scanStop){break;}
    stepper.run();
  }
  solenoidOff();
  stepper.stop();

  stepper.setMaxSpeed(calibrateSpeed);
  stepper.setAcceleration(calibrateAccel);

  stepper.moveTo(0);
  while((gotHome()==false) && stepper.isRunning()){
    stepper.run();
  }

  stepper.stop();  
  stepper.disableOutputs();
}

//check for home limit sensor
byte gotHome(){return digitalRead(homePin) ? false : true;}
//check for end limit sensor
byte gotEnd(){return digitalRead(endPin) ? false : true;}
//scanner button (solenoid) control
void solenoidOn(){digitalWrite(scanSolenoid,HIGH);}
void solenoidOff(){digitalWrite(scanSolenoid,LOW);}

//initial calibration move
void calibrate(){
  stepper.setMaxSpeed(calibrateSpeed);
  stepper.setAcceleration(calibrateAccel);

  stepper.moveTo(100000);      //move to end. Fake step number, must bee BIG
  stepper.enableOutputs();
  //moving loop with end limit checking
  while(gotEnd()==0){
    stepper.run();
  }

  //change direction by faking stepper motor
  stepper.setPinsInverted(false);   //hack direction
  stepper.setCurrentPosition(0);    //hack postion
  stepper.moveTo(100000);     //move to home. Fake step number, must bee BIG

  //moving loop with home limit cheking
  while(gotHome()==0){
    stepper.run();
  }
  
  totalSteps=stepper.currentPosition();   //get steps from full trolley move
  
  initStepper();              //reinit stepper after hack
  stepper.setCurrentPosition(0);    //set true zero postion
  stepper.disableOutputs();         //diable motor

  //tur on scanner for calibration read
  solenoidOn();
  delay(scanCalibrateTime);
  solenoidOff();
}
/*
void atHome(){
  stepper.stop();
  //stopReason=stopHome;  
  //stepper.setCurrentPosition(0);
}

void atEnd(){
  stepper.stop();
  //stopReason=stopEnd;
  //stepper.setCurrentPosition(8000);

}
*/
