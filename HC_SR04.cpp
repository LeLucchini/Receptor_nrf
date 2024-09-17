#include "HC_SR04.h"

using namespace std::chrono;

HC_SR04::HC_SR04(PinName echo, PinName trigger, float updateSpeed, float timeout, void onUpdate(int)): interrupt_echo(echo), trigger_out(trigger), distance(9999) {
    // this->interrupt_echo.fall(callback(this, &HC_SR04::fall_event));
    // this->interrupt_echo.rise(callback(this, &HC_SR04::rise_event));

    // this->trigger_out.period_ms(500);
    // this->trigger_out.pulsewidth_us(50);
    _onUpdateMethod = onUpdate;
    updateSpeed = updateSpeed;
    timeout = timeout;
    timer.start();
}

// void HC_SR04::fall_event() {
//     this->timer.stop();
//     // this->distance = duration_cast<microseconds>(timer.elapsed_time()).count() / 58.0;
//     // this->timer.reset();
//     this->rdy = true;
// }

// void HC_SR04::rise_event() {
//     timer.start();
// }


float HC_SR04::get_distance() {
    return this->distance;
}

// void HC_SR04::update_event() {
    
//     if(rdy){
//         this->distance = duration_cast<microseconds>(timer.elapsed_time()).count() / 58.0;
//         this->timer.reset();
//         this->rdy = false;
//     }

// }

void HC_SR04::startT()
{ 
    if(timer.elapsed_time() > std::chrono::milliseconds(600))
    {
        timer.reset ();
    }
    start = timer.elapsed_time().count();
}

void HC_SR04::updateDist()
{
    end = timer.elapsed_time().count();
    done = 1;
    distance = (end - start)/6;       
    tout.detach();
    tout.attach(callback(this,&HC_SR04::startTrig), std::chrono::milliseconds(static_cast<int>(updateSpeed)));
}

void HC_SR04::startTrig(void){
    tout.detach();
    trigger_out=1;             
    wait_us(10);        
    done = 0;            
    interrupt_echo.rise(callback(this,&HC_SR04::startT));
    interrupt_echo.fall(callback(this,&HC_SR04::updateDist));
    interrupt_echo.enable_irq ();
    tout.attach(callback(this,&HC_SR04::startTrig), std::chrono::milliseconds(static_cast<int>(timeout)));

    trigger_out=0;
}

int HC_SR04::getCurrentDistance(void){
    return distance;
}

void HC_SR04::pauseUpdates(void){
    tout.detach();
    interrupt_echo.rise(NULL);
    interrupt_echo.fall(NULL);
}

void HC_SR04::startUpdates(void){
    startTrig();
}

void HC_SR04::attachOnUpdate(void callback(int)){
    _onUpdateMethod = callback;
}

void HC_SR04::changeUpdateSpeed(float updateSpeed){
    updateSpeed = updateSpeed;
}

float HC_SR04::getUpdateSpeed(){
    return updateSpeed;
}

int HC_SR04::isUpdated(void){
    d=done;
    done = 0;
    return d;
}

void HC_SR04::checkDistance(void){
    if(isUpdated())
    {
        (*_onUpdateMethod)(distance);
    }
}