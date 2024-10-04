#include "HC_SR04_2.h"
#include <chrono>

HC_SR04_2::HC_SR04_2(): trigger(PTA12), echo(PTD1)  {
    echo.mode(PullNone);
}

int HC_SR04_2::getDistance() {
    this->t.reset();
    this->trigger = 1;
    wait_us(10);
    this->trigger = 0;
    while(echo == 0);
    this->t.start();
    while(echo == 1);
    this->t.stop();
    return static_cast<int>(duration_cast<microseconds>(t.elapsed_time()).count() / 58.0);
    
}