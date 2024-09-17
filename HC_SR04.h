#ifndef HC_SR04_H
#define HC_SR04_H

#include "mbed.h"

class HC_SR04 {
private:
    Timer timer;
    InterruptIn interrupt_echo;
    PwmOut trigger_out;
    int distance;
    //bool rdy;
    Timeout tout;

    float updateSpeed;
    int start;
    int end;
    volatile int done;
    void (*_onUpdateMethod)(int);
    void startT(void);
    void updateDist(void);
    void startTrig(void);
    float timeout;

    int d;
    
    //void rise_event();
   // void fall_event();

public:
    HC_SR04(PinName echo, PinName trigger, float updateSpeed, float timeout, void onUpdate(int));
    float get_distance();
    
    void attachOnUpdate(void callback(int));

    int getCurrentDistance(void);

    void pauseUpdates(void);

    void startUpdates(void);

    int isUpdated(void);

    void checkDistance(void);

    void changeUpdateSpeed(float updateSpeed);

    float getUpdateSpeed(void);

    BufferedSerial * serial;
};


#endif