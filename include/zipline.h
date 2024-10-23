#ifndef ZIPLINE_H
#define ZIPLINE_H

#include <Arduino.h>
#include <AccelStepper.h>
#include "configuration.h"
#include "input.h"

enum AlarmType{
    ZL_OVER_LIMITS,
    MOTOR_ALARM,
    ES_NOT_REACHED_ON_RETURN
};

class Zipline{
    private:
        AccelStepper motor;
        Input endstopFront;
        Input endstopBack;
        Input alarm;

        typedef void (*CallbackFunction)(AlarmType alm);
        CallbackFunction _cbAlarm;

        enum ZLState{
            IDLE,
            LAUNCH,
            RETURN,
            PAUSE,
            LAUNCH_DELAY,
            HOMING,
            ESTOP
        };

        ZLState _state;

        unsigned long _pauseEnd; 

        // utility functions
        int metersToSteps(float meters);
        float stepsToMeters(int steps);

        // FSM state functions
        void stateLaunch();
        void stateReturn();
        void statePause();
        void stateLaunchDelay();
        void stateHoming();

        // internal state entry functions
        void returnHome();
        void pause();

    public:
        Zipline();

        void setAlarmCb(CallbackFunction f);

        // external state entry functions
        void homing();
        void launch();
        void launchDelay();
        void estop();
        void estopRelease();

        // called when second light barrier is triggered
        // to decelerate the motor to a stop 
        void stop();

        // cyclic FSM update function, called on every loop
        void poll();

};

#endif