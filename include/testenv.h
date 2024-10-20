#include <Arduino.h>
#include "input.h"
#include "sequencer.h"
#include "zipline.h"
#include "LED.h"

#define GREEN_LED_PIN 8
#define RED_LED_PIN 7

int metersToSteps(float meters){
    return M_STEPS_PER_REV*(meters/M_WHEEL_CIRCUMFERENCE);
}

float stepsToMeters(int steps){
    return M_WHEEL_CIRCUMFERENCE*(steps/M_STEPS_PER_REV);
}

Input estop(PIN_ESTOP, true, true);
Input button1(PIN_BTN_1, false, true);
Input button2(PIN_BTN_2, false, true);
AccelStepper motor(AccelStepper::MotorInterfaceType::DRIVER, PIN_MOTOR_STEP, PIN_MOTOR_DIR);

LED led(RED_LED_PIN);
const bool morsePattern[] = {0, 1, 0};
const bool sosPattern[] = {0,0,0,1,1,1,0,0,0};
int length = 3;

float speed = 2.0;

void button1ActivationHandler(){
    motor.setSpeed(-metersToSteps(speed));
}

void button1DeactivationHandler(){
    motor.setSpeed(0);
}

void button2ActivationHandler(){
    motor.setSpeed(metersToSteps(speed));
}

void button2DeactivationHandler(){
    motor.setSpeed(0);
}

void estopActivationHandler(){
    motor.disableOutputs();
}

void setup() {
    Serial.begin(115200);

    button1.setActivationHandler(button1ActivationHandler);
    button1.setDeactivationHandler(button1DeactivationHandler);
    button1.limitRate(20);

    button2.setActivationHandler(button2ActivationHandler);
    button2.setDeactivationHandler(button2DeactivationHandler);
    button2.limitRate(20);

    motor.setMaxSpeed(metersToSteps(ZL_V_MAX));
    motor.setSpeed(metersToSteps(0));

}

void loop() {
    button1.poll();
    button2.poll();

    led.poll();
    motor.runSpeed();
}
