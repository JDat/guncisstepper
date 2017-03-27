#include "sensors.h"

uint8_t irLedState;

    uint8_t readIndex;
    
    typedef struct  {
      int arr[numSamples];
      int total;
      int average;   
    } avg;
    
    avg leftLow;
    avg leftHigh;
    avg rightLow;
    avg rightHigh;
    avg paperLow;
    avg paperHigh;

uint8_t _leftResult;
uint8_t _rightResult;
uint8_t _paperResult;

sensors::sensors(){
  pinMode(ledDrive,OUTPUT);
}
static uint8_t sensors::leftResult(){
  return _leftResult;
}

static uint8_t sensors::rightResult(){
  return _rightResult;
}

static uint8_t sensors::paperResult(){
  return _paperResult;
}

static void sensors::irModulate(){
  if (irLedState){
    leftLow.total = leftLow.total - leftLow.arr[readIndex];
    leftLow.arr[readIndex] = analogRead(swLeft);
    leftLow.total = leftLow.total + leftLow.arr[readIndex];  

    rightLow.total = rightLow.total - rightLow.arr[readIndex];
    rightLow.arr[readIndex] = analogRead(swRight);
    rightLow.total = rightLow.total + rightLow.arr[readIndex];  

    paperLow.total = paperLow.total - paperLow.arr[readIndex];
    paperLow.arr[readIndex] = analogRead(swPaper);
    paperLow.total = paperLow.total + paperLow.arr[readIndex];  

    digitalWrite(ledDrive,HIGH);
  } else {
    leftHigh.total = leftHigh.total - leftHigh.arr[readIndex];
    leftHigh.arr[readIndex] = analogRead(swLeft);
    leftHigh.total = leftHigh.total + leftHigh.arr[readIndex];  

    rightHigh.total = rightHigh.total - rightHigh.arr[readIndex];
    rightHigh.arr[readIndex] = analogRead(swRight);
    rightHigh.total = rightHigh.total + rightHigh.arr[readIndex];  

    paperHigh.total = paperHigh.total - paperHigh.arr[readIndex];
    paperHigh.arr[readIndex] = analogRead(swPaper);
    paperHigh.total = paperHigh.total + paperHigh.arr[readIndex];  

    digitalWrite(ledDrive,LOW);
  }
  irLedState= ~irLedState;

  readIndex = readIndex + 1;
  
  if (readIndex >= numSamples) {
    readIndex = 0;
  }

  leftLow.average = leftLow.total / numSamples;  
  leftHigh.average = leftHigh.total / numSamples;
  _leftResult=(leftLow.average-leftHigh.average) ? 1 : 0;

  rightLow.average = rightLow.total / numSamples;  
  rightHigh.average = rightHigh.total / numSamples;
  _rightResult=(rightLow.average-rightHigh.average) ? 1 : 0;

  paperLow.average = paperLow.total / numSamples;  
  paperHigh.average = paperHigh.total / numSamples;
  //paperResult=(paperLow.average-paperHigh.average-200) ? 200 : 0;
  if (paperLow.average-paperHigh.average>paperTreshold){
    _paperResult=1;
  } else {
    _paperResult=0;
  }

}
