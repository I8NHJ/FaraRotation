#define DEBUG
#define CODE_VERSION "2023.04.25.01 N5NHJ"
#define CONTROL_PORT_SERIAL_PORT_CLASS HardwareSerial
#define NEXTION_PORT_SERIAL_PORT_CLASS HardwareSerial

#include "FaraRotation_Pins.h"
#include "FaraRotation_Settings.h"
#include "FaraRotation_Commands.h"

/*-------- VARIABLES --------*/
CONTROL_PORT_SERIAL_PORT_CLASS *control_port;
char control_port_buffer[COMMAND_BUFFER_SIZE];
// byte control_port_buffer[COMMAND_BUFFER_SIZE];
byte incoming_control_port_byte = 0;
int control_port_buffer_index = 0;
unsigned long last_control_port_receive_time = 0;

NEXTION_PORT_SERIAL_PORT_CLASS *nextion_port;
char nextion_port_buffer[COMMAND_BUFFER_SIZE];
// byte nextion_port_buffer[COMMAND_BUFFER_SIZE];
byte incoming_nextion_port_byte = 0;
int nextion_port_buffer_index = 0;
unsigned long last_nextion_port_receive_time = 0;

void setup() {  
  initialize_serial();
  initialize_pins();
}

void loop() {
  check_nextion_port();
//  control_port->println ("This comes from control_port");
//  delay(1000);
}

/*-------- SUBROUTINES --------*/
void initialize_serial() {
  control_port = CONTROL_PORT_MAPPED_TO;
  control_port->begin(CONTROL_PORT_BAUD_RATE);
  control_port->flush();
  #ifdef DEBUG
    control_port->println (F("\r\n---\r\nApp started"));
    control_port->flush();
    control_port->println("initialize_serial(control_port) complete");
    control_port->flush();
  #endif

  nextion_port = NEXTION_PORT_MAPPED_TO;
  nextion_port->begin(NEXTION_PORT_BAUD_RATE);
  nextion_port->flush();
  #ifdef DEBUG
    control_port->println("initialize_serial(nextion_port) complete");
    control_port->flush();
  #endif
} /* END initialize_serial() */

void initialize_pins(){
} /* END initialize_pins() */

void clear_command_buffer() {
  control_port_buffer_index = 0;
  control_port_buffer[0] = 0;
} /* END clear_command_buffer() */

void check_nextion_port() {
  if (nextion_port->available()) {
      #ifdef DEBUG
        control_port->println(F("nextion_port has something to be read"));
      #endif
    if ((millis() - last_nextion_port_receive_time) > NEXTION_PORT_READ_RATE) {
      bool command_received = false;
      #ifdef DEBUG
        control_port->println(F("Checking nextion_port for incoming command"));
      #endif
      String nextion_received_command = (nextion_port->readStringUntil('\n'));
      #ifdef DEBUG
        control_port->println(nextion_received_command);
      #endif
      if (nextion_received_command == STOP) {
        command_received = true;
        #ifdef DEBUG
          control_port->println(F("nextion_port STOP command received"));
        #endif
      } // STOP COMMAND 
 
      if (nextion_received_command == ROTATE_RX_ANTENNA_CW) {
        command_received = true;
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CW command received"));
        #endif
      }  // ROTATE_RX_ANTENNA_CW

      if (nextion_received_command == ROTATE_RX_ANTENNA_CCW) {
        command_received = true;
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CCW command received"));
        #endif
      }  // ROTATE_RX_ANTENNA_CCW

      if (command_received) {
        last_nextion_port_receive_time = millis();
      }
      else {
        #ifdef DEBUG
          control_port->println(F("nextion_port garbage received"));
        #endif // END IF (COMMAND_RECEIVED)
      }
    }
  }
  if ((incoming_nextion_port_byte > 96) && (incoming_nextion_port_byte < 123)) {  // uppercase it
    incoming_nextion_port_byte = incoming_nextion_port_byte - 32;
  }
}
