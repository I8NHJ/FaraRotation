//#define CODE_VERSION "V1." __DATE__ " " __TIME__ " N5NHJ"
#define CODE_VERSION "V1." __DATE__ " N5NHJ"
//#define DEBUG

#define CONTROL_PORT_SERIAL_PORT_CLASS HardwareSerial
#define NEXTION_PORT_SERIAL_PORT_CLASS HardwareSerial

#include <EEPROM.h>
#include <RTClib.h>

#include "FaraRotation_Settings.h"
#include "FaraRotation_Pins.h"
#include "FaraRotation_Commands.h"
#include "FaraRotation_Enums.h"

#include "moon2.h"

// #include "FaraRotation_SpatialOffset.h"

/*-------- VARIABLES --------*/
CONTROL_PORT_SERIAL_PORT_CLASS *control_port;
// char control_port_buffer[COMMAND_BUFFER_SIZE];
// byte control_port_buffer[COMMAND_BUFFER_SIZE];

NEXTION_PORT_SERIAL_PORT_CLASS *nextion_port;
char nextion_port_buffer[COMMAND_BUFFER_SIZE];

uint8_t active_features;

unsigned long last_control_port_receive_time = 0;
unsigned long last_nextion_port_receive_time = 0;
unsigned long last_degrees_reading_time = 0;
unsigned long last_info_sending_time = 0;
unsigned long last_rtc_reading_time = 0;
unsigned long last_action_control_time = 0;
unsigned long last_status_sending_time = 0;
unsigned long last_ptt_checking_time = 0;

RTC_DS3231 rtc;

struct Conf {
  unsigned int Analog_CCW[2];
  unsigned int Analog_CW[2];
  char Callsign[10];                        // Max 9 characters!
  char Grid[7];                             // Max 6 characters!
} configuration_data;

DateTime Now;
char nowString[21] = "YYYY MMM DD hh:mm:ss";

unsigned int Year;
unsigned int Month;
unsigned int Day;
double UTCTime;

double MyLong;
double MyLat;
double MoonRAscension;                      //Moon right ascension
double MoonDeclination;
double TopRAscension;
double TopDeclination;
double LST;
double HA;
double MyMoonAz;
double MyMoonEl;
double MyMoonDist;

char TargetGrid[7];                         // Max 6 characters!
double TargetLong;
double TargetLat;
double TargetMoonRAscension;                //Moon right ascension
double TargetMoonDeclination;
double TargetTopRAscension;
double TargetTopDeclination;
double TargetLST;
double TargetHA;
double TargetMoonAz;
double TargetMoonEl;
double TargetMoonDist;

int MyPolarAngle;
int SpatialOffset;
int TXFaradayAngle;

int RX_DegreesTo;
int TX_DegreesTo;
int RX_DegreesPttInit;

bool Linked = false;
bool Ptt = false;

void setup() {
  initialize_serial();
  initialize_pins();
  initialize_eeprom();
  initialize_rtc();
  read_rtc(NOW);
  read_degrees(NOW);
  initialize_geometry();
  initialize_features();
  send_info_to_nextion(NOW);
  send_status_to_nextion(NOW);
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
  read_rtc(TIMED);
  read_degrees(TIMED);
  check_nextion_port_for_commands();
  check_if_action_is_needed();
  send_info_to_nextion(TIMED);
  send_status_to_nextion(TIMED);
  #if defined (PTT_AUTOMATION)
    if (Ptt) {check_ptt_status(TIMED);}
  #endif
}
