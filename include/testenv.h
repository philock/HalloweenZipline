#include <Arduino.h>
#include "input.h"
#include "sequencer.h"
#include "zipline.h"
#include "LED.h"

#define BUTTON_PIN A0
#define SWITCH_PIN A1
#define GREEN_LED_PIN 8
#define RED_LED_PIN 7

Input button(BUTTON_PIN, false, true);
Input slideswitch(SWITCH_PIN, false, true);

LED led(RED_LED_PIN);
const bool morsePattern[] = {0, 1, 0};
const bool sosPattern[] = {0,0,0,1,1,1,0,0,0};
int length = 3;

void buttonActivationHandler(){
    led.errCode(&sosPattern[0], 9);
}

void buttonDeactivationHandler(){

}

void slideswitchActivationHandler(){
    led.errCode(&morsePattern[0], length);
}

void slideswitchDeactivationHandler(){
    led.off();
}


void setup() {
    Serial.begin(115200);

    button.setActivationHandler(buttonActivationHandler);
    button.setDeactivationHandler(buttonDeactivationHandler);
    button.limitRate(5);

    slideswitch.setActivationHandler(slideswitchActivationHandler);
    slideswitch.setDeactivationHandler(slideswitchDeactivationHandler);
    slideswitch.limitRate(5);
}

void loop() {
    button.poll();
    slideswitch.poll();

    led.poll();
}
