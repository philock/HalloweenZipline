//#define TEST_ENV

#ifdef TEST_ENV
#include "testenv.h"
#else

#include <Arduino.h>
#include <MD_YX5300.h>
#include "configuration.h"
#include "sequencer.h"
#include "zipline.h"
#include "input.h"
#include "led.h"

Input button1(PIN_BTN_1, false, true);
Input button2(PIN_BTN_2, false, true);
Input estop(PIN_ESTOP, true, true);
Input lightbarrier1(PIN_LB_1, false, true);
Input lightbarrier2(PIN_LB_2, false, true);

Zipline zipline;
Sequencer effectSequencer;
LED ledGreen(PIN_LED_GREEN);
LED ledRed(PIN_LED_RED);
MD_YX5300 mp3(Serial1);

unsigned int loopIterationsCnt = 0;
unsigned long lastLoopSpeedMeasurement = 0;

/*--------------------
Output control functions
----------------------*/
void activateSocket1(){
    digitalWrite(PIN_SOCK_1A, LOW);
    digitalWrite(PIN_SOCK_1B, LOW);
}

void activateSocket2(){
    digitalWrite(PIN_SOCK_2A, LOW);
    digitalWrite(PIN_SOCK_2B, LOW);
}

void deactivateSocket1(){
    digitalWrite(PIN_SOCK_1A, HIGH);
    digitalWrite(PIN_SOCK_1B, HIGH);
}

void deactivateSocket2(){
    digitalWrite(PIN_SOCK_2A, HIGH);
    digitalWrite(PIN_SOCK_2B, HIGH);
}

void audioPlay(){
    mp3.playStart();
}

void audioStop(){
    mp3.playStop();
}

/*--------------------
Input handler functions
----------------------*/
void handler_button1(){
    zipline.launch();

    DEBUG_MSG("Button 1 pressed")
}

void handler_button2(){
    zipline.launch();
    effectSequencer.start();

    DEBUG_MSG("Button 2 pressed");
}

void handler_estopActivation(){
    zipline.estop();
    effectSequencer.stop();

    deactivateSocket1();
    deactivateSocket2();

    audioStop();

    ledRed.errCode(&estopError[0], 3);
    ledGreen.on();

    DEBUG_MSG("E-stop pressed")
}

void handler_estopRelease(){
    zipline.estopRelease();

    ledRed.off();

    DEBUG_MSG("E-stop released")
}

void handler_lightbarrier1(){
    zipline.launchDelay();
    effectSequencer.start();

    DEBUG_MSG("Light barrier 1 activated")
}

void handler_lightbarrier2(){
    zipline.stop();

    DEBUG_MSG("Light barrier 2 activated")
}

void handler_motorAlarm(AlarmType alm){
    effectSequencer.stop();
    deactivateSocket1();
    deactivateSocket2();
    audioStop();
    ledGreen.on();

    switch(alm){

    case AlarmType::ZL_OVER_LIMITS:

        ledRed.errCode(&ZLOverLimitsError[0], 3);

        DEBUG_MSG("Error: Motor over endstop");

        break;

    case AlarmType::MOTOR_ALARM:

        ledRed.errCode(&ZLMotorError[0], 3);

        DEBUG_MSG("Error: Motor alarm");

        break;

    case AlarmType::ES_NOT_REACHED_ON_RETURN:
    
        ledRed.errCode(&ZLEsNotReached[0], 3);

        DEBUG_MSG("Error: Endstop not reached while returning");

        break;
    }
}

// wrappers for bound events
void event_ledGreenRunning(){ledGreen.blink();}
void event_ledGreenIdle(){ledGreen.on();}

long zlRunupDelay = ZL_RUNUP_DELAY;
const int seqLen = 10;
BEGIN_SEQUENCE(effectSequence)
    DEFINE_EVENT_NO_PARAM(event_ledGreenRunning)
    EVENT_DELAY(&zlRunupDelay)
    DEFINE_EVENT_NO_PARAM(activateSocket1)
    DEFINE_EVENT_NO_PARAM(activateSocket2)
    DEFINE_EVENT_NO_PARAM(audioPlay)
    EVENT_DELAY(&zlRunupDelay)
    DEFINE_EVENT_NO_PARAM(deactivateSocket1)
    DEFINE_EVENT_NO_PARAM(deactivateSocket2)
    DEFINE_EVENT_NO_PARAM(event_ledGreenIdle)
END_SEQUENCE

void configureInputs(){
    button1.setActivationHandler(handler_button1);
    button2.setActivationHandler(handler_button2);
    estop.setActivationHandler(handler_estopActivation);
    estop.setDeactivationHandler(handler_estopRelease);
    lightbarrier1.setActivationHandler(handler_lightbarrier1);
    lightbarrier2.setActivationHandler(handler_lightbarrier2);
    zipline.setAlarmCb(handler_motorAlarm);

    button1.limitRate(INPUT_READ_INTERVAL);
    button2.limitRate(INPUT_READ_INTERVAL);
    estop.limitRate(INPUT_READ_INTERVAL);
    lightbarrier1.limitRate(INPUT_READ_INTERVAL);
    lightbarrier2.limitRate(INPUT_READ_INTERVAL);
}

void configureOutputs(){
    pinMode(PIN_SOCK_1A, OUTPUT);
    pinMode(PIN_SOCK_1B, OUTPUT);
    pinMode(PIN_SOCK_2A, OUTPUT);
    pinMode(PIN_SOCK_2B, OUTPUT);
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
}

void configureMP3(){
    Serial1.begin(MD_YX5300::SERIAL_BPS);
    mp3.begin();
    mp3.setSynchronous(true);
    mp3.playFolderRepeat(1);
    mp3.volume(mp3.volumeMax());

}

void setup() {
    Serial.begin(115200);

    configureInputs();
    configureOutputs();

    deactivateSocket1();
    deactivateSocket2();

    configureMP3();

    effectSequencer.setSequence(&effectSequence[0], seqLen);

    handler_estopActivation();
}

void loop() {
    estop.poll();
    button1.poll();
    button2.poll();
    lightbarrier1.poll();
    lightbarrier2.poll();

    effectSequencer.poll();
    zipline.poll();

    ledGreen.poll();
    ledRed.poll();

    mp3.check();

    // measure number of loop iterations per second
    #ifdef DEBUG_MEASURE_LOOP_TIME
    loopIterationsCnt ++;

    if(loopIterationsCnt == 20000){
        unsigned long iterationsPerSec = 20e6/(millis() - lastLoopSpeedMeasurement);

        Serial.print("Loop iterations per second: ");
        Serial.println(iterationsPerSec);

        loopIterationsCnt = 0;
        lastLoopSpeedMeasurement = millis();
    }
    #endif   
}

#endif
