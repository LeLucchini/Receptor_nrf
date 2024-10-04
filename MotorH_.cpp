#include "MotorH_.h"
#include <chrono>
#include <cstdlib>
#include <memory>

MotorH::MotorH():leftDriver1(H_IN1), leftDriver2(H_IN2), rightDriver1(H_IN3), rightDriver2(H_IN4), leftEncoder(PTA5), rightEncoder(ENC_DIR_PIN), last_update_left_ms(Kernel::Clock::now()), last_update_right_ms(Kernel::Clock::now()) {
    this->leftDriver1.period(PWM_PERIOD);
    this->rightDriver1.period(PWM_PERIOD);
    this->leftEncoder.rise(callback(this, &MotorH::left_encoder_event));
    this->rightEncoder.rise(callback(this, &MotorH::right_encoder_event));

}

void MotorH::left_encoder_event() {
    update_left_count = true;
}

void MotorH::right_encoder_event() {
    update_right_count = true;
}

void MotorH::setLeftVel(float vel) {
    this->leftDriver1 = vel;
    this->leftDriver2 = 0; 
}

void MotorH::setRightVel(float vel) {
    this->rightDriver1 = vel;
    this->rightDriver2 = 0;
}

void MotorH::update() {

    int delta_time_left = duration_cast<milliseconds> (Kernel::Clock::now() - this->last_update_left_ms ).count();
    int delta_time_right = duration_cast<milliseconds> (Kernel::Clock::now() - this->last_update_right_ms).count();

    if (this->update_left_count && delta_time_left >= DEBOUNCE_PERIOD) {
        this->last_update_left_ms = Kernel::Clock::now();
        update_left_count = false;
        this->left_count += 1;
    } else this->update_left_count = false;

    if (this->update_right_count && delta_time_right >= DEBOUNCE_PERIOD) {
        this->last_update_right_ms = Kernel::Clock::now();
        update_right_count = false;
        this->right_count += 1;
    } else this->update_right_count = false;
    debug();
}

int MotorH::cm_to_ticks(float dist) {
    return static_cast<int>((dist / (2 * PI * W_RADIUS)) * 5);
}

void MotorH::stop(int time_ms) {
    this->currentCommand = STOP;
    this->commandDrivers();
    wait_us(1000 * time_ms);
}

void MotorH::turnRight(int angle) {
    this->currentCommand = RIGHT;
    int target_ticks = this->cm_to_ticks((angle * (2 * PI / 360.0)) * W_WHEELS);
    this->left_count = 0;
    this->right_count = 0;

    while (this->left_count < target_ticks) {
        this->update();
        this->commandDrivers();
    }

    this->stop(500);
}

void MotorH::turnLeft(int angle) {
    this->currentCommand = LEFT;
    int target_ticks = this->cm_to_ticks((angle * (2 * PI / 360.0)) * W_WHEELS);
    this->left_count = 0;
    this->right_count = 0;

    while (this->right_count < target_ticks) {
        this->update();
        this->commandDrivers();
    }

    this->stop(500);
}

void MotorH::forward(int dist) {
    this->currentCommand = FORWARD;
    int target_ticks = cm_to_ticks(dist);
    this->left_count = 0;
    this->right_count = 0;
    while (this->left_count < target_ticks) {
        this->update();
        this->commandDrivers();
        int temp_count_left = this->left_count;
        int temp_count_right = this->right_count;

        resourceManagment.lock();
        bool need_dodge = this->distance <= MIN_OBJECT_DIST && this->isDodging == false;
        resourceManagment.unlock();
        if (need_dodge) {
            this->isDodging = true;
            this->dodge();
            this->currentCommand = FORWARD;
            this->left_count = temp_count_left + cm_to_ticks(50);
            this->right_count = temp_count_right + cm_to_ticks(50);
        }
    }

    this->stop(500);

}

void MotorH::commandDrivers() {
    switch(this->currentCommand) {
        case FORWARD:
            this->setLeftVel(velEsq);
            this->setRightVel(velDir);
        break;
        case LEFT:
            this->setLeftVel(0);
            this->setRightVel(velDir);
        break;
        case RIGHT:
            this->setLeftVel(velEsq);
            this->setRightVel(0);
        break;
        case STOP:
            this->setLeftVel(0);
            this->setRightVel(0);
        break;
    }
}

void MotorH::evalSR04() {
    while(this->isBackgroundRunning) {
        int new_distance = this->ultrassom.getDistance();
        resourceManagment.lock();
        this->distance = new_distance;
        resourceManagment.unlock();
        ThisThread::sleep_for(500ms);
    }
}

void MotorH::startBackgroundService() {
    this->isBackgroundRunning = true;
    this->backgroundThread.start(callback(this, &MotorH::evalSR04));
}

void MotorH::stopBackgroundService() {
    this->isBackgroundRunning = true;
    this->backgroundThread.join();
}

void MotorH::execute(char input[8]) {
    char op1 = input[0];
    char op2 = input[4];

    char num1[] = {input[1], input[2], input[3], '\0'};
    char num2[] = {input[5], input[6], input[7], '\0'};

    int dist1 = atoi(num1);
    int dist2 = atoi(num2);

    switch(op1) {
        case 'f':
        this->forward(dist1);
        break;
        case 'd':
        this->turnRight(90);
        this->forward(dist1);
        break;
        case 'e':
        this->turnLeft(90);
        this->forward(dist1);
        break;
        default:
        stop(1000 * dist1);
    }

    switch(op2) {
        case 'f':
        this->forward(dist2);
        break;
        case 'd':
        this->turnRight(90);
        this->forward(dist2);
        break;
        case 'e':
        this->turnLeft(90);
        this->forward(dist2);
        break;
        default:
        stop(1000 * dist2);
    }
    
}

void MotorH::dodge() {
    int dist_90 = cm_to_ticks((90 * (2 * PI / 360.0)) * W_WHEELS);
    int cm_evasion = cm_to_ticks(50);

    this->stop(500);
    this->turnRight(90);
    this->forward(30);
    this->turnLeft(90);
    this->forward(30);
    this->turnLeft(90);
    this->forward(30);
    this->turnRight(90);
    this->stop(0);

    this->isDodging = false;

}

void MotorH::debug() {

}

