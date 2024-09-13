#ifndef HC_SR04_H
#define HC_SR04_H

#include "mbed.h"

class HC_SR04 {
private:
    Timer timer;
    InterruptIn interrupt_echo;
    PwmOut trigger_out;
    float distance;
    bool rdy;
    
    void rise_event();
    void fall_event();

public:
    HC_SR04(PinName echo, PinName trigger);
    float get_distance();
    
    void update_event();
};


#endif