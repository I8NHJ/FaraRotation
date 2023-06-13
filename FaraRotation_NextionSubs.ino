/*-------- SUBROUTINES --------  NEXTION  */

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
        strcpy(workstring, "codeversion.txt=\"");
        strcat(workstring, CODE_VERSION);
        strcat(workstring, "\"");
        send_nextion_message(workstring);
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