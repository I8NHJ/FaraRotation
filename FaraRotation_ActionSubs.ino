/*-------- SUBROUTINES --------  ACTIONS  */

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
    #ifdef PWM_OUTPUT
      #ifdef SLOW_START_STOP
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

    #ifdef DIGITAL_OUTPUT 
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
    #ifdef PWM_OUTPUT
      #ifdef SLOW_START_STOP
        for (int speed = PWM_SPEED/2; speed > SLOW_START_STOP_MIN; speed -= SLOW_START_STOP_STEPS) {
          if (RX_ROTATING == RX_ROTATING_CW)  {analogWrite(rx_rotate_cw_pwm, speed);}
          if (RX_ROTATING == RX_ROTATING_CCW) {analogWrite(rx_rotate_ccw_pwm, speed);} 
          delay (SLOW_START_STOP_DELAY);
        }
      #endif
      analogWrite(rx_rotate_cw_pwm, 0);
      analogWrite(rx_rotate_ccw_pwm, 0); 
    #endif    
    #ifdef DIGITAL_OUTPUT 
      digitalWrite(rx_rotate_cw_dig, LOW);
      digitalWrite(rx_rotate_ccw_dig, LOW);
    #endif
    RX_ROTATION_STATUS = RX_IDLE;
    RX_ROTATING = RX_NO_ROTATING;
  }

  if ((antenna == TX_ANTENNA)  && (TX_ROTATION_STATUS != TX_IDLE)) {
    #ifdef PWM_OUTPUT
      #ifdef SLOW_START_STOP
        for (int speed = PWM_SPEED/2; speed > SLOW_START_STOP_MIN; speed -= SLOW_START_STOP_STEPS) {
          if (TX_ROTATING == TX_ROTATING_CW)  {analogWrite(tx_rotate_cw_pwm, speed);}
          if (TX_ROTATING == TX_ROTATING_CCW) {analogWrite(tx_rotate_ccw_pwm, speed);} 
          delay(SLOW_START_STOP_DELAY);
        }
      #endif
      analogWrite(tx_rotate_cw_pwm, 0);
      analogWrite(tx_rotate_ccw_pwm, 0);
    #endif
    #ifdef DIGITAL_OUTPUT 
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
      #ifdef PWM_OUTPUT
       #ifdef SLOW_START_STOP
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
      #ifdef DIGITAL_OUTPUT
        digitalWrite(rx_rotate_cw_dig, HIGH);
          if (rotate == ROTATE_BOTH) {
            digitalWrite(tx_rotate_cw_dig, HIGH);
        }
      #endif
    break;

    case ROTATE_RX_ANTENNA_CCW:
      #ifdef PWM_OUTPUT
        #ifdef SLOW_START_STOP
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
      #ifdef DIGITAL_OUTPUT
        digitalWrite(rx_rotate_ccw_dig, HIGH);
        if (rotate == ROTATE_BOTH) {
          digitalWrite(tx_rotate_ccw_dig, HIGH);
        }
      #endif
    break;

    case ROTATE_TX_ANTENNA_CW:
      #ifdef PWM_OUTPUT
        #ifdef SLOW_START_STOP
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
      #ifdef DIGITAL_OUTPUT
        digitalWrite(tx_rotate_cw_dig, HIGH);
        if (rotate == ROTATE_BOTH) {
          digitalWrite(rx_rotate_cw_dig, HIGH);
        }
      #endif
    break;

    case ROTATE_TX_ANTENNA_CCW:
      #ifdef PWM_OUTPUT
        #ifdef SLOW_START_STOP
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
      #ifdef DIGITAL_OUTPUT
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

#ifdef PTT_AUTOMATION
void check_ptt_status(rrc read) {
  static int DegreesPttReturnTo;
  static int FaradayDegrees; 
  if ((millis() - last_ptt_checking_time) > PTT_CHECKING_RATE || (read == NOW)) {
    if (digitalRead(ptt_automation)) {                                                        // If (PTT) move to Faraday else move to Old RX degrees
      if (PTTEngaged) {
        #ifdef DEBUG 
          control_port->print("PTT IS OFF. BACK TO BEST RX ANGLE: ");
          control_port->println(DegreesPttReturnTo);
        #endif
        RX_DegreesTo = DegreesPttReturnTo;
        TX_DegreesTo = DegreesPttReturnTo;
        ptt_move_to(DegreesPttReturnTo);
        PTTEngaged = false;
        bitWrite(active_features, 6, 0);
      }
      int RX_DegreesPtt = convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA);
      int TX_DegreesPtt = convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA);
      if (abs(RX_DegreesPtt - TX_DegreesPtt) > 1) {                                                 // Check if RX degrees have changed since starting automation, if yes move and init. 1 is the 'tollerance' angle. 
        if (rotation_way(TX_DegreesPtt, RX_DegreesPtt) == CCW) {
          if (TX_ROTATING == TX_NO_ROTATING) {
            TX_ROTATING = TX_ROTATING_CCW;
            TX_ROTATION_STATUS = TX_SYNC_TO_RX_PTT_CCW;
            send_status_to_nextion(NOW);
            rotate_antenna(ROTATE_TX_ANTENNA_CCW, ROTATE_ONE);
          }
        }
        else {
          if (TX_ROTATING == TX_NO_ROTATING) {
            TX_ROTATING = TX_ROTATING_CW;
            TX_ROTATION_STATUS =  TX_SYNC_TO_RX_PTT_CW;
            send_status_to_nextion(NOW);
            rotate_antenna(ROTATE_TX_ANTENNA_CW, ROTATE_ONE);
          }
        }
      }
      else {
        stop(TX_ANTENNA);
        TX_ROTATING = TX_NO_ROTATING;
      }
    }
    else {
      if (!PTTEngaged) {
        DegreesPttReturnTo = convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA);
        RX_DegreesTo = TXFaradayAngle;
        TX_DegreesTo = TXFaradayAngle;
        #ifdef DEBUG
          control_port->print("PTT IS ON. MOVE TO THE FARADAY ANGLE: ");
          control_port->println(TXFaradayAngle);
        #endif
        ptt_move_to(TXFaradayAngle);
        PTTEngaged = true;
        bitWrite(active_features, 6, 1);
      }
    }
  last_ptt_checking_time=millis();
  }
} /* END check_ptt_status() */

void ptt_move_to (int ptt_angle_to){
        stop(RX_ANTENNA);
        stop(TX_ANTENNA);
        int RX_Antenna_angle = convert_analog_to_degrees(analogRead(rx_rotator_degs_pin), RX_ANTENNA);
        // int TX_Antenna_angle = convert_analog_to_degrees(analogRead(tx_rotator_degs_pin), TX_ANTENNA);
        if (rotation_way(RX_Antenna_angle, ptt_angle_to) == CCW) {
          RX_ROTATING = RX_ROTATING_CCW;
          RX_ROTATION_STATUS = RX_TO_TARGET_CCW;
          TX_ROTATING = TX_ROTATING_CCW;
          TX_ROTATION_STATUS = TX_TO_TARGET_CCW;
          send_status_to_nextion(NOW);
          rotate_antenna(ROTATE_RX_ANTENNA_CCW, ROTATE_BOTH);
        }
        else {
          RX_ROTATING = RX_ROTATING_CW;
          RX_ROTATION_STATUS = RX_TO_TARGET_CW;
          TX_ROTATING = TX_ROTATING_CW;
          TX_ROTATION_STATUS = TX_TO_TARGET_CW;
          send_status_to_nextion(NOW);
          rotate_antenna(ROTATE_RX_ANTENNA_CW, ROTATE_BOTH);
        }
        #ifdef DEBUG
          control_port->print(F("ROTATING RX AND TX ANTENNAS BECAUSE OF PTT "));
          control_port->println(RX_DegreesTo);
        #endif  
} /* END ptt_move_to() */
#endif /*PTT_AUTOMATION */

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
        #ifdef PWM_OUTPUT
          #ifdef SLOW_START_STOP
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
        #ifdef PWM_OUTPUT
          #ifdef SLOW_START_STOP
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
        #ifdef PWM_OUTPUT
          #ifdef SLOW_START_STOP
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
        #ifdef PWM_OUTPUT
          #ifdef SLOW_START_STOP
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

enum rdir rotation_way (int angle_from, int angle_to) {
  if (angle_to > angle_from) {
    return CW;
  }
  else {
    return CCW;
  }
} /* END rotation_way() */

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
#endif /* DEBUG */
