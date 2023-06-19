/*-------- RX Antenna pins definitions --------*/
/* USE DIGITAL OR PWM ACCORDINGLY WITH FaraRotation_Settings.h */
#define rx_rotate_cw_enable 16      // goes high to activate rotator CW rotation (For activating the H-Bridge board if not managed in hardware)
#define rx_rotate_ccw_enable 17     // goes high to activate rotator CCW rotation (For activating the H-Bridge board if not managed in hardware)
#define rx_rotate_cw_pwm 5          // PWM CW output - must be PWM capable pin PWM: 2 to 13 and 44 to 46 for ATMEGA2560 REV 3. 
#define rx_rotate_ccw_pwm 6         // PWM CCW output - must be PWM capable pin)
#define rx_rotate_cw_dig 5          // DIGITAL CW output 
#define rx_rotate_ccw_dig 6         // DIGITAL CCW output 
#define rx_rotator_degs_pin A0      // reads analog voltage from rotator 

/*-------- TX Antenna pins definitions --------*/
/* USE DIGITAL OR PWM ACCORDINGLY WITH FaraRotation_Settings.h */
#define tx_rotate_cw_enable 18      // goes high to activate rotator CW rotation (For activating the H-Bridge board if not managed in hardware)
#define tx_rotate_ccw_enable 19     // goes high to activate rotator CCW rotation (For activating the H-Bridge board if not managed in hardware)
#define tx_rotate_cw_pwm 7          // PWM CW output -  must be PWM capable pin PWM: 2 to 13 and 44 to 46 for ATMEGA2560 REV 3.
#define tx_rotate_ccw_pwm 8         // PWM CCW output - set to 0 to disable (must be PWM capable pin)
#define tx_rotate_cw_dig 7          // DIGITAL CW output 
#define tx_rotate_ccw_dig 8         // DIGITAL CCW output 
#define tx_rotator_degs_pin A1      // reads analog voltage from rotator

/*-------- PTT Feature --------*/
#define ptt_automation 2                  // If enabled activates automatic TX/RX sync rotation for Faraday compensation
