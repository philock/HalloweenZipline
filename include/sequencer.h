#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <Arduino.h>
#include "configuration.h"

// Todo: Make Event array const in macros and Sequencer class.

// Alternative implementation: use virtual functions. A sequence is an array of Event*. The base class 'Event' has a virtual execute() method. The 'Event' class is 
// specialized into delay or class that redirects execute to a custom function pointer.  

struct Event{
    void (*func)(void*);
    void *param;
};

enum SpecialEvents{
    DELAY,
    REPEAT,
    END
};

class Sequencer{
    public:
        Sequencer(Event *sequence, int sequenceLength);
        Sequencer();

        void setSequence(Event *sequence, int sequenceLength);

        // starts the sequence at the beginning
        void start();

        // stops sequence and goes back to the start
        void stop();

        // called every cycle 
        void poll();

    private:
        Event *_sequence;
        int _sequenceLength;

        int _step = 0;
        bool _running = false;

        bool _timerRunning;
        unsigned long _timerStart;

        bool timer(long delay);
};

#define BEGIN_SEQUENCE(name) \
Event name[] = {

#define DEFINE_EVENT(func, param) \
{ func, param },

#define DEFINE_EVENT_NO_PARAM(func) \
{ reinterpret_cast<void(*)(void*)>(func), nullptr },

#define EVENT_REPEAT \
{ reinterpret_cast<void(*)(void*)>(SpecialEvents::REPEAT), nullptr },

#define EVENT_DELAY(milliseconds) \
{ reinterpret_cast<void(*)(void*)>(SpecialEvents::DELAY), milliseconds },

#define END_SEQUENCE \
{ reinterpret_cast<void(*)(void*)>(SpecialEvents::END), nullptr }};


#endif