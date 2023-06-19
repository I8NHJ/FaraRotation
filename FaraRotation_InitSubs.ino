/*-------- SUBROUTINES --------  INITIALIZATIONS  */

void initialize_features () {
  // bit 0 <-> NEXTION 7 - OUTPUT CONTROL   (0=DIGITAL, 1=PWM) 
  // bit 1 <-> NEXTION 6 - SPEED CONTROL    (0=NO CONTROL, 1=SLOW START/STOP)
  // bit 2 <-> NEXTION 5 - PTT AUTOMATION   (0=DISABLED, 1=ENABLED)
  // bit 3 <-> NEXTION 4 - ONE ANTENNA ONLY (0=DISABLED, 1=ENABLED)
  // bit 4 <-> NEXTION 3 - TX LINKED TO RX  (0=INACTIVE, 1=ACTIVE)
  // bit 5 <-> NEXTION 2 - PTT CONTROL      (0=INACTIVE, 1=ACTIVE)
  // bit 6 <-> NEXTION 1 - PTT ENGAGED      (0=RX, 1=TX)
  // bit 7 <-> NEXTION 0 - FUTURE EXPANSION (DEFAULT=1)
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
  
  #if defined(EXTERNAL_REFERENCE)
    analogReference(EXTERNAL);
  #endif

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
