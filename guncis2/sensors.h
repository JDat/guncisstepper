#ifndef sensors_h
#define sensors_h
#include <Arduino.h>


#define ledDrive 13
#define swLeft A0
#define swRight A3
#define swPaper A4

#define numSamples 10
#define paperTreshold 20

class sensors {
  public:    
    //constructor
    sensors(void);

    static void irModulate(void);
    static uint8_t sensors::leftResult(void);
    static uint8_t sensors::rightResult(void);
    static uint8_t sensors::paperResult(void);
};
#endif
