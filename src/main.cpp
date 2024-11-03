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

int errorCounter = 0;
bool errorState = true;

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


/*---------------------------------------------------
Wrappers for bound events (for the lack of std::bind)
---------------------------------------------------*/
void event_ledGreenRunning(){ledGreen.blink();}
void event_ledGreenIdle(){ledGreen.on();}
void event_launchZipline(){zipline.launch();}


/*----------------------
Effect sequences
------------------------*/
long zlRunupDelay = ZL_RUNUP_DELAY;
long timeToReturn = ZL_WAIT_TO_RETURN;
long lightbarrierLaunchDelay = LB_LAUNCH_DELAY;

BEGIN_SEQUENCE(effectSequenceLightbarrier)
    DEFINE_EVENT_NO_PARAM(event_ledGreenRunning)
    EVENT_DELAY(&lightbarrierLaunchDelay)
    DEFINE_EVENT_NO_PARAM(audioPlay)
    DEFINE_EVENT_NO_PARAM(event_launchZipline)
    EVENT_DELAY(&zlRunupDelay)
    DEFINE_EVENT_NO_PARAM(activateSocket2)
    DEFINE_EVENT_NO_PARAM(deactivateSocket1)
    EVENT_DELAY(&timeToReturn)
    DEFINE_EVENT_NO_PARAM(activateSocket1)
    DEFINE_EVENT_NO_PARAM(deactivateSocket2)
    DEFINE_EVENT_NO_PARAM(audioStop)
    DEFINE_EVENT_NO_PARAM(event_ledGreenIdle)
END_SEQUENCE
const int seqLenLightbarrier = sizeof(effectSequenceLightbarrier)/sizeof(Event);

BEGIN_SEQUENCE(effectSequenceButton)
    DEFINE_EVENT_NO_PARAM(event_ledGreenRunning)
    DEFINE_EVENT_NO_PARAM(audioPlay)
    DEFINE_EVENT_NO_PARAM(event_launchZipline)
    EVENT_DELAY(&zlRunupDelay)
    DEFINE_EVENT_NO_PARAM(activateSocket2)
    DEFINE_EVENT_NO_PARAM(deactivateSocket1)
    EVENT_DELAY(&timeToReturn)
    DEFINE_EVENT_NO_PARAM(activateSocket1)
    DEFINE_EVENT_NO_PARAM(deactivateSocket2)
    DEFINE_EVENT_NO_PARAM(audioStop)
    DEFINE_EVENT_NO_PARAM(event_ledGreenIdle)
END_SEQUENCE
const int seqLenButton = sizeof(effectSequenceButton)/sizeof(Event);

/*--------------------
Input handler functions
----------------------*/
void handler_button1(){
    if(errorState) return;

    effectSequencer.stop();

    audioStop();

    activateSocket1();
    deactivateSocket2();

    zipline.returnHome();

    ledGreen.on();

    DEBUG_MSG("Button 1 pressed")
}

void handler_button2(){
    if(errorState) return;

    effectSequencer.setSequence(&effectSequenceButton[0], seqLenButton);
    effectSequencer.start();

    DEBUG_MSG("Button 2 pressed");
}

void handler_estopActivation(){

    zipline.estop();
    effectSequencer.stop();

    activateSocket1();
    deactivateSocket2();

    audioStop();

    ledRed.errCode(&estopError[0], 3);
    ledGreen.on();

    errorState = true;

    DEBUG_MSG("E-stop pressed")
}

void handler_estopRelease(){
    zipline.estopRelease();

    ledRed.off();

    activateSocket1();
    deactivateSocket2();

    errorState = false;

    DEBUG_MSG("E-stop released")
}

void handler_lightbarrier1(){
    if(errorState) return;

    if(effectSequencer.isRunning()) return;
    
    effectSequencer.setSequence(&effectSequenceLightbarrier[0], seqLenLightbarrier);
    effectSequencer.start();

    DEBUG_MSG("Light barrier 1 activated")
}

void handler_lightbarrier2(){
    zipline.stop();

    DEBUG_MSG("Light barrier 2 activated")
}

void handler_lightbarrier2Misaligned(){
    zipline.estop();
    effectSequencer.stop();
    deactivateSocket1();
    deactivateSocket2();
    audioStop();
    ledGreen.on();
    ledRed.errCode(&LBMisaligned[0], 3);

    errorState = true;

    DEBUG_MSG("ERROR: Light barrier two misaligned")
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

    errorCounter ++;
    errorState = true;
}


/*--------------------
Misc. functions
----------------------*/
void configureInputs(){
    button1.setActivationHandler(handler_button1);
    button2.setActivationHandler(handler_button2);
    estop.setActivationHandler(handler_estopActivation);
    estop.setDeactivationHandler(handler_estopRelease);
    lightbarrier1.setActivationHandler(handler_lightbarrier1);
    lightbarrier2.setActivationHandler(handler_lightbarrier2);
    lightbarrier2.setLongpressHandler(handler_lightbarrier2Misaligned);
    zipline.setAlarmCb(handler_motorAlarm);

    button1.limitRate(INPUT_READ_INTERVAL);
    button2.limitRate(INPUT_READ_INTERVAL);
    estop.limitRate(INPUT_READ_INTERVAL);
    lightbarrier1.limitRate(INPUT_READ_INTERVAL);
    lightbarrier2.limitRate(INPUT_READ_INTERVAL);

    lightbarrier2.setLongpressTime(LB2_MISALIGNED_TIMEOUT);
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
    #ifdef DEBUG
    Serial.begin(115200);
    #endif

    configureInputs();
    configureOutputs();

    activateSocket1();
    deactivateSocket2();

    configureMP3();

    handler_estopActivation();
}

#ifdef DEBUG_MEASURE_LOOP_TIME
unsigned int loopIterationsCnt = 0;
unsigned long lastLoopSpeedMeasurement = 0;
#endif

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

    // Max number of errors reached, lock entire system and just update the error led.
    if(errorCounter == MAX_ERROR_COUNT){

        handler_estopActivation();

        ledRed.errCode(&maxErrCountLockout[0], 3);

        while(true) ledRed.poll();

    }

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
