#include "input.h"

Input::Input(int pin, bool activeHigh, bool inputPullup){
    if(inputPullup) pinMode(pin, INPUT_PULLUP);
    else            pinMode(pin, INPUT);    
    
    _activeHigh = activeHigh;
    _pin        = pin;
}

void Input::limitRate(int rateMS){
    if(rateMS != 0){
        _isRateLimited = true;
        _rateLimitMS   = rateMS;

    }
    else{
        _isRateLimited = false;

    }
}

void Input::setDebounceTime(int tMS){
    _debounceTimeMS = tMS;
}

void Input::setActivationHandler(CallbackFunction f){
    _activationHandler = f;
}

void Input::setDeactivationHandler(CallbackFunction f){
    _deactivationHandler = f;
}

bool Input::read(){
    if(_isRateLimited){

        if(millis() > _tNextRead){
            _tNextRead = millis() + _rateLimitMS;

            // invert input if active low
            bool state = _activeHigh == digitalRead(_pin);

            bool debounceDelay = millis() > _tLastChange + _debounceTimeMS;

            if(debounceDelay){
                _isActive = state;
                _tLastChange = millis();

            }
        }

    }
    else{
        // invert input if active low
        bool state = _activeHigh == digitalRead(_pin);

        bool debounceDelay = millis() > _tLastChange + _debounceTimeMS;

        if(debounceDelay){
            _isActive = state;
            _tLastChange = millis();

        }
    }

    return _isActive;
}

void Input::poll(){
    if(_isRateLimited){

        if(millis() > _tNextRead){
            _tNextRead = millis() + _rateLimitMS;
            handleInput();

        }

    }
    else{
        handleInput();

    }
}

void Input::handleInput(){
    // invert input if active low
    bool state = _activeHigh == digitalRead(_pin);

    bool debounceDelay = millis() > _tLastChange + _debounceTimeMS;

    // rising edge detected, call handler function
    if(debounceDelay && _isActive == false && state == true){
        
        _isActive = true;
        _tLastChange = millis();

        if(_activationHandler) _activationHandler();

        return;
    }

    // falling edge detected, call handler function
    if(debounceDelay && _isActive == true && state == false){

        _isActive = false;
        _tLastChange = millis();

        if(_deactivationHandler) _deactivationHandler();
    }
}