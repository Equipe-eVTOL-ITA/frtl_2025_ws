#pragma once // diz ao compilador para incluir este código apenas uma vez
#include <chrono>

using namespace std;
using Clock = chrono::high_resolution_clock;
typedef Clock::time_point Tempo;

class PIDController {
public:
    PIDController(float kp, float ki, float kd, float setpoint, float min_dt=0.1){
        this->kp_ = kp;
        this->ki_ = ki;
        this->kd_ = kd;

        this->setpoint_ = setpoint;
        this->min_dt_ = min_dt;

        this->last_time_ = Clock::now();
    }

    float compute(float current_value) {
        Tempo current_time = Clock::now();
        chrono::duration<float> elapsed = current_time - this->last_time_;
        float dt = elapsed.count();
        
        if(dt >= this->min_dt_){
            float error = this->setpoint_ - current_value;

            this->integral_ += error*dt;
            float derivative = (error - this->last_error_) / dt;

            float output = this->kp_*error +
                            this->ki_*integral_ +
                            this->kd_*derivative;
            
            this->last_error_ = error;
            this->last_time_ = current_time;

            return output;
        }

        return 0.0; // isso aqui pode fazer o drone travar??
    }

    void tune(float kp, float ki, float kd){
        this->kp_ = kp;
        this->ki_ = ki;
        this->kd_ = kd;
    }

    void setSetpoint(float setpoint){
        this->setpoint_ = setpoint;
    }

    void reset(){
        this->integral_ = 0.0;
        this->last_error_ = 0.0;
    }

private:
    float kp_, ki_, kd_;

    float setpoint_;
    float min_dt_;

    float integral_ = 0.0;
    float last_error_ = 0.0;
    Tempo last_time_;
};