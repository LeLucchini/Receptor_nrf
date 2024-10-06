#ifndef MOTORH__H
#define MOTORH__H

#include "mbed.h"
#include "HC_SR04_2.h"

#define PI 3.14159
#define W_RADIUS 3.3 // Em cm
#define W_WHEELS 18 // Em cm

// Portas Ponte H
#define H_IN1 PTC8 
#define H_IN2 PTB9
#define H_IN3 PTC9
#define H_IN4 PTB11

//Velocidade Motores
#define PWM_PERIOD 0.01
#define velDir 0.283f //0.566f
#define velEsq 0.320f//0.68f

//Entradas Encoders
#define ENC_ESQ_PIN PTA5 // encoders
#define ENC_DIR_PIN PTA4

#define DEBOUNCE_PERIOD 300 // em ms
#define DEBOUNCE_FACTORA 1.0
#define DEBOUNCE_FACTORB 1.05
#define MIN_OBJECT_DIST 20

#define MAX_STACK_SIZE 12
//Debug

using namespace std;
using namespace std::chrono;

enum Command_Type {FORWARD, LEFT, RIGHT, STOP};

class MotorH {
public: 
    MotorH();
    void update();
    void debug();
    void setLeftVel(float vel);
    void setRightVel(float vel);
    int cm_to_ticks(float dist);

    void stop(int time_ms);
    void turnRight(int angle);
    void turnLeft(int angle);
    void forward(int dist);

    void execute(char input[8]);

    void startBackgroundService();
    void stopBackgroundService();
    
private:
    PwmOut leftDriver1;
    DigitalOut leftDriver2;
    PwmOut rightDriver1;
    DigitalOut rightDriver2;

    InterruptIn leftEncoder;
    InterruptIn rightEncoder;

    Kernel::Clock::time_point last_update_left_ms;
    Kernel::Clock::time_point last_update_right_ms;

    bool update_left_count = false;
    bool update_right_count = false;
    int left_count = 0;
    int right_count = 0;

    void left_encoder_event();
    void right_encoder_event();

    Command_Type currentCommand;
    void commandDrivers();


    HC_SR04_2 ultrassom{};
    int distance{9999};
    bool isDodging = false;
    void dodge();


    //Background Thread
    Thread backgroundThread;
    Mutex resourceManagment;
    bool isBackgroundRunning = false;
    void evalSR04();

    //Debug

    
};


#endif