#define CALLSIGN "  N5NHJ  "          // ATTENTION!: Max 9 characters
#define GRID "EM13pc"                 // ATTENTION: Max 6 characters

/* Serial Port Settings */
#define CONTROL_PORT_MAPPED_TO &Serial
#define CONTROL_PORT_BAUD_RATE 115200

/* Nextion Serial Port Settings */
#define NEXTION_PORT_MAPPED_TO &Serial3
#define NEXTION_PORT_BAUD_RATE 115200

/* OPTIONS */
// #define DIGITAL_OUTPUT
#define PWM_OUTPUT
#define SLOW_START_STOP
#define PTT_AUTOMATION
#define ONE_ANTENNA

/* OPTIONS CONFIGURATION */
#define PWM_SPEED 255
#define SLOW_START_STOP_MIN 50
#define SLOW_START_STOP_DELAY 200
#define SLOW_START_STOP_STEPS 50
#define SLOW_START_STOP_PROXIMITY 3

/* TIMING CONFIGURATION */
#define POTS_READING_RATE 100 //250         // Potentiometer reading loop timing (millisecs) Nextion timer refresh is set to 100ms
#define INFO_SENDING_RATE 1000        // Info broadcasting timing (millisecs)
#define STATUS_SENDING_RATE 250       // Info broadcasting timing (millisecs)
#define RTC_READING_RATE 1000         // RTC reading timing (millisecs)
#define PTT_CHECKING_RATE 200         // RTC reading timing (millisecs)
#define ACTION_LOOP_RATE 50           // Reaction loop timing (millisecs)
#define CONTROL_PORT_READ_RATE 500    // NOT IN USE
#define NEXTION_PORT_READ_RATE 250    // Command loop timing (millisecs)

#define UTCDIFF 0                     // Time Difference between Real Time Clock and GMT 

#define COMMAND_BUFFER_SIZE 50