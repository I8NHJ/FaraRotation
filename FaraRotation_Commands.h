// NEXTION CONTROL STRINGS
const char STOP[]  = "*0";                                                                              // Completed
const char ROTATE_RX_ANTENNA_CW[]      = "*RRX1";                                                       // Completed
const char ROTATE_RX_ANTENNA_CCW[]     = "*RRX2";                                                       // Completed
const char ROTATE_RX_ANTENNA_TO[]      = "*GRX";          // Plus up to 3 chars for degrees (-90/90) 3  // Enabled
const char ROTATE_TX_ANTENNA_CW[]      = "*RTX1";                                                       // Completed
const char ROTATE_TX_ANTENNA_CCW[]     = "*RTX2";                                                       // Completed
const char ROTATE_TX_ANTENNA_TO[]      = "*GTX";          // Plus up to 3 chars for degrees (-90/90) 3  // Enabled
const char SET_TARGET_GRID[]           = "*TG";           // Plus up to 6 chars for GRID                // Completed 
const char SYNC_TX_2_RX_ENABLE[]       = "*SRXTX1";                                                     // Enabled, DO WE NEED A TOGLE? 
const char SYNC_TX_2_RX_DISABLE[]      = "*SRXTX0";                                                     // Enabled, NO LOGIC, WE DON"T NEED IT IF IT IS NOT A TOGLE
const char LINK_RX_AND_TX_ENABLE[]     = "*LRXTX1";                                                     // Completed
const char LINK_RX_AND_TX_DISABLE[]    = "*LRXTX0";                                                     // Completed
const char PTT_AUTOMATION_ENABLED[]    = "*PTT1";                                                       // Enabled, No Logic
const char PTT_AUTOMATION_DISABLED[]   = "*PTT0";                                                       // Enabled, No Logic
const char CALIBRATE_RX_ANTENNA_CW[]   = "*CRX1";                                                       // Completed
const char CALIBRATE_RX_ANTENNA_CCW[]  = "*CRX2";                                                       // Completed
const char CALIBRATE_TX_ANTENNA_CW[]   = "*CTX1";                                                       // Completed
const char CALIBRATE_TX_ANTENNA_CCW[]  = "*CTX2";                                                       // Completed
const char SAVE_CALLSIGN_TO_EEPROM[]   = "*SC";           // Plus up to 10 chars for callsign           // Completed 
const char SAVE_GRID_TO_EEPROM[]       = "*SG";           // Plus up to 6 chars for GRID                // Completed 
const char SEND_CONFIG[]               = "*CFG";                                                        // Completed

// NEXTION COMMAND STRINGS
const char NEXTION_POST_COMMAND[] = "ÿÿÿ";