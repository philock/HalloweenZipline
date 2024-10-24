#include "zipline.h"

Zipline::Zipline() :
motor(AccelStepper::MotorInterfaceType::DRIVER, PIN_MOTOR_STEP, PIN_MOTOR_DIR),
endstopFront(PIN_ES_FRONT, false, false),
endstopBack(PIN_ES_BACK, false, false),
alarm(PIN_MOTOR_ALM, true, false)
{
    motor.setAcceleration(metersToSteps(ZL_ACCELERATION));
    motor.setPinsInverted(true, false, false);

    alarm.limitRate(INPUT_READ_INTERVAL);
    alarm.setLongpressTime(MOTOR_ALARM_REGISTRATION_TIME);

    _state = ZLState::ESTOP;
}

void Zipline::setAlarmCb(CallbackFunction f){
    _cbAlarm = f;
}

void Zipline::poll(){
    switch (_state){
    case ZLState::IDLE:
        break;

    case ZLState::LAUNCH:
        stateLaunch();
        break;

    case ZLState::RETURN:
        stateReturn();
        break;

    case ZLState::PAUSE:
        statePause();
        break;

    case ZLState::LAUNCH_DELAY:
        stateLaunchDelay();
        break;

    case ZLState::HOMING:
        stateHoming();
        break;

    case ZLState::ESTOP:
        break;
    }

    if(alarm.readLongpress()){
        estop();
        _cbAlarm(AlarmType::MOTOR_ALARM);
    }
}

/*-------------------------
FSM state functions
-------------------------*/
void Zipline::stateLaunch(){
    motor.run();

    if(motor.distanceToGo() == 0 ){
        pause();
    }

    if(endstopFront.read()){
        estop();
        _cbAlarm(AlarmType::ZL_OVER_LIMITS);

        DEBUG_MSG("Error: Motor reached endstop while in FSM launch state")
    }
}

void Zipline::stateReturn(){
    motor.run();

    if(motor.distanceToGo() == 0 || endstopBack.read()){
        
        if(endstopBack.read()){
            _state = ZLState::IDLE;

        }
        else{
            estop();
            _cbAlarm(AlarmType::ES_NOT_REACHED_ON_RETURN);

        }
    }
}

void Zipline::statePause(){
    if(millis() > _pauseEnd){
        returnHome();
    }
}

void Zipline::stateLaunchDelay(){
    if(millis() > _pauseEnd){
        launch();
    }
}

void Zipline::stateHoming(){
    motor.runSpeed();

    if(endstopBack.read() || endstopFront.read()){
        motor.setSpeed(0);
        motor.setCurrentPosition(metersToSteps(ZL_ENDSTOP_OFFSET));

        _state = ZLState::IDLE;
    }
}

/*-------------------------
FSM state entry functions
-------------------------*/
void Zipline::estop(){
    // hard stop motor
    motor.setMaxSpeed(0);
    motor.moveTo(motor.currentPosition());

    _state = ZLState::ESTOP;
}

void Zipline::estopRelease(){
    if(_state != ZLState::ESTOP) return;

    homing();
}

void Zipline::homing(){
    motor.setMaxSpeed(metersToSteps(ZL_V_MAX));
    motor.setSpeed(-metersToSteps(ZL_V_HOMING));

    _state = ZLState::HOMING;

    DEBUG_MSG("Zipline FSM state entry: homing")
}

void Zipline::launch(){
    if(_state != ZLState::IDLE) return;

    motor.setMaxSpeed(metersToSteps(ZL_V_MAX));
    motor.moveTo(metersToSteps(ZL_STOP_POS));

    _state = ZLState::LAUNCH;

    DEBUG_MSG("Zipline FSM state entry: launch")
}

void Zipline::launchDelay(int delay){
    if(_state != ZLState::IDLE) return;

    _pauseEnd = millis() + delay;

    _state = ZLState::LAUNCH_DELAY;

    DEBUG_MSG("Zipline FSM state entry: launch delay")
}

void Zipline::returnHome(){
    motor.setMaxSpeed(metersToSteps(ZL_V_RETURN));
    motor.moveTo(0);

    _state = ZLState::RETURN;

    DEBUG_MSG("Zipline FSM state entry: return")
}

void Zipline::pause(){
    _pauseEnd = millis() + ZL_WAIT_TO_RETURN;

    _state = ZLState::PAUSE;

    DEBUG_MSG("Zipline FSM state entry: pause")
}

void Zipline::stop(){
    motor.stop();
}

/*-------------------------
utility functions
-------------------------*/
int Zipline::metersToSteps(float meters){
    return M_STEPS_PER_REV*(meters/M_WHEEL_CIRCUMFERENCE);
}

float Zipline::stepsToMeters(int steps){
    return M_WHEEL_CIRCUMFERENCE*(steps/M_STEPS_PER_REV);
}