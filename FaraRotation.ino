#define CODE_VERSION __DATE__ " " __TIME__ " N5NHJ"

//#define DEBUG

#define CONTROL_PORT_SERIAL_PORT_CLASS HardwareSerial
#define NEXTION_PORT_SERIAL_PORT_CLASS HardwareSerial

#include <EEPROM.h>
#include <RTClib.h>

#include "FaraRotation_Pins.h"
#include "FaraRotation_Settings.h"
#include "FaraRotation_Commands.h"
#include "FaraRotation_Enums.h"

// #include "Maidenhead.h"
#include "moon2.h"
#include "FaraRotation_SpatialOffset.h"

/*-------- VARIABLES --------*/
CONTROL_PORT_SERIAL_PORT_CLASS *control_port;
// char control_port_buffer[COMMAND_BUFFER_SIZE];
// byte control_port_buffer[COMMAND_BUFFER_SIZE];

NEXTION_PORT_SERIAL_PORT_CLASS *nextion_port;
char nextion_port_buffer[COMMAND_BUFFER_SIZE];

byte active_features;

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
  char Callsign[10];                        // Max 9 characters
  char Grid[7];                             // Max 6 characters
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

char TargetGrid[7];                         // Max 6 characters
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

/*-------- SUBROUTINES --------  INITIALIZATIONS  */

void initialize_features () {
  // bit 0 <-> NEXTION 7 - OUTPUT CONTROL   (0=DIGITAL, 1=PWM) 
  // bit 1 <-> NEXTION 6 - SPEED CONTROL    (0=NO CONTROL, 1=SLOW START/STOP)
  // bit 2 <-> NEXTION 5 - PTT AUTOMATION   (0=DISABLED, 1=ENABLED)
  // bit 3 <-> NEXTION 4 - ONE ANTENNA ONLY (0=DISABLED, 1=ENABLED)
  // bit 4 <-> NEXTION 3 - TX LINKED TO RX  (0=INACTIVE, 1=ACTIVE)
  // bit 5 <-> NEXTION 2 - PTT CONTROL      (0=INACTIVE, 1=ACTIVE)
  // bit 6 <-> NEXTION 1 - FUTURE EXPANSION (DEFAULT = 0)
  // bit 7 <-> NEXTION 0 - FUTURE EXPANSION (DEFAULT = 1)
  // NEXTION STRING: 10000111
  //                 0<---->7

  active_features = B10000000; //DEFAULT

  #if defined (PWM_OUTPUT)
    bitWrite(active_features, 0, 1);
    #if defined (SLOW_START_STOP)
      bitWrite(active_features, 1, 1);
    #endif    
  #endif
  #if defined (PTT_AUTOMATION)
    bitWrite(active_features, 2, 1);
  #endif
  #if defined (ONE_ANTENNA)
    bitWrite(active_features, 3, 1);
  #endif    
} /* END initialize_features() */

void initialize_geometry () {
  grid2deg(configuration_data.Grid, &MyLong, &MyLat);
  moon2(Year, Month, Day, UTCTime, MyLong, MyLat, &MoonRAscension, &MoonDeclination, &TopRAscension, &TopDeclination, &LST, &HA, &MyMoonAz, &MyMoonEl, &MyMoonDist);
  PolarAxisOffset (MyLat, MyMoonAz, MyMoonEl, 0, 0, 0, &MyPolarAngle);
} /* END initialize_geometry() */

void calculate_geometry () {  
  moon2(Year, Month, Day, UTCTime, MyLong, MyLat, &MoonRAscension, &MoonDeclination, &TopRAscension, &TopDeclination, &LST, &HA, &MyMoonAz, &MyMoonEl, &MyMoonDist);
  moon2(Year, Month, Day, UTCTime, TargetLong, TargetLat, &TargetMoonRAscension, &TargetMoonDeclination, &TargetTopRAscension, &TargetTopDeclination, &TargetLST, &TargetHA, &TargetMoonAz, &TargetMoonEl, &TargetMoonDist);
  PolarAxisOffset (MyLat, MyMoonAz, MyMoonEl, TargetLat, TargetMoonAz, TargetMoonEl, &SpatialOffset);
  FaradayOffset (convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA), SpatialOffset, &TXFaradayAngle);
} /* END calculate_geometry() */

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
  #if defined(PTT_AUTOMATION)
    if (ptt_automation) {
      pinMode(ptt_automation, INPUT_PULLUP);
    }
  #endif
  
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
  if (rtc.lostPower()) {
    #ifdef DEBUG
      control_port->println(F("RTC time reset"));
    #endif
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
} /* END initialize_rtc() */

/*-------- SUBROUTINES --------  FUNCTIONS AND ACTIONS  */

void read_rtc(rrc read) {
  if ( (millis() - last_rtc_reading_time) > RTC_READING_RATE || (read == NOW)) {
    Now = rtc.now() + TimeSpan(0,UTCDIFF,0,0);
    Year = Now.year();
    Month = Now.month();
    Day = Now.day();
    UTCTime=(Now.hour()+(Now.minute()/60.));
    last_rtc_reading_time=millis();
  }
} /* END read_rtc() */

void stop(int antenna) {
  if ((antenna == ALL_ANTENNAS) || (Linked) && ((RX_ROTATION_STATUS != RX_IDLE) || (TX_ROTATION_STATUS != TX_IDLE))) {
    #if defined (PWM_OUTPUT)
      #if defined (SLOW_START_STOP)
        for (int speed = PWM_SPEED/2; speed > SLOW_START_STOP_MIN; speed -= SLOW_START_STOP_STEPS) {
          if (RX_ROTATING == RX_ROTATING_CW)  {analogWrite(rx_rotate_cw_pwm, speed);}
          if (RX_ROTATING == RX_ROTATING_CCW) {analogWrite(rx_rotate_ccw_pwm, speed);}
          if (TX_ROTATING == TX_ROTATING_CW)  {analogWrite(tx_rotate_cw_pwm, speed);}
          if (TX_ROTATING == TX_ROTATING_CCW) {analogWrite(tx_rotate_ccw_pwm, speed);}
          delay (SLOW_START_STOP_DELAY);
        }
      #endif
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
    RX_ROTATION_STATUS = RX_IDLE;
    RX_ROTATING = RX_NO_ROTATING;
    TX_ROTATION_STATUS = TX_IDLE;
    TX_ROTATING = TX_NO_ROTATING;
    }

  if ((antenna == RX_ANTENNA) && (RX_ROTATION_STATUS != RX_IDLE)) {
    #if defined(PWM_OUTPUT)
      #if defined (SLOW_START_STOP)
        for (int speed = PWM_SPEED/2; speed > SLOW_START_STOP_MIN; speed -= SLOW_START_STOP_STEPS) {
          if (RX_ROTATING == RX_ROTATING_CW)  {analogWrite(rx_rotate_cw_pwm, speed);}
          if (RX_ROTATING == RX_ROTATING_CCW) {analogWrite(rx_rotate_ccw_pwm, speed);} 
          delay (SLOW_START_STOP_DELAY);
        }
      #endif
      analogWrite(rx_rotate_cw_pwm, 0);
      analogWrite(rx_rotate_ccw_pwm, 0); 
    #endif    
    #if defined(DIGITAL_OUTPUT) 
      digitalWrite(rx_rotate_cw_dig, LOW);
      digitalWrite(rx_rotate_ccw_dig, LOW);
    #endif
    RX_ROTATION_STATUS = RX_IDLE;
    RX_ROTATING = RX_NO_ROTATING;
  }

  if ((antenna == TX_ANTENNA)  && (TX_ROTATION_STATUS != TX_IDLE)) {
    #if defined(PWM_OUTPUT)
      #if defined (SLOW_START_STOP)
        for (int speed = PWM_SPEED/2; speed > SLOW_START_STOP_MIN; speed -= SLOW_START_STOP_STEPS) {
          if (TX_ROTATING == TX_ROTATING_CW)  {analogWrite(tx_rotate_cw_pwm, speed);}
          if (TX_ROTATING == TX_ROTATING_CCW) {analogWrite(tx_rotate_ccw_pwm, speed);} 
          delay(SLOW_START_STOP_DELAY);
        }
      #endif
      analogWrite(tx_rotate_cw_pwm, 0);
      analogWrite(tx_rotate_ccw_pwm, 0);
    #endif
    #if defined(DIGITAL_OUTPUT) 
      digitalWrite(tx_rotate_cw_dig, LOW);
      digitalWrite(tx_rotate_ccw_dig, LOW);
    #endif
    TX_ROTATION_STATUS = TX_IDLE;
    TX_ROTATING = TX_NO_ROTATING;
  }
} /* END stop() */

void rotate_antenna(cmdenum action, rlnk rotate) {
  #ifdef DEBUG
    test_pins();
  #endif
  switch (action) {

    case ROTATE_RX_ANTENNA_CW:
      #if defined(PWM_OUTPUT)
       #if defined(SLOW_START_STOP)
          for (int speed = 0; speed < PWM_SPEED; speed += SLOW_START_STOP_STEPS) {
            analogWrite(rx_rotate_cw_pwm, speed);
            if (rotate == ROTATE_BOTH) {
              analogWrite(tx_rotate_cw_pwm, speed);
            }
            delay(SLOW_START_STOP_DELAY);
          }
        #endif
        analogWrite(rx_rotate_cw_pwm, PWM_SPEED);
        if (rotate == ROTATE_BOTH) {
          analogWrite(tx_rotate_cw_pwm, PWM_SPEED);
        }
      #endif
      #if defined(DIGITAL_OUTPUT)
        digitalWrite(rx_rotate_cw_dig, HIGH);
          if (rotate == ROTATE_BOTH) {
            digitalWrite(tx_rotate_cw_dig, HIGH);
        }
      #endif
    break;

    case ROTATE_RX_ANTENNA_CCW:
      #if defined(PWM_OUTPUT)
        #if defined(SLOW_START_STOP)
          for (int speed = 0; speed < PWM_SPEED; speed += SLOW_START_STOP_STEPS) {
            analogWrite(rx_rotate_ccw_pwm, speed);
            if (rotate == ROTATE_BOTH) {
              analogWrite(tx_rotate_ccw_pwm, speed);
            }
            delay(SLOW_START_STOP_DELAY);
          }
        #endif
      analogWrite(rx_rotate_ccw_pwm, PWM_SPEED);
      if (rotate == ROTATE_BOTH) {
        analogWrite(tx_rotate_ccw_pwm, PWM_SPEED);
      }
      #endif
      #if defined(DIGITAL_OUTPUT)
        digitalWrite(rx_rotate_ccw_dig, HIGH);
        if (rotate == ROTATE_BOTH) {
          digitalWrite(tx_rotate_ccw_dig, HIGH);
        }
      #endif
    break;

    case ROTATE_TX_ANTENNA_CW:
      #if defined(PWM_OUTPUT)
        #if defined(SLOW_START_STOP)
          for (int speed = 0; speed < PWM_SPEED; speed += SLOW_START_STOP_STEPS) {
            analogWrite(tx_rotate_cw_pwm, speed);
            if (rotate == ROTATE_BOTH) {
              analogWrite(rx_rotate_cw_pwm, speed);
            }
            delay(SLOW_START_STOP_DELAY);
          }
        #endif    
        analogWrite(tx_rotate_cw_pwm, PWM_SPEED);
        if (rotate == ROTATE_BOTH) {
          analogWrite(rx_rotate_cw_pwm, PWM_SPEED);
        }
      #endif
      #if defined(DIGITAL_OUTPUT)
        digitalWrite(tx_rotate_cw_dig, HIGH);
        if (rotate == ROTATE_BOTH) {
          digitalWrite(rx_rotate_cw_dig, HIGH);
        }
      #endif
    break;

    case ROTATE_TX_ANTENNA_CCW:
      #if defined(PWM_OUTPUT)
        #if defined(SLOW_START_STOP)
          for (int speed = 0; speed < PWM_SPEED; speed += SLOW_START_STOP_STEPS) {
            analogWrite(tx_rotate_ccw_pwm, speed);
            if (rotate == ROTATE_BOTH) {
              analogWrite(rx_rotate_ccw_pwm, speed);
            }
            delay(SLOW_START_STOP_DELAY);
          }
        #endif
        analogWrite(tx_rotate_ccw_pwm, PWM_SPEED);
        if (rotate == ROTATE_BOTH) {
          analogWrite(rx_rotate_ccw_pwm, PWM_SPEED);
        }
      #endif
      #if defined(DIGITAL_OUTPUT)
        digitalWrite(tx_rotate_ccw_dig, HIGH);
        if (rotate == ROTATE_BOTH) {
          digitalWrite(rx_rotate_ccw_dig, HIGH);
        }
      #endif
    break;
  }
  #ifdef DEBUG
    test_pins();
  #endif
} /* END rotate_antenna() */

#if defined(PTT_AUTOMATION)
void check_ptt_status(rrc read) {
  if ((millis() - last_ptt_checking_time) > PTT_CHECKING_RATE || (read == NOW)) {
  // Enable TX to RX LINK ?
  // If (PTT) move to Faraday else move to Old RX degrees 
  if (digitalRead(ptt_automation)){
    control_port->println("TX OFF RITORNA SU RX");
    // Ritorna a RXToReturn
  }
  else
  {
    control_port->println("TX ON MUOVI TO FARADAY");
    // Leggi la posizione di RX RXToReturn 
  }

    last_ptt_checking_time=millis();
  }
} /* END check_ptt_status() */
#endif

#ifdef DEBUG
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
#endif

void read_degrees(rrc read) {
  if ((millis() - last_degrees_reading_time) > POTS_READING_RATE  || (read == NOW)) {
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
}  /* END convert_analog_to_degrees() */

void reduce_speed(unsigned int antenna, int rdir) {
  if (antenna == RX_ANTENNA) {
    if (rdir == RX_ROTATING_CW) {analogWrite(rx_rotate_cw_pwm,  PWM_SPEED/2); RX_ROTATION_STATUS = RX_SLOW_DOWN_CW;}
    if (rdir == RX_ROTATING_CCW) {analogWrite(rx_rotate_ccw_pwm,  PWM_SPEED/2); RX_ROTATION_STATUS = RX_SLOW_DOWN_CCW;}
  }
    if (antenna == TX_ANTENNA) {
    if (rdir == TX_ROTATING_CW)  {analogWrite(tx_rotate_cw_pwm,  PWM_SPEED/2); TX_ROTATION_STATUS = TX_SLOW_DOWN_CW;}
    if (rdir == TX_ROTATING_CCW) {analogWrite(tx_rotate_ccw_pwm, PWM_SPEED/2); TX_ROTATION_STATUS = TX_SLOW_DOWN_CCW;}
  }
} /* END reduce_speed() */

void check_if_action_is_needed() {
  if ((RX_ROTATION_STATUS != RX_IDLE) || (TX_ROTATION_STATUS != TX_IDLE)) {
    if ((millis() - last_action_control_time) > ACTION_LOOP_RATE) {

      if ((RX_ROTATION_STATUS == RX_TO_TARGET_CW) || (RX_ROTATION_STATUS == RX_SLOW_DOWN_CW)) {
        #if defined(PWM_OUTPUT)
          #if defined (SLOW_START_STOP)
            if ( abs((RX_DegreesTo - convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA))) < SLOW_START_STOP_PROXIMITY) {
              reduce_speed(RX_ANTENNA, RX_ROTATING);
            }
          #endif
        #endif
        if (RX_DegreesTo <= convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA)) {
          stop(RX_ANTENNA);
          // RX_ROTATION_STATUS = RX_IDLE;
        }
      }

      if ((RX_ROTATION_STATUS == RX_TO_TARGET_CCW) || (RX_ROTATION_STATUS == RX_SLOW_DOWN_CCW)) {
        #if defined(PWM_OUTPUT)
          #if defined (SLOW_START_STOP)
            if ( abs((RX_DegreesTo - convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA))) < SLOW_START_STOP_PROXIMITY) {
              reduce_speed(RX_ANTENNA, RX_ROTATING);
            }
          #endif
        #endif
        if (RX_DegreesTo >= convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA)) {
          stop(RX_ANTENNA);
        //  RX_ROTATION_STATUS = RX_IDLE;
        }
      }

      if ((TX_ROTATION_STATUS == TX_TO_TARGET_CW) || (TX_ROTATION_STATUS == TX_SYNC_TO_RX_CW) || (TX_ROTATION_STATUS == TX_SLOW_DOWN_CW)) {
        #if defined(PWM_OUTPUT)
          #if defined (SLOW_START_STOP)
            if ( abs((TX_DegreesTo - convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA))) < SLOW_START_STOP_PROXIMITY) {
              reduce_speed(TX_ANTENNA, TX_ROTATING);
            }
          #endif
        #endif
        if (TX_DegreesTo <= convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA)) {
          stop(TX_ANTENNA);
          // TX_ROTATION_STATUS = TX_IDLE;
        } 
      }

      if ((TX_ROTATION_STATUS == TX_TO_TARGET_CCW) || (TX_ROTATION_STATUS == TX_SYNC_TO_RX_CCW) || (TX_ROTATION_STATUS == TX_SLOW_DOWN_CCW)) {
        #if defined(PWM_OUTPUT)
          #if defined (SLOW_START_STOP)
            if ( abs((TX_DegreesTo - convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA))) < SLOW_START_STOP_PROXIMITY) {
              reduce_speed(TX_ANTENNA, TX_ROTATING);
            }
          #endif
        #endif
        if (TX_DegreesTo >= convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA)) {
          stop(TX_ANTENNA);
        //  TX_ROTATION_STATUS = TX_IDLE; 
        }
      }

      last_action_control_time = millis();  
    }
  }
} /* END check_if_action_is_needed() */

/*-------- SUBROUTINES --------  NEXTION RELATED  */

void send_nextion_message(char message[30]) {
  nextion_port->print(message);
  nextion_port->write(0xFF);
  nextion_port->write(0xFF);
  nextion_port->write(0xFF);
  #ifdef DEBUG
    control_port->println(message);
  #endif
} /* END send_nextion_message() */

void send_config_to_nextion() {
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
} /* END send_config_to_nextion() */

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
} /* END nextion_show_angle() */

void send_info_to_nextion(rrc read) {
  if (((millis() - last_info_sending_time) > INFO_SENDING_RATE) || (read == NOW)) {
    char workstring[30];
    read_rtc(NOW);
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
    strcpy(workstring, "tTXFaradayDeg.txt=\"");
    strcat(workstring, String(TXFaradayAngle).c_str());
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    #ifdef DEBUG
      control_port->println(F("Sending Info Data to Nextion port"));
    #endif
    last_info_sending_time = millis();
  }
} /* END send_info() */

void send_status_to_nextion(rrc read) {
  if (((millis() - last_status_sending_time) > STATUS_SENDING_RATE) || (read == NOW)) {
    char workstring[30];
    strcpy(workstring, "gStatus.txt=\"");
    for (int i = 7; i >= 0; i--) {
      strcat(workstring, String(bitRead(active_features, i)).c_str());
    }
    strcat(workstring, "\"");
    send_nextion_message(workstring);
    #ifdef DEBUG
      control_port->println(F("Sending Status Data to Nextion port"));
      control_port->println(workstring);
    #endif
    strcpy(workstring, "tRXRotStatus.txt=\"");
    strcat(workstring, ROTATION_STATUS_TEXT[RX_ROTATION_STATUS]);
    strcat(workstring, "\"");
    send_nextion_message(workstring);    
    strcpy(workstring, "tTXRotStatus.txt=\"");
    strcat(workstring, ROTATION_STATUS_TEXT[TX_ROTATION_STATUS]);
    strcat(workstring, "\"");
    send_nextion_message(workstring);   
    last_status_sending_time = millis();
  }
} /* END send_status _to_nextion() */



