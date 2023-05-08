/*-------- RX Antenna pins definitions --------*/
#define rx_rotate_cw_enable 6       // goes high to activate rotator CW rotation
#define rx_rotate_ccw_enable 7      // goes high to activate rotator CCW rotation
#define rx_rotate_cw_ccw  0         // goes high for both CW and CCW rotation
#define rx_rotate_cw_pwm 0          // optional - PWM CW output - set to 0 to disable (must be PWM capable pin)
#define rx_rotate_ccw_pwm 0         // optional - PWM CCW output - set to 0 to disable (must be PWM capable pin)
#define rx_button_cw 0              // normally open button to ground for manual CW rotation (schematic pin: A2)
#define rx_button_ccw 0             // normally open button to ground for manual CCW rotation (schematic pin: A3)
#define rx_rotator_degs_pin A0      // reads analog voltage from rotator - pin 4 on Yaesu connector

/*-------- TX Antenna pins definitions --------*/
#define tx_rotate_cw_enable 8       // goes high to activate rotator CW rotation
#define tx_rotate_ccw_enable 9      // goes high to activate rotator CCW rotation
#define tx_rotate_cw_ccw 0          // goes high for both CW and CCW rotation
#define tx_rotate_cw_pwm 0          // optional - PWM CW output - set to 0 to disable (must be PWM capable pin)
#define tx_rotate_ccw_pwm 0         // optional - PWM CCW output - set to 0 to disable (must be PWM capable pin)
#define tx_button_cw 0              // normally open button to ground for manual CW rotation
#define tx_button_ccw 0             // normally open button to ground for manual CCW rotation
#define tx_rotator_degs_pin A1      // reads analog voltage from rotator
