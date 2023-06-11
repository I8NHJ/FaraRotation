// NEXTION CONTROL STRINGS
const char STOP_CMD[]                      = "*0";            // PLUS 1 char for antenna 0=RX, 1=TX, 2=ALL  // Completed
const char ROTATE_RX_ANTENNA_CW_CMD[]      = "*RRX1";                                                       // Completed
const char ROTATE_RX_ANTENNA_CCW_CMD[]     = "*RRX2";                                                       // Completed
const char ROTATE_RX_ANTENNA_TO_CMD[]      = "*GRX";          // Plus up to 3 chars for degrees (-90/90) 3  // Completed
const char ROTATE_TX_ANTENNA_CW_CMD[]      = "*RTX1";                                                       // Completed
const char ROTATE_TX_ANTENNA_CCW_CMD[]     = "*RTX2";                                                       // Completed
const char ROTATE_TX_ANTENNA_TO_CMD[]      = "*GTX";          // Plus up to 3 chars for degrees (-90/90) 3  // Completed
const char SET_TARGET_GRID_CMD[]           = "*TG";           // Plus up to 6 chars for GRID                // Completed 
const char SYNC_TX_2_RX_ENABLE_CMD[]       = "*SRXTX1";                                                     // Completed
// const char SYNC_TX_2_RX_DISABLE_CMD[]      = "*SRXTX0";                                                     // Enabled, NO LOGIC, WE DON"T NEED IT IF IT IS NOT A TOGLE
const char LINK_RX_AND_TX_ENABLE_CMD[]     = "*LRXTX1";                                                     // Completed
const char LINK_RX_AND_TX_DISABLE_CMD[]    = "*LRXTX0";                                                     // Completed
const char PTT_AUTOMATION_ENABLED_CMD[]    = "*PTT1";                                                       // Enabled, No Logic
const char PTT_AUTOMATION_DISABLED_CMD[]   = "*PTT0";                                                       // Enabled, No Logic
const char CALIBRATE_RX_ANTENNA_CW_CMD[]   = "*CRX1";                                                       // Completed
const char CALIBRATE_RX_ANTENNA_CCW_CMD[]  = "*CRX2";                                                       // Completed
const char CALIBRATE_TX_ANTENNA_CW_CMD[]   = "*CTX1";                                                       // Completed
const char CALIBRATE_TX_ANTENNA_CCW_CMD[]  = "*CTX2";                                                       // Completed
const char SAVE_CALLSIGN_TO_EEPROM_CMD[]   = "*SC";           // Plus up to 10 chars for callsign           // Completed 
const char SAVE_GRID_TO_EEPROM_CMD[]       = "*SG";           // Plus up to 6 chars for GRID                // Completed 
const char SEND_CONFIG_CMD[]               = "*CFG";                                                        // Completed

// NEXTION COMMAND STRINGS
const char NEXTION_POST_COMMAND[] = "ÿÿÿ";