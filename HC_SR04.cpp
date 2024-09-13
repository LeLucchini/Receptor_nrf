#include "HC_SR04.h"

using namespace std::chrono;

HC_SR04::HC_SR04(PinName echo, PinName trigger): interrupt_echo(echo), trigger_out(trigger), rdy(false), distance(9999) {
    this->interrupt_echo.fall(callback(this, &HC_SR04::fall_event));
    this->interrupt_echo.rise(callback(this, &HC_SR04::rise_event));

    this->trigger_out.period_ms(500);
    this->trigger_out.pulsewidth_us(50);
}

void HC_SR04::fall_event() {
    this->timer.stop();
    // this->distance = duration_cast<microseconds>(timer.elapsed_time()).count() / 58.0;
    // this->timer.reset();
    this->rdy = true;
}

void HC_SR04::rise_event() {
    timer.start();
}


float HC_SR04::get_distance() {
    return this->distance;
}

void HC_SR04::update_event() {
    
    if(rdy){
        this->distance = duration_cast<microseconds>(timer.elapsed_time()).count() / 58.0;
        this->timer.reset();
        this->rdy = false;
    }

}