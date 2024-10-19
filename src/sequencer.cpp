#include "sequencer.h"

Sequencer::Sequencer(Event *sequence, int sequenceLength){
    _sequence = sequence;
    _sequenceLength = sequenceLength;
}

Sequencer::Sequencer(){

}

void Sequencer::setSequence(Event *sequence, int sequenceLength){
    _sequence = sequence;
    _sequenceLength = sequenceLength;
    _step = 0;
}

void Sequencer::start(){
    _running = true;

    DEBUG_MSG("Sequencer: start")
}

void Sequencer::stop(){
    _step = 0;
    _running = false;

    DEBUG_MSG("Sequencer: stop")
}

void Sequencer::poll(){
    if(_running){
        if(_sequence[_step].func == reinterpret_cast<void(*)(void*)>(SpecialEvents::DELAY)){
            if(timer(*(long*)_sequence[_step].param)) _step ++;

            if(_step >= _sequenceLength) stop();
            return;

        }
        else if(_sequence[_step].func == reinterpret_cast<void(*)(void*)>(SpecialEvents::END)){
            stop();
            return;

        }
        else if(_sequence[_step].func == reinterpret_cast<void(*)(void*)>(SpecialEvents::REPEAT)){
            _step = 0;
            return;

        }
        else{
            _sequence[_step].func(_sequence[_step].param);
            _step ++;

            if(_step >= _sequenceLength) stop();
            return;

        }
    }
}

bool Sequencer::timer(long delay){
    if(_timerRunning){
        if(millis() > _timerStart + delay){
            _timerRunning = false;
            return true;

        } 

    }
    else{
        _timerStart = millis();
        _timerRunning = true;

    }

    return false;
}