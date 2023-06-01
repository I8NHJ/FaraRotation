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
      
      if (nextion_received_command.substring(0, 2) == STOP) {
        command_received = true;
        COMMAND_ENUM = STOP_ENUM;
        stop(atoi(nextion_received_command.substring(2).c_str()));
        #ifdef DEBUG
          control_port->println(F("nextion_port STOP command received"));
        #endif
      }  // COMMAND STOP

      if (nextion_received_command == ROTATE_RX_ANTENNA_CW) {
        command_received = true;
        COMMAND_ENUM = ROTATE_RX_ANTENNA_CW_ENUM;
        stop(RX_ANTENNA);
        RX_ROTATION_STATUS = RX_NORMAL_CW;
        rotate_antenna(ROTATE_RX_ANTENNA_CW_ENUM, ROTATE_ONE);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CW command received"));
        #endif
      }  // COMMAND ROTATE_RX_ANTENNA_CW

      if (nextion_received_command == ROTATE_RX_ANTENNA_CCW) {
        command_received = true;
        COMMAND_ENUM = ROTATE_RX_ANTENNA_CCW_ENUM;
        stop(RX_ANTENNA);
        RX_ROTATION_STATUS = RX_NORMAL_CCW;
        rotate_antenna(ROTATE_RX_ANTENNA_CCW_ENUM, ROTATE_ONE);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CCW command received"));
        #endif
      }  // COMMAND ROTATE_RX_ANTENNA_CCW

      if (nextion_received_command.substring(0, 4) == ROTATE_RX_ANTENNA_TO) {
        command_received = true;
        COMMAND_ENUM = ROTATE_RX_ANTENNA_TO_ENUM;
        stop(RX_ANTENNA);
        RX_DegreesTo = atoi(nextion_received_command.substring(4).c_str());
        int RX_Antenna_angle = convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA);
        if (RX_DegreesTo < RX_Antenna_angle) {
          RX_ROTATION_STATUS = RX_TO_TARGET_CCW;
          rotate_antenna(ROTATE_RX_ANTENNA_CCW_ENUM, ROTATE_ONE);
        }
        else {
          RX_ROTATION_STATUS = RX_TO_TARGET_CW;
          rotate_antenna(ROTATE_RX_ANTENNA_CW_ENUM, ROTATE_ONE);
        }
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA TO command received"));
          control_port->print(F("ROTATING RX ANTENNA TO "));
          control_port->println(RX_DegreesTo);
        #endif  
      } // COMMAND ROTATE_RX_ANTENNA_TO

      if (nextion_received_command == ROTATE_TX_ANTENNA_CW) {
        command_received = true;
        COMMAND_ENUM = ROTATE_TX_ANTENNA_CW_ENUM;
        stop(TX_ANTENNA);
        TX_ROTATION_STATUS = TX_NORMAL_CW;
        rotate_antenna(ROTATE_TX_ANTENNA_CW_ENUM, ROTATE_ONE);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA CW command received"));
        #endif
      }  // COMMAND ROTATE_TX_ANTENNA_CW

      if (nextion_received_command == ROTATE_TX_ANTENNA_CCW) {
        command_received = true;
        COMMAND_ENUM = ROTATE_TX_ANTENNA_CCW_ENUM;
        stop(TX_ANTENNA);
        TX_ROTATION_STATUS = TX_NORMAL_CCW;
        rotate_antenna(ROTATE_TX_ANTENNA_CCW_ENUM, ROTATE_ONE);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA CCW command received"));
        #endif
      }  // COMMAND ROTATE_TX_ANTENNA_CCW

      if (nextion_received_command.substring(0, 4) == ROTATE_TX_ANTENNA_TO) {
        command_received = true;
        COMMAND_ENUM = ROTATE_TX_ANTENNA_TO_ENUM;
        stop(TX_ANTENNA);
        TX_DegreesTo = atoi(nextion_received_command.substring(4).c_str());
        int TX_Antenna_angle = convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA);
        if (TX_DegreesTo < TX_Antenna_angle) {
          TX_ROTATION_STATUS = TX_TO_TARGET_CCW;
          rotate_antenna(ROTATE_TX_ANTENNA_CCW_ENUM, ROTATE_ONE);
        }
        else {
          TX_ROTATION_STATUS = TX_TO_TARGET_CW;
          rotate_antenna(ROTATE_TX_ANTENNA_CW_ENUM, ROTATE_ONE);
        }
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA TO command received"));
          control_port->print(F("ROTATE TX ANTENNA TO "));
          control_port->println(TX_DegreesTo);
        #endif  
      } // COMMAND ROTATE_TX_ANTENNA_TO

      if (nextion_received_command.substring(0, 3) == SET_TARGET_GRID) {
        command_received = true;
        COMMAND_ENUM = SET_TARGET_GRID_ENUM;
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
        COMMAND_ENUM = SYNC_TX_2_RX_ENABLE_ENUM;
        stop(TX_ANTENNA);
        TX_DegreesTo = convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA);
        int TX_Antenna_angle = convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA);
        if (TX_DegreesTo < TX_Antenna_angle) {
          TX_ROTATION_STATUS = TX_SYNC_TO_RX_CCW;
          rotate_antenna(ROTATE_TX_ANTENNA_CCW_ENUM, ROTATE_ONE);
        }
        else {
          TX_ROTATION_STATUS =  TX_SYNC_TO_RX_CW;
          rotate_antenna(ROTATE_TX_ANTENNA_CW_ENUM, ROTATE_ONE);
        }
        #ifdef DEBUG
          control_port->println(F("nextion_port SYNC_TX_2_RX_ENABLE command received"));
        #endif
      }  // COMMAND SYNC_TX_2_RX_ENABLE

      if (nextion_received_command == SYNC_TX_2_RX_DISABLE) {
        command_received = true;
        COMMAND_ENUM = SYNC_TX_2_RX_DISABLE_ENUM;
        // SYNCRO LOGIC TO BE ADDED HERE  HERE;
        #ifdef DEBUG
          control_port->println(F("nextion_port SYNC_TX_2_RX_DISABLE command received"));
        #endif
      }  // COMMAND SYNC_TX_2_RX_DISABLE

      if (nextion_received_command == LINK_RX_AND_TX_ENABLE) {
        command_received = true;
        COMMAND_ENUM = LINK_TX_AND_RX_ENABLE_ENUM;
        // rotate_both = true; ROTATE_BOTH
        #ifdef DEBUG
          control_port->println(F("nextion_port LINK_RX_AND_TX_ENABLE command received"));
        #endif
      }  // COMMAND LINK_RX_AND_TX_ENABLE

      if (nextion_received_command == LINK_RX_AND_TX_DISABLE) {
        command_received = true;
        COMMAND_ENUM = LINK_TX_AND_RX_DISABLE_ENUM;
        // rotate_both = false; ROTATE_ONE
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
}  //END check_nextion_port_for_commands()
