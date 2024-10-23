#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//#define DEBUG
//#define DEBUG_MEASURE_LOOP_TIME

// Launch sequence timing properties
#define ZL_RUNUP_DELAY 400     // milliseconds
#define ZL_WAIT_TO_RETURN 5000 // milliseconds
#define LB_LAUNCH_DELAY 1000   // milliseconds

// Zipline physical properties
#define ZL_LENGTH 3.5              // meters
#define M_STEPS_PER_REV 50         // steps
#define M_WHEEL_CIRCUMFERENCE 0.12 // meters
#define ZL_STOP_POS 3.0            // meters
#define ZL_ENDSTOP_OFFSET 0.01     // meters

// Zipline movement properties
#define ZL_V_MAX 1.5      // meters/sec
#define ZL_V_RETURN 0.4   // meters/sec
#define ZL_V_HOMING 0.3   // meters/sec
#define ZL_ACCELERATION 4 // meters/sec^2

// Error codes
const bool estopError[]        = {0,0,0};
const bool ZLOverLimitsError[] = {1,0,0};
const bool ZLEsNotReached[]    = {0,1,0};
const bool ZLMotorError[]      = {0,0,1};
const bool LBMisaligned[]      = {1,0,1};

// Pin definitions
#define PIN_LED_GREEN 7
#define PIN_LED_RED 6
#define PIN_MOTOR_STEP 12
#define PIN_MOTOR_DIR 13
#define PIN_MOTOR_ALM 11
#define PIN_LB_1 43
#define PIN_LB_2 45
#define PIN_SOCK_1A 54
#define PIN_SOCK_1B 55
#define PIN_SOCK_2A 56
#define PIN_SOCK_2B 57
#define PIN_ES_BACK 52
#define PIN_ES_FRONT 53
#define PIN_BTN_1 37
#define PIN_BTN_2 35
#define PIN_ESTOP 39

// System Properties
#define INPUT_READ_INTERVAL 20            // milliseconds
#define MOTOR_ALARM_REGISTRATION_TIME 200 // milliseconds 

#ifdef DEBUG
    #define DEBUG_MSG(msg) Serial.println(msg);
#else
    #define DEBUG_MSG(msg)
#endif

#endif