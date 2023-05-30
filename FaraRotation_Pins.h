/*-------- RX Antenna pins definitions --------*/
#define rx_rotate_cw_enable 16       // goes high to activate rotator CW rotation
#define rx_rotate_ccw_enable 17      // goes high to activate rotator CCW rotation
#define rx_rotate_cw_ccw  0         // goes high for both CW and CCW rotation
#define rx_rotate_cw_pwm 5          // optional - PWM CW output - set to 0 to disable (must be PWM capable pin) - JUST USE DIGITAL OR PWM
#define rx_rotate_ccw_pwm 6         // optional - PWM CCW output - set to 0 to disable (must be PWM capable pin)
#define rx_rotate_cw_dig 5          // optional - DIGITAL CW output - set to 0 to disable (must be PWM capable pin)
#define rx_rotate_ccw_dig 6         // optional - DIGITAL CCW output - set to 0 to disable (must be PWM capable pin)
#define rx_button_cw 0              // normally open button to ground for manual CW rotation
#define rx_button_ccw 0             // normally open button to ground for manual CCW rotation 
#define rx_rotator_degs_pin A0      // reads analog voltage from rotator - pin 4 on Yaesu connector

/*-------- TX Antenna pins definitions --------*/
#define tx_rotate_cw_enable 18       // goes high to activate rotator CW rotation
#define tx_rotate_ccw_enable 19      // goes high to activate rotator CCW rotation
#define tx_rotate_cw_ccw 0          // goes high for both CW and CCW rotation
#define tx_rotate_cw_pwm 7          // optional - PWM CW output - set to 0 to disable (must be PWM capable pin)
#define tx_rotate_ccw_pwm 8         // optional - PWM CCW output - set to 0 to disable (must be PWM capable pin)
#define tx_rotate_cw_dig 7          // optional - DIGITAL CW output - set to 0 to disable (must be PWM capable pin)
#define tx_rotate_ccw_dig 8         // optional - DIGITAL CCW output - set to 0 to disable (must be PWM capable pin)
#define tx_button_cw 0              // normally open button to ground for manual CW rotation
#define tx_button_ccw 0             // normally open button to ground for manual CCW rotation
#define tx_rotator_degs_pin A1      // reads analog voltage from rotator

#define ptt_link 2                  // If enabled activates automatic TX/RX sync rotation for Faraday compensation, 0 to disable 