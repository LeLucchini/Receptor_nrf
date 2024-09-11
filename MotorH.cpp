
#include "MotorH.h"
#include <cstdint>
#include <cstdio>
#include "mbed.h"
#include <cstring>


#define H_IN1 PTC8 // ponte H
#define H_IN2 PTB9
#define H_IN3 PTC9
#define H_IN4 PTB11

#define PWM_FREQ 0.01f //frequencia do pwn 1kHz
#define velDir 0.00568f
#define velEsq 0.0068f

#define ENC_ESQ_PIN PTA5 // encoders
#define ENC_DIR_PIN PTA4


#define LEFT_TURN_DISTANCE 10
#define RIGHT_TURN_DISTANCE 10


motorH::motorH(void):_in1(H_IN1),_in2(H_IN2),_in3(H_IN3),_in4(H_IN4)
    ,_encoderEsq(ENC_ESQ_PIN),_encoderDir(ENC_DIR_PIN)
{
    _in1.period(PWM_FREQ); // seta frequencia do pwm
    _in2 = 0;
    _in3.period(PWM_FREQ);
    _in4 = 0;
    
    _encoderEsq.rise(callback(this, &motorH::countEsqIrq)); // interrupção que atualiza encoder
    _encoderDir.rise(callback(this, &motorH::countDirIrq));
   
}


void motorH::moveForward(int dist)
{
    //_serial.write("\n\rmovendo para frente", 31);
    serial->write("dentroFo", 8);
    pulsoDir = 0;
    while(pulsoDir < dist)
    {        
        moving = true;

        motorDir(1, velDir); 
        motorEsq(1, velEsq);
    }
    stop();
}
void motorH::moveBackwards(int dist)
{
    //_serial.write("\n\rmovendo para tras", 21);    
    pulsoDir = 0;
    while(pulsoDir < dist)
    {
        moving = true;
        motorDir(-1, velDir);
        motorEsq(-1, velEsq);
    }
    stop();
}

void motorH::stop(void)
{
    moving = false;
    motorDir(0, 0); // inicia os motores em ponto morto
    motorEsq(0, 0); // inicia os motores em ponto morto
    serial->write("stop\r\n", 4);
   
}

void motorH::hardStop(void)
{
    moving = false;
    motorDir(2, 0); // trava os motores
    motorEsq(2, 0); // trava os motores    
}

void motorH::turnLeft(void)
{
    stop();
    ThisThread::sleep_for(1s);
    //_serial.write("\n\rvirar para esquerda", 5);
    pulsoDir = 0;
    while(pulsoDir < LEFT_TURN_DISTANCE)
    {
        moving = true;

        motorDir(1, velDir);
        motorEsq(-1, velEsq);
    }
    stop();
    ThisThread::sleep_for(1s);
}

void motorH::turnRight(void)
{
    stop();
    ThisThread::sleep_for(1s);
    //_serial.write("\n\rvirar para direita", 8);
    pulsoDir = 0;
    while(pulsoDir < RIGHT_TURN_DISTANCE)
    {
        moving = true;

        motorDir(-1, velDir);
        motorEsq(1, velEsq);
    }
    stop();
    ThisThread::sleep_for(1s);
}   

void motorH::motorDir(int command, float duty)
{
    switch(command) 
    {
    case 0: // ponto morto
        _in1.write(0);
        _in2.write(0);
        break;
    case 1: // move forward
        _in1.write(duty);
        _in2.write(0);
        break;
    case -1: // move backwards
        _in1.write(duty);
        _in2.write(0);
        break;
    case 2: // hard break
        _in1.write(1.0f);
        _in2.write(1.0f);
        break;
    }
}

void motorH::motorEsq(int command, float duty)
{
    switch(command) 
    {
    case 0: // ponto morto
        _in3.write(0);
        _in4.write(0);
        break;
    case 1: // move forward
        _in3.write(duty);
        _in4.write(0);
        break;
    case -1: // move backwards
        _in3.write(duty);
        _in4.write(0);
        break;
    case 2: // hard break
        _in3.write(1.0f);
        _in4.write(1.0f);
        break;
    }
}

void motorH::execute(char rxData[4])
{
    
    int dist_x, dist_y;
    char dist_char_x[1], dist_char_y[1];

    
    dist_char_x[0] = rxData[1];

    dist_char_y[0] = rxData[3];

    dist_x = atoi(dist_char_x);
    dist_y = atoi(dist_char_y);

    //serial->write(rxData, 4);

    char string_10[100];

    sprintf(string_10, "dist_x: %d\r\n", dist_x); 
    serial->write(string_10, strlen(string_10));

    sprintf(string_10, "dist_y: %d", dist_y); 
    serial->write(string_10, strlen(string_10));

    // frente esquerda
    if (rxData[0] == 'f' && rxData[2] == 'd')
    {
        
        //_serial.write("\n\rcomando frente esquerda", 9);
        if (dist_x > 0) {moveForward(dist_x);}
        if (dist_y > 0) {turnLeft();moveForward(dist_y);}
    }
    // frente direita
    if (rxData[0] == 'f' && rxData[2] == 't')
    {
        //_serial.write("\n\rcomando frente direita", 6);
        if (dist_x > 0) {moveForward(dist_x);}
        if (dist_y > 0) {turnRight();moveForward(dist_y);}
    }
    // trás esquerda
    if (rxData[0] == 't' && rxData[2] == 'f')
    {
        //_serial.write("\n\rcomando trás esquerda", 4);
        if (dist_x > 0) {moveBackwards(dist_x);}
        if (dist_y > 0) {turnLeft();moveForward(dist_y);}
    }
    // trás direita
    if (rxData[0] == 't' && rxData[2] == 't')
    {
        //_serial.write("\n\rcomando trás direita", 10);
        if (dist_x > 0) {moveBackwards(dist_x);}
        if (dist_y > 0) {turnRight();moveForward(dist_y);}
    }
    // 11111111 panic stop
    if (rxData[0] == '1' && rxData[2] == '1')
    {
       // _serial.write("\n\rcomando parada imediata", 111);
        stop();
    }    
}

void motorH::countEsqIrq(void)
{
    pulsoEsq++;
}

void motorH::countDirIrq(void)
{
    pulsoDir++;    
}

void motorH::debug(void)  // debug function
{
    //_serial.write("\n\resq:%d  dist:%dmm",pulsoEsq);
    //_serial.write("\n\rdir%d   dist:%dmm",pulsoDir);
}
