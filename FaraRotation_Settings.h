#define CALLSIGN "  N5NHJ  " // ATTENTION!: Max 9 characters
#define GRID "EM13pc" // ATTENTION: Max 6 characters

// Serial Port Settings
#define CONTROL_PORT_MAPPED_TO &Serial
#define CONTROL_PORT_BAUD_RATE 115200

// Nextion Serial Port Settings
#define NEXTION_PORT_MAPPED_TO &Serial3
#define NEXTION_PORT_BAUD_RATE 115200

#define COMMAND_BUFFER_SIZE 50

#define POTS_READING_RATE 250         // Potentiometer reading loop timing (millisecs)
#define INFO_SENDING_RATE 1000        // Info broadcasting timing (millisecs)
#define RTC_READING_RATE 1000         // RTC reading timing (millisecs)
#define ACTION_LOOP_RATE 50           // Reaction loop timing (millisecs)
#define CONTROL_PORT_READ_RATE 500    // NOT IN USE
#define NEXTION_PORT_READ_RATE 250    // Command loop timing (millisecs)

#define UTCDIFF 0

// #define PWM_OUTPUT
#define DIGITAL_OUTPUT