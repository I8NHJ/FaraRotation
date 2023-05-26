#define CODE_VERSION __DATE__ " " __TIME__ " N5NHJ"

#define DEBUG

#define CONTROL_PORT_SERIAL_PORT_CLASS HardwareSerial
#define NEXTION_PORT_SERIAL_PORT_CLASS HardwareSerial

#include <EEPROM.h>
#include <RTClib.h>

#include "FaraRotation_Pins.h"
#include "FaraRotation_Settings.h"
#include "FaraRotation_Commands.h"
#include "FaraRotation_Enums.h"

#include "Maidenhead.h"
#include "moon2.h"
#include "SpatialOffset.h"

/*-------- VARIABLES --------*/
CONTROL_PORT_SERIAL_PORT_CLASS *control_port;
char control_port_buffer[COMMAND_BUFFER_SIZE];
// byte control_port_buffer[COMMAND_BUFFER_SIZE];
byte incoming_control_port_byte = 0;
int control_port_buffer_index = 0;
unsigned long last_control_port_receive_time = 0;
bool rotate_both = false;

NEXTION_PORT_SERIAL_PORT_CLASS *nextion_port;
char nextion_port_buffer[COMMAND_BUFFER_SIZE];
byte incoming_nextion_port_byte = 0;
int nextion_port_buffer_index = 0;

unsigned long last_nextion_port_receive_time = 0;
unsigned long last_degrees_reading_time = 0;
unsigned long last_info_sending_time = 0;
unsigned long last_rtc_reading_time = 0;

RTC_DS3231 rtc;

struct Conf {
  unsigned int Analog_CCW[2];
  unsigned int Analog_CW[2];
  char Callsign[10];  // Max 9 characters
  char Grid[7];       // Max 6 characters
} configuration_data;

DateTime Now;
char nowString[21] = "YYYY MMM DD hh:mm:ss";

unsigned int Year;
unsigned int Month;
unsigned int Day;
double UTCTime;

double MyLong;
double MyLat;
double MoonRAscension;                                       //Moon right ascension
double MoonDeclination;
double TopRAscension;
double TopDeclination;
double LST;
double HA;
double MyMoonAz;
double MyMoonEl;
double MyMoonDist;

char TargetGrid[7];       // Max 6 characters
double TargetLong;
double TargetLat;
double TargetMoonRAscension;                                       //Moon right ascension
double TargetMoonDeclination;
double TargetTopRAscension;
double TargetTopDeclination;
double TargetLST;
double TargetHA;
double TargetMoonAz;
double TargetMoonEl;
double TargetMoonDist;

double MyPolarAngle;
double TargetPolarAngle;
double SpatialOffset;
double TXFaradayAngle;

void setup() {
  initialize_serial();
  initialize_pins();
  initialize_eeprom();
  initialize_rtc();
  read_rtc(READ_RTC_NOW);
  initialize_geometry();

  #ifdef DEBUG
    control_port->println(CODE_VERSION);
    control_port->print(F("My Long: "));
    control_port->print(String(MyLong, 4));
    control_port->print(F("\tMy Lat: "));
    control_port->println(String(MyLat, 4));
    control_port->print(F("Date: "));
    control_port->print(String(Year));
    control_port->print(F("/"));
    control_port->print(String(Month));
    control_port->print(F("/"));
    control_port->print(String(Day));
    control_port->print(F(" "));
    control_port->println(String(UTCTime));
    control_port->print(F("Moon Azimuth: "));
    control_port->print(String(MyMoonAz));
    control_port->print(F("\tElevation: "));
    control_port->print(String(MyMoonEl));
    control_port->print(F("\tDistance: "));
    control_port->println(String(MyMoonDist));
    control_port->print(F("My Polar Axis Angle: "));
    control_port->println(String(MyPolarAngle));
  #endif
}

void loop() {
  check_nextion_port_for_commands();
  read_degrees();
  read_rtc(0);
  send_info();
}

/*-------- SUBROUTINES --------*/
void initialize_geometry () {
  grid2deg(configuration_data.Grid, &MyLong, &MyLat);
  moon2(Year, Month, Day, UTCTime, MyLong, MyLat, &MoonRAscension, &MoonDeclination, &TopRAscension, &TopDeclination, &LST, &HA, &MyMoonAz, &MyMoonEl, &MyMoonDist);
  PolarAxisOffset (MyLat, MyMoonAz, MyMoonEl, 0, 0, 0, &MyPolarAngle);
} /* END initialize_geometry() */

void calculate_geometry () {  
  moon2(Year, Month, Day, UTCTime, MyLong, MyLat, &MoonRAscension, &MoonDeclination, &TopRAscension, &TopDeclination, &LST, &HA, &MyMoonAz, &MyMoonEl, &MyMoonDist);
  moon2(Year, Month, Day, UTCTime, TargetLong, TargetLat, &TargetMoonRAscension, &TargetMoonDeclination, &TargetTopRAscension, &TargetTopDeclination, &TargetLST, &TargetHA, &TargetMoonAz, &TargetMoonEl, &TargetMoonDist);
  PolarAxisOffset (MyLat, MyMoonAz, MyMoonEl, TargetLat, TargetMoonAz, TargetMoonEl, &SpatialOffset);
  double RXAngle; //to be read from antenna position!
  FaradayOffset (RXAngle, SpatialOffset, &TXFaradayAngle);
} /* END calculate_geometry() */

void send_info() {
  if ((millis() - last_info_sending_time) > INFO_SENDING_RATE) {
    char workstring[30];
    read_rtc(READ_RTC_NOW);
    calculate_geometry();
    strcpy(workstring, "tCLOCK.txt=\"");
    char nowString[]="YYYY MMM DD hh:mm:ss";
    strcat(workstring, Now.toString(nowString));
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    strcpy(workstring, "tMyGrid.txt=\"");
    strcat(workstring, configuration_data.Grid);
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    strcpy(workstring, "tMyMoonAZ.txt=\"");
    strcat(workstring, String(MyMoonAz).c_str());
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    strcpy(workstring, "tMyMoonEL.txt=\"");
    strcat(workstring, String(MyMoonEl).c_str());
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    strcpy(workstring, "tTargetGrid.txt=\"");
    strcat(workstring, TargetGrid);
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    strcpy(workstring, "tTargetMoonAZ.txt=\"");
    strcat(workstring, String(TargetMoonAz).c_str());
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    strcpy(workstring, "tTargetMoonEL.txt=\"");
    strcat(workstring, String(TargetMoonEl).c_str());
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    strcpy(workstring, "tOffset.txt=\"");
    strcat(workstring, String(SpatialOffset).c_str());
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    strcpy(workstring, "tTXFaradayAngle.txt=\"");
    strcat(workstring, String(TXFaradayAngle).c_str());
    strcat(workstring, "\"");
    send_nextion_message(workstring);    
    #ifdef DEBUG
      control_port->println(F("Sending Info Data to Nextion port"));
    #endif
    last_info_sending_time = millis();
  }
} /* END send_info() */

void initialize_eeprom() {
  byte value = EEPROM.read(0);
  if (value != 1) {
    control_port->println("Init EEPROM");
    EEPROM.write(0, 1);
    configuration_data.Analog_CCW[0] = 0;
    configuration_data.Analog_CCW[1] = 0;
    configuration_data.Analog_CW[0] = 1023;
    configuration_data.Analog_CW[1] = 1023;
    strcpy(configuration_data.Callsign, CALLSIGN);
    strcpy(configuration_data.Grid, GRID);
    EEPROM.put(1, configuration_data);
  } else {
    EEPROM.get(1, configuration_data);
  }
  #ifdef DEBUG
    control_port->println(configuration_data.Analog_CCW[0]);
    control_port->println(configuration_data.Analog_CW[0]);
    control_port->println(configuration_data.Analog_CCW[1]);
    control_port->println(configuration_data.Analog_CW[1]);
    control_port->println(configuration_data.Callsign);
    control_port->println(configuration_data.Grid);
  #endif
} /* END initialize_eeprom() */

void initialize_serial() {
  control_port = CONTROL_PORT_MAPPED_TO;
  control_port->begin(CONTROL_PORT_BAUD_RATE);
  control_port->flush();
  #ifdef DEBUG
    control_port->println(F("\r\n---\r\nApp started"));
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

void initialize_pins() {
  // define pins
  pinMode(rx_rotate_cw_enable, OUTPUT);
  pinMode(rx_rotate_ccw_enable, OUTPUT);
  pinMode(tx_rotate_cw_enable, OUTPUT);
  pinMode(tx_rotate_ccw_enable, OUTPUT);
  #if defined(PWM_OUTPUT)
    pinMode(rx_rotate_cw_pwm, OUTPUT);
    pinMode(rx_rotate_ccw_pwm, OUTPUT);
    pinMode(tx_rotate_cw_pwm, OUTPUT);
    pinMode(tx_rotate_ccw_pwm, OUTPUT);
  #endif
  #if defined(DIGITAL_OUTPUT)
    pinMode(rx_rotate_cw_dig, OUTPUT);
    pinMode(rx_rotate_ccw_dig, OUTPUT);
    pinMode(tx_rotate_cw_dig, OUTPUT);
    pinMode(tx_rotate_ccw_dig, OUTPUT);
  #endif
  // set output controls to low
  #if defined(PWM_OUTPUT)
    analogWrite(rx_rotate_cw_pwm, 0);
    analogWrite(rx_rotate_ccw_pwm, 0);
    analogWrite(tx_rotate_cw_pwm, 0);
    analogWrite(tx_rotate_ccw_pwm, 0);
  #endif
  #if defined(DIGITAL_OUTPUT)
    digitalWrite(rx_rotate_cw_dig, LOW);
    digitalWrite(rx_rotate_ccw_dig, LOW);
    digitalWrite(tx_rotate_cw_dig, LOW);
    digitalWrite(tx_rotate_ccw_dig, LOW);
  #endif
  // enable rotation (needed for PWM board)
  digitalWrite(rx_rotate_cw_enable, HIGH);
  digitalWrite(rx_rotate_ccw_enable, HIGH);
  digitalWrite(tx_rotate_cw_enable, HIGH);
  digitalWrite(tx_rotate_ccw_enable, HIGH);
} /* END initialize_pins() */

void initialize_rtc(){
  if (! rtc.begin()) {
    #ifdef DEBUG
      control_port->println(F("RTC can not be found"));
    #endif
    while (1) delay(10);
  }
  if (! rtc.lostPower()) {
    #ifdef DEBUG
      control_port->println(F("RTC time reset"));
    #endif
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
} /* END initialize_rtc() */

void read_rtc(bool read_now) {
  if ( (millis() - last_rtc_reading_time) > RTC_READING_RATE || (read_now)) {
    Now = rtc.now() + TimeSpan(0,UTCDIFF,0,0);
    Year = Now.year();
    Month = Now.month();
    Day = Now.day();
    UTCTime=(Now.hour()+(Now.minute()/60.));
    last_rtc_reading_time=millis();
  }
//  UTCTime=((Now.hour()+UTCDIFF)+(Now.minute()/60.));
} /* END read_rtc() */

void check_nextion_port_for_commands() {
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
      
      if (nextion_received_command == STOP) {
        command_received = true;
        stop_all();
        #ifdef DEBUG
          control_port->println(F("nextion_port STOP command received"));
        #endif
      }  // COMMAND STOP

      if (nextion_received_command == ROTATE_RX_ANTENNA_CW) {
        command_received = true;
        stop_all();
        rotate_antenna(ROTATE_RX_ANTENNA_CW_ENU, rotate_both);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CW command received"));
        #endif
      }  // COMMAND ROTATE_RX_ANTENNA_CW

      if (nextion_received_command == ROTATE_RX_ANTENNA_CCW) {
        command_received = true;
        stop_all();
        rotate_antenna(ROTATE_RX_ANTENNA_CCW_ENU, rotate_both);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CCW command received"));
        #endif
      }  // COMMAND ROTATE_RX_ANTENNA_CCW

      if (nextion_received_command.substring(0, 4) == ROTATE_RX_ANTENNA_TO) {
        command_received = true;
        int DegreesTo = atoi(nextion_received_command.substring(4).c_str());
        // ROTATE_RX_ANTENNA_TO LOGIC GOES HERE
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA TO command received"));
          control_port->print(F("ROTATING RX ANTENNA TO "));
          control_port->println(DegreesTo);
        #endif  
      } // COMMAND ROTATE_RX_ANTENNA_TO

      if (nextion_received_command == ROTATE_TX_ANTENNA_CW) {
        command_received = true;
        stop_all();
        rotate_antenna(ROTATE_TX_ANTENNA_CW_ENU, rotate_both);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA CW command received"));
        #endif
      }  // COMMAND ROTATE_TX_ANTENNA_CW

      if (nextion_received_command == ROTATE_TX_ANTENNA_CCW) {
        command_received = true;
        stop_all();
        rotate_antenna(ROTATE_TX_ANTENNA_CCW_ENU, rotate_both);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA CCW command received"));
        #endif
      }  // COMMAND ROTATE_TX_ANTENNA_CCW

      if (nextion_received_command.substring(0, 4) == ROTATE_TX_ANTENNA_TO) {
        command_received = true;
        int DegreesTo = atoi(nextion_received_command.substring(4).c_str());
        // ROTATE_TX_ANTENNA_TO LOGIC GOES HERE
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA TO command received"));
          control_port->print(F("ROTATE TX ANTENNA TO "));
          control_port->println(DegreesTo);
        #endif  
      } // COMMAND ROTATE_TX_ANTENNA_TO

      if (nextion_received_command.substring(0, 3) == SET_TARGET_GRID) {
        command_received = true;
        strcpy (TargetGrid, nextion_received_command.substring(3).c_str());
        read_rtc(READ_RTC_NOW);
        grid2deg(TargetGrid, &TargetLong, &TargetLat);
        moon2(Year, Month, Day, UTCTime, TargetLong, TargetLat, &TargetMoonRAscension, &TargetMoonDeclination, &TargetTopRAscension, &TargetTopDeclination, &TargetLST, &TargetHA, &TargetMoonAz, &TargetMoonEl, &TargetMoonDist);
        #ifdef DEBUG
          control_port->println(F("nextion_port SET_TARGET_GRID command received"));
          control_port->print("TARGET GRID: ");
          control_port->println(TargetGrid);
          control_port->print(F("Target Long: "));
          control_port->print(String(TargetLong, 4));
          control_port->print(F("\tTarget Lat: "));
          control_port->println(String(TargetLat, 4));
          control_port->print(F("Target Moon Azimuth: "));
          control_port->print(String(TargetMoonAz));
          control_port->print(F("\tTarget Elevation: "));
          control_port->print(String(TargetMoonEl));
          control_port->print(F("\tTarget Distance: "));
          control_port->println(String(TargetMoonDist));
        #endif
      }  // COMMAND SET_TARGET_GRID

      if (nextion_received_command == SYNC_TX_2_RX_ENABLE) {
        command_received = true;
        // SYNCRO LOGIC TO BE ADDED HERE  HERE;
        #ifdef DEBUG
          control_port->println(F("nextion_port SYNC_TX_2_RX_ENABLE command received"));
        #endif
      }  // COMMAND SYNC_TX_2_RX_ENABLE

      if (nextion_received_command == SYNC_TX_2_RX_DISABLE) {
        command_received = true;
        // SYNCRO LOGIC TO BE ADDED HERE  HERE;
        #ifdef DEBUG
          control_port->println(F("nextion_port SYNC_TX_2_RX_DISABLE command received"));
        #endif
      }  // COMMAND SYNC_TX_2_RX_DISABLE

      if (nextion_received_command == LINK_RX_AND_TX_ENABLE) {
        command_received = true;
        rotate_both = true;
        #ifdef DEBUG
          control_port->println(F("nextion_port LINK_RX_AND_TX_ENABLE command received"));
        #endif
      }  // COMMAND LINK_RX_AND_TX_ENABLE

      if (nextion_received_command == LINK_RX_AND_TX_DISABLE) {
        command_received = true;
        rotate_both = false;
        #ifdef DEBUG
          control_port->println(F("nextion_port LINK_RX_AND_TX_DISABLE command received"));
        #endif
      }  // COMMAND LINK_RX_AND_TX_DISABLE

      if (nextion_received_command == PTT_AUTOMATION_ENABLED) {
        command_received = true;
        // PTT LOGIC TO BE ADDED HERE;
        #ifdef DEBUG
          control_port->println(F("nextion_port PTT_AUTOMATION_ENABLED command received"));
        #endif
      }  // COMMAND PTT_AUTOMATION_ENABLED_ENABLE

      if (nextion_received_command == PTT_AUTOMATION_DISABLED) {
        command_received = true;
        // PTT LOGIC TO BE ADDED HERE;
        #ifdef DEBUG
          control_port->println(F("nextion_port PTT_AUTOMATION_DISABLED command received"));
        #endif
      }  // COMMAND PTT_AUTOMATION_ENABLED_DISABLED

      if (nextion_received_command == CALIBRATE_RX_ANTENNA_CW) {
        command_received = true;
        configuration_data.Analog_CW[RX_ANTENNA] = analogRead(rx_rotator_degs_pin);
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port CALIBRATE_RX_ANTENNA_CW command received"));
        #endif
      }  // COMMAND CALIBRATE_RX_ANTENNA_CW

      if (nextion_received_command == CALIBRATE_RX_ANTENNA_CCW) {
        command_received = true;
        configuration_data.Analog_CCW[RX_ANTENNA] = analogRead(rx_rotator_degs_pin);
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port CALIBRATE_RX_ANTENNA_CCW command received"));
        #endif
      }  // COMMAND CALIBRATE_RX_ANTENNA_CCW

      if (nextion_received_command == CALIBRATE_TX_ANTENNA_CW) {
        command_received = true;
        configuration_data.Analog_CW[TX_ANTENNA] = analogRead(tx_rotator_degs_pin);
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port CALIBRATE_TX_ANTENNA_CW command received"));
        #endif
      }  // COMMAND CALIBRATE_TX_ANTENNA_CW

      if (nextion_received_command == CALIBRATE_TX_ANTENNA_CCW) {
        command_received = true;
        configuration_data.Analog_CCW[TX_ANTENNA] = analogRead(tx_rotator_degs_pin);
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port CALIBRATE_TX_ANTENNA_CCW command received"));
        #endif
      }  // COMMAND CALIBRATE_TX_ANTENNA_CCW

      if (nextion_received_command == SEND_CONFIG) {
        command_received = true;
        EEPROM.get(1, configuration_data);
        char workstring[30];
        strcpy(workstring, "callsign.txt=\"");
        strcat(workstring, configuration_data.Callsign);
        strcat(workstring, "\"");
        send_nextion_message(workstring);
        strcpy(workstring, "gMyGrid.txt=\"");
        strcat(workstring, configuration_data.Grid);
        strcat(workstring, "\"");
        send_nextion_message(workstring);
        strcpy(workstring, "RX_ANT_CW_ANA.txt=\"");
        strcat(workstring, String(configuration_data.Analog_CW[0]).c_str());
        strcat(workstring, "\"");
        send_nextion_message(workstring);
        strcpy(workstring, "RX_ANT_CCW_ANA.txt=\"");
        strcat(workstring, String(configuration_data.Analog_CCW[0]).c_str());
        strcat(workstring, "\"");
        send_nextion_message(workstring);
        strcpy(workstring, "TX_ANT_CW_ANA.txt=\"");
        strcat(workstring, String(configuration_data.Analog_CW[1]).c_str());
        strcat(workstring, "\"");
        send_nextion_message(workstring);
        strcpy(workstring, "TX_ANT_CCW_ANA.txt=\"");
        strcat(workstring, String(configuration_data.Analog_CCW[1]).c_str());
        strcat(workstring, "\"");
        send_nextion_message(workstring);
        #ifdef DEBUG
          control_port->println(F("nextion_port SEND_CONFIG command received"));
        #endif
      }  // COMMAND SEND_CONFIG

      if (nextion_received_command.substring(0, 3) == SAVE_CALLSIGN_TO_EEPROM) {
        command_received = true;
        strcpy(configuration_data.Callsign, nextion_received_command.substring(3).c_str());
        #ifdef DEBUG
          control_port->print("nextion command: ");
          control_port->println(nextion_received_command);
          control_port->print("Callsign to EEPROM: ");
          control_port->println(configuration_data.Callsign);
        #endif
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port SAVE_CALLSIGN_TO_EEPROM command received"));
        #endif
      }  // COMMAND SAVE_CALLSIGN_TO_EEPROM

      if (nextion_received_command.substring(0, 3) == SAVE_GRID_TO_EEPROM) {
        command_received = true;
        strcpy(configuration_data.Grid, nextion_received_command.substring(3).c_str());
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port SAVE_GRID_TO_EEPROM command received"));
          control_port->print("GRID to EEPROM: ");
          control_port->println(configuration_data.Grid);
        #endif
      }  // COMMAND SAVE_GRID_TO_EEPROM

      if (!command_received) {
        nextion_port->flush();
        #ifdef DEBUG
          control_port->println(F("nextion_port GARBAGE received"));
        #endif
      }  // END IF (COMMAND_RECEIVED)
      last_nextion_port_receive_time = millis();
    }
  }
}  //END check_nextion_port()

void stop_all() {
  #if defined(PWM_OUTPUT)
    analogWrite(rx_rotate_cw_pwm, 0);
    analogWrite(rx_rotate_ccw_pwm, 0);
    analoglWrite(tx_rotate_cw_pwm, 0);
    analogWrite(tx_rotate_ccw_pwm, 0);
  #endif
  #if defined(DIGITAL_OUTPUT)
    digitalWrite(rx_rotate_cw_dig, LOW);
    digitalWrite(rx_rotate_ccw_dig, LOW);
    digitalWrite(tx_rotate_cw_dig, LOW);
    digitalWrite(tx_rotate_ccw_dig, LOW);
  #endif
} /* END stop_all() */

void rotate_antenna(byte action, bool link) {
  stop_all();
  #ifdef DEBUG
    test_pins();
  #endif
  switch (action) {
    case ROTATE_RX_ANTENNA_CW_ENU:
      #if defined(PWM_OUTPUT)
        analogWrite(rx_rotate_cw_pwm, 255);
        if (link) {
          analogWrite(tx_rotate_cw_pwm, 255);
        }
      #endif
      #if defined(DIGITAL_OUTPUT)
        digitalWrite(rx_rotate_cw_dig, HIGH);
        if (link) {
        digitalWrite(tx_rotate_cw_dig, HIGH);
        }
      #endif
      break;

    case ROTATE_RX_ANTENNA_CCW_ENU:
      #if defined(PWM_OUTPUT)
        analogWrite(rx_rotate_ccw_pwm, 255);
        if (link) {
          analogWrite(tx_rotate_ccw_pwm, 255);
        }
      #endif
      #if defined(DIGITAL_OUTPUT)
        digitalWrite(rx_rotate_ccw_dig, HIGH);
        if (link) {
        digitalWrite(tx_rotate_ccw_dig, HIGH);
        }
      #endif
      break;

    case ROTATE_TX_ANTENNA_CW_ENU:
      #if defined(PWM_OUTPUT)
        analogWrite(tx_rotate_cw_pwm, 255);
        if (link) {
          analogWrite(rx_rotate_cw_pwm, 255);
        }
      #endif
      #if defined(DIGITAL_OUTPUT)
        digitalWrite(tx_rotate_cw_dig, HIGH);
        if (link) {
          digitalWrite(rx_rotate_cw_dig, HIGH);
        }
      #endif
      break;

    case ROTATE_TX_ANTENNA_CCW_ENU:
      #if defined(PWM_OUTPUT)
        analogWrite(tx_rotate_ccw_pwm, 255);
        if (link) {
          analogWrite(rx_rotate_ccw_pwm, 255);
        }
      #endif
      #if defined(DIGITAL_OUTPUT)
        digitalWrite(tx_rotate_ccw_dig, HIGH);
        if (link) {
          digitalWrite(rx_rotate_ccw_dig, HIGH);
        }
      #endif
      break;
  }
  #ifdef DEBUG
    test_pins();
  #endif
} /* END rotate_antenna() */

void test_pins() {
  control_port->print("RX_CW ");
  control_port->print(String(digitalRead(rx_rotate_cw_dig)));
  control_port->print("\tRX_CCW ");
  control_port->print(String(digitalRead(rx_rotate_ccw_dig)));
  control_port->print("\tTX_CW ");
  control_port->print(String(digitalRead(tx_rotate_cw_dig)));
  control_port->print("\tTX_CCW ");
  control_port->println(String(digitalRead(tx_rotate_ccw_dig)));
} /* END test_pins() */

void read_degrees() {
  if ((millis() - last_degrees_reading_time) > POTS_READING_RATE) {
    unsigned int RX_Antenna_angle = analogRead(rx_rotator_degs_pin);
    nextion_show_angle(convert_analog_to_degrees(RX_Antenna_angle, RX_ANTENNA), RX_ANTENNA);
    unsigned int TX_Antenna_angle = analogRead(tx_rotator_degs_pin);
    nextion_show_angle(convert_analog_to_degrees(TX_Antenna_angle, TX_ANTENNA), TX_ANTENNA);
    last_degrees_reading_time = millis();
  }
} /* END read_degrees() */

int convert_analog_to_degrees(unsigned int analog_reading, unsigned int antenna) {
  if (analog_reading > configuration_data.Analog_CW[antenna]) {
    analog_reading = configuration_data.Analog_CW[antenna];
  }
  if (analog_reading < configuration_data.Analog_CCW[antenna]) {
    analog_reading = configuration_data.Analog_CCW[antenna];
  }
#ifdef DEBUG
  control_port->print(F("Anolog reading: \t"));
  control_port->println(analog_reading);
#endif
  // returns a number between -90 and 90 based on the low (ccw) and hi(cw) calibration points for the motor
  return round(((analog_reading - configuration_data.Analog_CCW[antenna]) / (float)(configuration_data.Analog_CW[antenna] - configuration_data.Analog_CCW[antenna]) * 180.0) - 90.0);
}  // END convert_analog_to_degrees

void send_nextion_message(char message[30]) {
  nextion_port->print(message);
  nextion_port->write(0xFF);
  nextion_port->write(0xFF);
  nextion_port->write(0xFF);
#ifdef DEBUG
  control_port->println(message);
#endif
} /* END send_nextion_message*/

void nextion_show_angle(int degrees, unsigned int antenna) {
  char workstring[30];
  switch (antenna) {
    case RX_ANTENNA:
      strcpy(workstring, "vRXAngle.val=");
      strcat(workstring, String(degrees).c_str());
      send_nextion_message(workstring);
      strcpy(workstring, "tRXAngle.txt=\"");
      strcat(workstring, String(degrees).c_str());
      strcat(workstring, "\"");
      send_nextion_message(workstring);
      break;
      ;
    case TX_ANTENNA:
      strcpy(workstring, "vTXAngle.val=");
      strcat(workstring, String(degrees).c_str());
      send_nextion_message(workstring);
      strcpy(workstring, "tTXAngle.txt=\"");
      strcat(workstring, String(degrees).c_str());
      strcat(workstring, "\"");
      send_nextion_message(workstring);
  }
}
