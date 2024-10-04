#ifndef HC_SR04_2_H

#define HC_SR04_2_H

#include "mbed.h"

using namespace std;
using namespace std::chrono;

//HC_SR04
#define HC_SR04_ECHO PTD1
#define HC_SR04_TRIGGER PTA12


class HC_SR04_2{
public:
    HC_SR04_2();
    int getDistance();
    
private:
    DigitalOut trigger;
    DigitalIn echo;
    Timer t;

};

#endif