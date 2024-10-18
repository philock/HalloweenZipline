#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h>


#define INPUT_DEBOUNCE_MS 50


class Input{
    private:
        typedef void (*CallbackFunction)();
        CallbackFunction _activationHandler = nullptr;
        CallbackFunction _deactivationHandler = nullptr;

        int _pin;
        bool _activeHigh;

        bool _isRateLimited = false;
        int _rateLimitMS;

        int _debounceTimeMS = 50;

        unsigned long _tNextRead;
        unsigned long _tLastChange;

        bool _isActive = false;

        void handleInput();

    public:
        Input(int pin, bool activeHigh, bool inputPullup);

        void limitRate(int rateMS);
        void setDebounceTime(int tMS);

        void setActivationHandler(CallbackFunction f);
        void setDeactivationHandler(CallbackFunction f);

        bool read();

        void poll();

};

#endif