#define numSteps 25000
#define driveSpeed 1000
#define accell 100>>1

#define magic 10

#define startPin 2
#define stopPin 3
#define stepPin 4
#define directionPin 12
#define enablePin 11
#define delaySignal 5

volatile byte eStop;
void setup() {
  //Serial.begin(9600);
  pinMode(startPin,INPUT_PULLUP);
  pinMode(stopPin,INPUT_PULLUP);
  pinMode(stepPin,OUTPUT);
  pinMode(enablePin,OUTPUT);
  pinMode(directionPin,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(startPin),emergencyBegin,FALLING);
  attachInterrupt(digitalPinToInterrupt(stopPin),emergencyEnd,FALLING);
}
void loop() {
  digitalWrite(enablePin,HIGH);
  long a=doAccell();
  if (a>0) {
    if (steps(numSteps-a*2,driveSpeed)==0){
      if (a>0){
        if (doDeccell(a)==0);  
        {
          changeDirection();
        }
      }    
      else
      {
        changeDirection();
      }
    }
    else
    {
      changeDirection();
    }
  }
  else
  {
    changeDirection();
  }
  //changeDirection();
  //doStep();
  digitalWrite(enablePin,LOW);
  delay(2000);
}

void changeDirection(){
  digitalWrite(directionPin,!digitalRead(directionPin));
}
long doAccell(){
  long Pt=0,Vt=0;
  long Pt0=0,Vt0=0;
  long a=accell;
  while (Vt<driveSpeed){
    Pt=Pt+Vt+a;
    Vt=Vt+a;
    if (steps((Pt-Pt0)/magic,Vt)){
      return 0;
    }
    //steps(Pt-Pt0,Vt-Vt0);
    Pt0=Pt;
    Vt0=Vt;
  }
  return Pt;
}
long doDeccell(long points){
  long Pt=points,Vt=driveSpeed;
  long Pt0=0,Vt0=0;
  long a=-accell;
  while (Vt>0){
    Pt=Pt+Vt+a;
    Vt=Vt+a;
    if (steps((Pt-Pt0)/magic,Vt)){
      return 0;
    }
    //steps(Pt-Pt0,Vt-Vt0);
    Pt0=Pt;
    Vt0=Vt;
  }
  return Pt;
}

byte steps(long steps,long curSpeed) {
  int currpps=1000000/curSpeed;
  eStop=false;
  for (long i=0;i<steps;i++){
    if (eStop) {
      changeDirection();
      for (byte j=0;j>10;j++){
        doStep();
        delayMicroseconds(16000);
      }
      changeDirection();
      return -1;
    }
    doStep();
    delayMicroseconds(currpps);
  }
  return 0;
}

void doStep() {
  digitalWrite(stepPin,HIGH);
  delayMicroseconds(delaySignal);
  digitalWrite(stepPin,LOW);
  delayMicroseconds(delaySignal);

}

void emergencyBegin(){
  eStop=true;
}

void emergencyEnd(){
  eStop=true;
}

