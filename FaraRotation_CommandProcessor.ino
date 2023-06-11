/*-------- MAIN COMMAND PROCESSING CODE  -------- */

void check_nextion_port_for_commands() {
  if (nextion_port->available()) {
    #ifdef DEBUG
      control_port->println(F("nextion_port has something to read"));
    #endif
    if ((millis() - last_nextion_port_receive_time) > NEXTION_PORT_READ_RATE) {
      COMMAND = NO_COMMAND;
      #ifdef DEBUG
        control_port->println(F("Checking nextion_port for incoming command"));
      #endif
      String nextion_received_command = (nextion_port->readStringUntil(13));
      #ifdef DEBUG
        control_port->println(nextion_received_command);
      #endif
      
      if (nextion_received_command.substring(0, 2) == STOP_CMD) {
        COMMAND = STOP;
        send_status_to_nextion(NOW);
        stop(atoi(nextion_received_command.substring(2).c_str()));
        #ifdef DEBUG
          control_port->println(F("nextion_port STOP command received"));
        #endif
      }  // COMMAND STOP

      if (nextion_received_command == ROTATE_RX_ANTENNA_CW_CMD) {
        COMMAND = ROTATE_RX_ANTENNA_CW;
        stop(RX_ANTENNA);
        RX_ROTATING = RX_ROTATING_CW;
        RX_ROTATION_STATUS = RX_NORMAL_CW;
        send_status_to_nextion(NOW);
        if (Linked) {
          rotate_antenna(ROTATE_RX_ANTENNA_CW, ROTATE_BOTH);
        }
        else {
          rotate_antenna(ROTATE_RX_ANTENNA_CW, ROTATE_ONE);
        }
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CW command received"));
        #endif
      }  // COMMAND ROTATE_RX_ANTENNA_CW

      if (nextion_received_command == ROTATE_RX_ANTENNA_CCW_CMD) {
        COMMAND = ROTATE_RX_ANTENNA_CCW;
        stop(RX_ANTENNA);
        RX_ROTATING = RX_ROTATING_CCW;
        RX_ROTATION_STATUS = RX_NORMAL_CCW;
        send_status_to_nextion(NOW);
        if (Linked) {
          rotate_antenna(ROTATE_RX_ANTENNA_CCW, ROTATE_BOTH);
        }
        else {
          rotate_antenna(ROTATE_RX_ANTENNA_CCW, ROTATE_ONE);
        }
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA CCW command received"));
        #endif
      }  // COMMAND ROTATE_RX_ANTENNA_CCW

      if (nextion_received_command.substring(0, 4) == ROTATE_RX_ANTENNA_TO_CMD) {
        //command_received = true;
        COMMAND = ROTATE_RX_ANTENNA_TO;
        stop(RX_ANTENNA);
        RX_DegreesTo = atoi(nextion_received_command.substring(4).c_str());
        int RX_Antenna_angle = convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA);
        if (rotation_way(RX_Antenna_angle, RX_DegreesTo) == CCW) {
          RX_ROTATING = RX_ROTATING_CCW;
          RX_ROTATION_STATUS = RX_TO_TARGET_CCW;
          send_status_to_nextion(NOW);
          if (Linked){
            rotate_antenna(ROTATE_TX_ANTENNA_CCW, ROTATE_BOTH);
          }
          else {
            rotate_antenna(ROTATE_RX_ANTENNA_CCW, ROTATE_ONE);
          }
        }
        else {
          RX_ROTATING = RX_ROTATING_CW;
          RX_ROTATION_STATUS = RX_TO_TARGET_CW;
          send_status_to_nextion(NOW);
          if (Linked){
            rotate_antenna(ROTATE_TX_ANTENNA_CW, ROTATE_BOTH);
          }
          else {
            rotate_antenna(ROTATE_RX_ANTENNA_CW, ROTATE_ONE);
          }
        }
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE RX ANTENNA TO command received"));
          control_port->print(F("ROTATING RX ANTENNA TO "));
          control_port->println(RX_DegreesTo);
        #endif  
      } // COMMAND ROTATE_RX_ANTENNA_TO

      if (nextion_received_command == ROTATE_TX_ANTENNA_CW_CMD) {
        COMMAND = ROTATE_TX_ANTENNA_CW;
        stop(TX_ANTENNA);
        TX_ROTATING = TX_ROTATING_CW;
        TX_ROTATION_STATUS = TX_NORMAL_CW;
        Linked = false;
        bitWrite(active_features, 4, 0);
        Ptt = false;
        bitWrite(active_features, 5, 0);
        send_status_to_nextion(NOW);
        rotate_antenna(ROTATE_TX_ANTENNA_CW, ROTATE_ONE);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA CW command received"));
        #endif
      }  // COMMAND ROTATE_TX_ANTENNA_CW

      if (nextion_received_command == ROTATE_TX_ANTENNA_CCW_CMD) {
        COMMAND = ROTATE_TX_ANTENNA_CCW;
        stop(TX_ANTENNA);
        TX_ROTATING = TX_ROTATING_CCW;
        TX_ROTATION_STATUS = TX_NORMAL_CCW;
        Linked = false;
        bitWrite(active_features, 4, 0);
        Ptt = false;
        bitWrite(active_features, 5, 0);
        send_status_to_nextion(NOW);
        rotate_antenna(ROTATE_TX_ANTENNA_CCW, ROTATE_ONE);
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA CCW command received"));
        #endif
      }  // COMMAND ROTATE_TX_ANTENNA_CCW

      if (nextion_received_command.substring(0, 4) == ROTATE_TX_ANTENNA_TO_CMD) {
        COMMAND = ROTATE_TX_ANTENNA_TO;
        stop(TX_ANTENNA);
        Linked = false;
        bitWrite(active_features, 4, 0);
        Ptt = false;
        bitWrite(active_features, 5, 0);
        TX_DegreesTo = atoi(nextion_received_command.substring(4).c_str());
        int TX_Antenna_angle = convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA);
        if (rotation_way(TX_Antenna_angle, TX_DegreesTo) == CCW) {
//        if (TX_DegreesTo < TX_Antenna_angle) {
          TX_ROTATING = TX_ROTATING_CCW;
          TX_ROTATION_STATUS = TX_TO_TARGET_CCW;
          send_status_to_nextion(NOW);
          rotate_antenna(ROTATE_TX_ANTENNA_CCW, ROTATE_ONE);
        }
        else {
          TX_ROTATING = TX_ROTATING_CW;
          TX_ROTATION_STATUS = TX_TO_TARGET_CW;
          send_status_to_nextion(NOW);
          rotate_antenna(ROTATE_TX_ANTENNA_CW, ROTATE_ONE);
        }
        #ifdef DEBUG
          control_port->println(F("nextion_port ROTATE TX ANTENNA TO command received"));
          control_port->print(F("ROTATE TX ANTENNA TO "));
          control_port->println(TX_DegreesTo);
        #endif  
      } // COMMAND ROTATE_TX_ANTENNA_TO

      if (nextion_received_command.substring(0, 3) == SET_TARGET_GRID_CMD) {
        COMMAND = SET_TARGET_GRID;
        strcpy (TargetGrid, nextion_received_command.substring(3).c_str());
        read_rtc(NOW);
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

      if (nextion_received_command == SYNC_TX_2_RX_ENABLE_CMD) {
        COMMAND = SYNC_TX_2_RX_ENABLE;
        stop(TX_ANTENNA);
        Linked = false;
        bitWrite(active_features, 4, 0);
        Ptt = false;
        bitWrite(active_features, 5, 0);
        TX_DegreesTo = convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA);
        int TX_Antenna_angle = convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA);
        if (rotation_way(TX_Antenna_angle, TX_DegreesTo) == CCW) {
//        if (TX_DegreesTo < TX_Antenna_angle) {
          TX_ROTATING = TX_ROTATING_CCW;
          TX_ROTATION_STATUS = TX_SYNC_TO_RX_CCW;
          send_status_to_nextion(NOW);
          rotate_antenna(ROTATE_TX_ANTENNA_CCW, ROTATE_ONE);
        }
        else {
          TX_ROTATING = TX_ROTATING_CW;
          TX_ROTATION_STATUS =  TX_SYNC_TO_RX_CW;
          send_status_to_nextion(NOW);
          rotate_antenna(ROTATE_TX_ANTENNA_CW, ROTATE_ONE);
        }
        #ifdef DEBUG
          control_port->println(F("nextion_port SYNC_TX_2_RX_ENABLE command received"));
        #endif
      }  // COMMAND SYNC_TX_2_RX_ENABLE

      // if (nextion_received_command == SYNC_TX_2_RX_DISABLE_CMD) {
      //   //command_received = true;
      //   COMMAND = SYNC_TX_2_RX_DISABLE;
      //   // SYNCRO LOGIC TO BE ADDED HERE  HERE;
      //   #ifdef DEBUG
      //     control_port->println(F("nextion_port SYNC_TX_2_RX_DISABLE command received"));
      //   #endif
      // }  // COMMAND SYNC_TX_2_RX_DISABLE

      if (nextion_received_command == LINK_RX_AND_TX_ENABLE_CMD) {
        COMMAND = LINK_TX_AND_RX_ENABLE;
        Linked = true;
        bitWrite(active_features, 4, 1);
        Ptt = false;
        bitWrite(active_features, 5, 0);
        #ifdef DEBUG
          control_port->println(F("nextion_port LINK_RX_AND_TX_ENABLE command received"));
        #endif
      }  // COMMAND LINK_RX_AND_TX_ENABLE

      if (nextion_received_command == LINK_RX_AND_TX_DISABLE_CMD) {
        //command_received = true;
        COMMAND = LINK_TX_AND_RX_DISABLE;
        Linked = false;
        bitWrite(active_features, 4, 0);
        Ptt = false;
        bitWrite(active_features, 5, 0);
        #ifdef DEBUG
          control_port->println(F("nextion_port LINK_RX_AND_TX_DISABLE command received"));
        #endif
      }  // COMMAND LINK_RX_AND_TX_DISABLE

      if (nextion_received_command == PTT_AUTOMATION_ENABLED_CMD) {
        COMMAND = PTT_AUTOMATION_ENABLED;
        Linked = false;
        bitWrite(active_features, 4, 0);
        Ptt = true;
        bitWrite(active_features, 5, 1);
        // Read RX ANTENNA INIT STATUS AND MOVE TX ANTENNA TO RX;
        RX_DegreesPttInit = convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA);
        TX_DegreesTo = RX_DegreesPttInit;
        int TX_Antenna_angle = convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA);
        if (rotation_way(TX_Antenna_angle, TX_DegreesTo) == CCW) {
//        if (TX_DegreesTo < TX_Antenna_angle) {
          TX_ROTATING = TX_ROTATING_CCW;
          TX_ROTATION_STATUS = TX_SYNC_TO_RX_PTT_CCW;
          send_status_to_nextion(NOW);
          rotate_antenna(ROTATE_TX_ANTENNA_CCW, ROTATE_ONE);
        }
        else {
          TX_ROTATING = TX_ROTATING_CW;
          TX_ROTATION_STATUS =  TX_SYNC_TO_RX_PTT_CW;
          send_status_to_nextion(NOW);
          rotate_antenna(ROTATE_TX_ANTENNA_CW, ROTATE_ONE);
        }

        #ifdef DEBUG
          control_port->println(F("nextion_port PTT_AUTOMATION_ENABLED command received"));
        #endif
      }  // COMMAND PTT_AUTOMATION_ENABLED_ENABLE

      if (nextion_received_command == PTT_AUTOMATION_DISABLED_CMD) {
        COMMAND = PTT_AUTOMATION_DISABLED;
        Linked = false;
        bitWrite(active_features, 4, 0);
        Ptt = false;
        bitWrite(active_features, 5, 0);
        #ifdef DEBUG
          control_port->println(F("nextion_port PTT_AUTOMATION_DISABLED command received"));
        #endif
      }  // COMMAND PTT_AUTOMATION_ENABLED_DISABLED

      if (nextion_received_command == CALIBRATE_RX_ANTENNA_CW_CMD) {
        // command_received = true;
        COMMAND = CALIBRATE_RX_ANTENNA_CW;
        configuration_data.Analog_CW[RX_ANTENNA] = analogRead(rx_rotator_degs_pin);
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port CALIBRATE_RX_ANTENNA_CW command received"));
        #endif
      }  // COMMAND CALIBRATE_RX_ANTENNA_CW

      if (nextion_received_command == CALIBRATE_RX_ANTENNA_CCW_CMD) {
        // command_received = true;
        COMMAND = CALIBRATE_RX_ANTENNA_CCW;
        configuration_data.Analog_CCW[RX_ANTENNA] = analogRead(rx_rotator_degs_pin);
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port CALIBRATE_RX_ANTENNA_CCW command received"));
        #endif
      }  // COMMAND CALIBRATE_RX_ANTENNA_CCW

      if (nextion_received_command == CALIBRATE_TX_ANTENNA_CW_CMD) {
        COMMAND = CALIBRATE_TX_ANTENNA_CW;
        //command_received = true;
        configuration_data.Analog_CW[TX_ANTENNA] = analogRead(tx_rotator_degs_pin);
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port CALIBRATE_TX_ANTENNA_CW command received"));
        #endif
      }  // COMMAND CALIBRATE_TX_ANTENNA_CW

      if (nextion_received_command == CALIBRATE_TX_ANTENNA_CCW_CMD) {
        COMMAND = CALIBRATE_TX_ANTENNA_CCW;
        //command_received = true;
        configuration_data.Analog_CCW[TX_ANTENNA] = analogRead(tx_rotator_degs_pin);
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port CALIBRATE_TX_ANTENNA_CCW command received"));
        #endif
      }  // COMMAND CALIBRATE_TX_ANTENNA_CCW

      if (nextion_received_command == SEND_CONFIG_CMD) {
        COMMAND = SEND_CONFIG;
        send_config_to_nextion();
        read_degrees(NOW);
      }  // COMMAND SEND_CONFIG

      if (nextion_received_command.substring(0, 3) == SAVE_CALLSIGN_TO_EEPROM_CMD) {
        COMMAND = SAVE_CALLSIGN_TO_EEPROM;
        // command_received = true;
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

      if (nextion_received_command.substring(0, 3) == SAVE_GRID_TO_EEPROM_CMD) {
        COMMAND = SAVE_GRID_TO_EEPROM;
        // command_received = true;
        strcpy(configuration_data.Grid, nextion_received_command.substring(3).c_str());
        EEPROM.put(1, configuration_data);
        #ifdef DEBUG
          control_port->println(F("nextion_port SAVE_GRID_TO_EEPROM command received"));
          control_port->print("GRID to EEPROM: ");
          control_port->println(configuration_data.Grid);
        #endif
      }  // COMMAND SAVE_GRID_TO_EEPROM

      if (COMMAND == NO_COMMAND) {
        nextion_port->flush();
        #ifdef DEBUG
          control_port->println(F("nextion_port GARBAGE received"));
        #endif
      }  // END IF (COMMAND_RECEIVED)
      last_nextion_port_receive_time = millis();
    }
  }
}  //END check_nextion_port_for_commands()
