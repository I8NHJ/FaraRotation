#define DEBUG
#define CODE_VERSION "2023.04.25.01 N5NHJ"
#define CONTROL_PORT_SERIAL_PORT_CLASS HardwareSerial
#define NEXTION_PORT_SERIAL_PORT_CLASS HardwareSerial

#include <EEPROM.h>
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
unsigned long last_degrees_reading_time = 0;
struct Cal {
  unsigned int Low[2];
  unsigned int Hi[2];
};
Cal calibration_data;

void setup() {
  initialize_serial();
  initialize_pins();
  initialize_eeprom();
}

void loop() {
  check_nextion_port();
  read_degrees();
}

/*-------- SUBROUTINES --------*/
void initialize_eeprom() {
  byte value = EEPROM.read(0);
  if (value!=1){
      control_port->println ("Init EEPROM");
    EEPROM.write(0,1);
    calibration_data = {
      0,
      0,
      1023,
      1023
    };
    EEPROM.put(1, calibration_data);
  }
  else {
    EEPROM.get(1,calibration_data);
  }
  #ifdef DEBUG
    control_port->println (calibration_data.Low[0]);
    control_port->println (calibration_data.Hi[0]); 
    control_port->println (calibration_data.Low[1]); 
    control_port->println (calibration_data.Hi[1]);
  #endif  
} /* END initialize_eeprom() */

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
  pinMode(rx_rotate_cw_enable,OUTPUT);
  pinMode(rx_rotate_ccw_enable,OUTPUT);
  pinMode(rx_rotate_cw_pwm,OUTPUT);
  pinMode(rx_rotate_ccw_pwm,OUTPUT);
  pinMode(tx_rotate_cw_enable,OUTPUT);
  pinMode(tx_rotate_ccw_enable,OUTPUT);
  pinMode(tx_rotate_cw_pwm,OUTPUT);
  pinMode(tx_rotate_ccw_pwm,OUTPUT);

  // enable pwm pins
  digitalWrite(rx_rotate_cw_enable,HIGH);
  digitalWrite(rx_rotate_ccw_enable,HIGH);
  digitalWrite(tx_rotate_cw_enable,HIGH);
  digitalWrite(tx_rotate_ccw_enable,HIGH);

  // set controls to low
  digitalWrite(rx_rotate_cw_pwm,LOW);
  digitalWrite(rx_rotate_ccw_pwm,LOW);
  digitalWrite(tx_rotate_cw_pwm,LOW);
  digitalWrite(tx_rotate_ccw_pwm,LOW);

} /* END initialize_pins() */

void check_nextion_port() {
  if (nextion_port->available()) {
      #ifdef DEBUG
        control_port->println(F("nextion_port has something to read"));
      #endif
    if ((millis() - last_nextion_port_receive_time) > NEXTION_PORT_READ_RATE) {
      bool command_received = false;
      #ifdef DEBUG
        control_port->println(F("Checking nextion_port for incoming command"));
      #endif
      String nextion_received_command = (nextion_port->readStringUntil(13));
      #ifdef DEBUG
        control_port->println(nextion_received_command);
      #endif
      if (nextion_received_command == STOP){
        command_received = true;
        stop_all();
        #ifdef DEBUG
          control_port->println(F("nextion_port STOP command received"));
        #endif
      } // COMMAND STOP 
 
      if (nextion_received_command == ROTATE_RX_ANTENNA_CW) {
        command_received = true;
        stop_all();
        rotate_antenna (ROTATE_RX_ANTENNA_CW_ENU);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CW command received"));
        #endif
      }  // COMMAND ROTATE_RX_ANTENNA_CW

      if (nextion_received_command == ROTATE_RX_ANTENNA_CCW) {
        command_received = true;
        stop_all();
        rotate_antenna (ROTATE_RX_ANTENNA_CCW_ENU);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CCW command received"));
        #endif
      } // COMMAND ROTATE_RX_ANTENNA_CCW

      if (nextion_received_command == ROTATE_TX_ANTENNA_CW) {
        command_received = true;
        stop_all();
        rotate_antenna (ROTATE_TX_ANTENNA_CW_ENU);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA CW command received"));
        #endif
      } // COMMAND ROTATE_TX_ANTENNA_CW

      if (nextion_received_command == ROTATE_TX_ANTENNA_CCW) {
        command_received = true;
        stop_all();
        rotate_antenna (ROTATE_TX_ANTENNA_CCW_ENU);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA CCW command received"));
        #endif
      } // COMMAND ROTATE_TX_ANTENNA_CCW
      
      if (nextion_received_command == CALIBRATE_RX_ANTENNA_CW) {
        command_received = true;
        calibration_data.Hi[RX_ANTENNA] = analogRead(rx_rotator_degs_pin);
        EEPROM.put(1, calibration_data);
      } // COMMAND CALIBRATE_RX_ANTENNA_CW

      if (nextion_received_command == CALIBRATE_RX_ANTENNA_CCW) {
        command_received = true;
        calibration_data.Low[RX_ANTENNA] = analogRead(rx_rotator_degs_pin);
        EEPROM.put(1, calibration_data);
      } // COMMAND CALIBRATE_RX_ANTENNA_CCW

      if (nextion_received_command == CALIBRATE_TX_ANTENNA_CW) {
        command_received = true;
        calibration_data.Hi[TX_ANTENNA] = analogRead(tx_rotator_degs_pin);
        EEPROM.put(1, calibration_data);
      } // COMMAND CALIBRATE_RX_ANTENNA_CW

      if (nextion_received_command == CALIBRATE_TX_ANTENNA_CCW) {
        command_received = true;
        calibration_data.Low[TX_ANTENNA] = analogRead(tx_rotator_degs_pin);
        EEPROM.put(1, calibration_data);
      } // COMMAND CALIBRATE_RX_ANTENNA_CCW

      if (!command_received) {
        nextion_port->flush();
        #ifdef DEBUG
          control_port->println(F("nextion_port garbage received"));
        #endif 
      } // END IF (COMMAND_RECEIVED)
      last_nextion_port_receive_time = millis();
    }
  }
} //END check_nextion_port()

void stop_all(){
  digitalWrite(rx_rotate_cw_pwm,LOW);
  digitalWrite(rx_rotate_ccw_pwm,LOW);
  digitalWrite(tx_rotate_cw_pwm,LOW);
  digitalWrite(tx_rotate_ccw_pwm,LOW);
} /* END stop_all() */

void rotate_antenna(byte action) {
  switch (action) {
    case ROTATE_RX_ANTENNA_CW_ENU:
      digitalWrite(rx_rotate_cw_pwm,HIGH);
    break;
    case ROTATE_RX_ANTENNA_CCW_ENU:
          digitalWrite(rx_rotate_ccw_pwm,HIGH);
    break;
    case ROTATE_TX_ANTENNA_CW_ENU:
          digitalWrite(tx_rotate_cw_pwm,HIGH);
    break;
    case ROTATE_TX_ANTENNA_CCW_ENU:
          digitalWrite(tx_rotate_ccw_pwm,HIGH);
    break;
  }
}  /* END rotate_antenna() */

void read_degrees() {
  if ((millis() - last_degrees_reading_time) > POTS_READING_RATE) {
    unsigned int RX_Antenna_angle = analogRead(rx_rotator_degs_pin);
    nextion_show_angle (convert_analog_to_degrees(RX_Antenna_angle, RX_ANTENNA), RX_ANTENNA);
    unsigned int TX_Antenna_angle = analogRead(tx_rotator_degs_pin);
    nextion_show_angle (convert_analog_to_degrees(TX_Antenna_angle, TX_ANTENNA), TX_ANTENNA);
    last_degrees_reading_time = millis();
  }  
} /* END read_degrees() */

int convert_analog_to_degrees(unsigned int analog_reading, unsigned int antenna){
  if (analog_reading > calibration_data.Hi[antenna]) {
    analog_reading = calibration_data.Hi[antenna];
  }
  if (analog_reading < calibration_data.Low[antenna]) {
    analog_reading = calibration_data.Low[antenna];
  }
  return round((180.0/(calibration_data.Hi[antenna]-calibration_data.Low[antenna]))*analog_reading);  
} // END convert_analog_to_degrees

void nextion_show_angle(int degrees, unsigned int antenna) {
  char workstring[30];
  switch (antenna) {
    case RX_ANTENNA:
      strcpy(workstring, "vRXAngle.val=");
      strcat (workstring, String(degrees).c_str());
      nextion_port->print (workstring);
      nextion_port->write(0xFF);
      nextion_port->write(0xFF);
      nextion_port->write(0xFF);
      #ifdef DEBUG
        control_port->println(workstring);
      #endif
      strcpy(workstring, "tRXAngle.txt=\"");
      strcat (workstring, String(degrees-90).c_str());
      strcat(workstring,"\"");
      nextion_port->print (workstring);
      nextion_port->write(0xFF);
      nextion_port->write(0xFF);
      nextion_port->write(0xFF);
      #ifdef DEBUG
        control_port->println(workstring);
      #endif
      break;;
    case TX_ANTENNA:
      strcpy(workstring, "vTXAngle.val=");
      strcat (workstring, String(degrees).c_str());
      nextion_port->print (workstring);
      nextion_port->write(0xFF);
      nextion_port->write(0xFF);
      nextion_port->write(0xFF);
      strcpy(workstring, "tTXAngle.txt=\"");
      strcat (workstring, String(degrees).c_str());
      strcat(workstring,"\"");
      nextion_port->print (workstring);
      nextion_port->write(0xFF);
      nextion_port->write(0xFF);
      nextion_port->write(0xFF);
  }
  //#ifdef DEBUG
  //  control_port->println(workstring);
  //#endif
}
