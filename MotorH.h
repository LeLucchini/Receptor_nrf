#ifndef MOTORH_H
#define MOTORH_H

#include "mbed.h"
#include "HC_SR04.h"

class motorH
{
    public:
        motorH(void);
        /**iniates the class with the specified trigger pin, echo pin, update speed and timeout**/
        void motorDir(int command, float duty);
        /**iniates the class with the specified trigger pin, echo pin, update speed, timeout and method to call when the distance changes**/
        void motorEsq(int command, float duty);
        /**pauses measuring the distance**/
        void moveForward(int dist);
        void moveBackwards(int dist);
        void turnLeft(void);
        void turnRight(void);
        void stop(void);
        void hardStop(void);
        void execute(char rxData[4]);
        int pulsoEsq = 0;
        int pulsoDir = 0;
        void debug(void);
        bool moving = false;
        BufferedSerial * serial;
    private:
        void countEsqIrq(void);
        void countDirIrq(void);
        void distIrq();
        void evadeMove();
        int debouceCounter = 0;
        PwmOut _in1;
        DigitalOut _in2;
        PwmOut _in3;
        DigitalOut _in4;
        //BufferedSerial _serial;
        InterruptIn _encoderEsq;
        InterruptIn _encoderDir;
        Ticker _tout;
        HC_SR04 sensor_ultrassom;
};
#endif