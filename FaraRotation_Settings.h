#define CALLSIGN "  N5NHJ  " // ATTENTION!: Max 9 characters
#define GRID "EM13pc" // ATTENTION: Max 6 characters

// Serial Port Settings
#define CONTROL_PORT_MAPPED_TO &Serial
#define CONTROL_PORT_BAUD_RATE 115200
#define CONTROL_PORT_READ_RATE 250

// Nextion Serial Port Settings
#define NEXTION_PORT_MAPPED_TO &Serial3
#define NEXTION_PORT_BAUD_RATE 115200
#define NEXTION_PORT_READ_RATE 250

#define COMMAND_BUFFER_SIZE 50

#define POTS_READING_RATE 10000
#define INFO_SENDING_RATE 1000

#define RX_ANTENNA 0
#define TX_ANTENNA 1

#define UTCDIFF 5

// #define PWM_OUTPUT
#define DIGITAL_OUTPUT