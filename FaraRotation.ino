#define DEBUG
#define CODE_VERSION "2023.04.25.01 N5NHJ"
#define CONTROL_PORT_SERIAL_PORT_CLASS Serial2
#define NEXTION_PORT_SERIAL_PORT_CLASS Serial3

#include "FaraRotation_Pins.h"
#include "FaraRotation_Settings.h"

/*-------- VARIABLES --------*/
CONTROL_PORT_SERIAL_PORT_CLASS * control_port;
byte incoming_nextion_byte = 0;
unsigned long last_nextion_receive_time = 0;
byte control_port_buffer[COMMAND_BUFFER_SIZE];
int control_port_buffer_index = 0;
byte nextion_port_buffer[COMMAND_BUFFER_SIZE];
int nextion_port_buffer_index = 0;

void setup() {  
  // put your setup code here, to run once:
  initialize_serial();
  initialize_pins();
}

void loop() {
  // put your main code here, to run repeatedly:
  check_nextion_port();
}

/*-------- SUBROUTINES --------*/
void initialize_serial(){
  control_port = CONTROL_PORT;
  control_port->begin(CONTROL_PORT_BAUD_RATE);
  control_port->flush();
    #ifdef DEBUG
      control_port->println (F("App started"));
      control_port->flush();
    #endif
  nextion_port = NEXTION_PORT;
  nextion_port->begin(NEXTION_PORT_BAUD_RATE);
  nextion_port->flush();

  #ifdef DEBUG
    control_port->println("initialize_serial() complete");
    control_port->flush();
  #endif
} /* END initialize_serial() */

void initialize_pins(){
} /* END initialize_pins() */

void clear_command_buffer(){
  control_port_buffer_index = 0;
  control_port_buffer[0] = 0;
} /* END clear_command_buffer() */

void clear_nextion_buffer(){
  nextion_port_buffer_index = 0;
  nextion_port_buffer[0] = 0;
} /* END clear_nextion_buffer() */

void check_nextion_port(){
  if (nextion_port->available()){
    incoming_nextion_byte = nextion_port->read();
    last_nextion_receive_time = millis();
  }
  if ((incoming_nextion_byte > 96) && (incoming_nextion_byte < 123)) {  // uppercase it
    incoming_snextion_byte = incoming_nextion_byte - 32;
  }  
}


