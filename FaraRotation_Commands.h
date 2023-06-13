// NEXTION COMMAND STRINGS
const char STOP_CMD[]                      = "*0";            // PLUS 1 char for antenna 0=RX, 1=TX, 2=ALL      Example: *02  
const char ROTATE_RX_ANTENNA_CW_CMD[]      = "*RRX1";                                                       
const char ROTATE_RX_ANTENNA_CCW_CMD[]     = "*RRX2";                                                       
const char ROTATE_RX_ANTENNA_TO_CMD[]      = "*GRX";          // Plus up to 3 chars for degrees (-90/90)        Example: *GRX15   
const char ROTATE_TX_ANTENNA_CW_CMD[]      = "*RTX1";                                                       
const char ROTATE_TX_ANTENNA_CCW_CMD[]     = "*RTX2";                                                       
const char ROTATE_TX_ANTENNA_TO_CMD[]      = "*GTX";          // Plus up to 3 chars for degrees (-90/90)        Example: *GTX-45  
const char SET_TARGET_GRID_CMD[]           = "*TG";           // Plus up to 6 chars for GRID                    Example: *TGJN71hn                 
const char SYNC_TX_2_RX_ENABLE_CMD[]       = "*SRXTX1";                                                     
// const char SYNC_TX_2_RX_DISABLE_CMD[]      = "*SRXTX0";    // Disabled. No Logic. WE DON'T NEED IT IF IT IS NOT A TOGLE
const char LINK_RX_AND_TX_ENABLE_CMD[]     = "*LRXTX1";                                                     
const char LINK_RX_AND_TX_DISABLE_CMD[]    = "*LRXTX0";                                                     
const char PTT_AUTOMATION_ENABLED_CMD[]    = "*PTT1";                                                       
const char PTT_AUTOMATION_DISABLED_CMD[]   = "*PTT0";                                                       
const char CALIBRATE_RX_ANTENNA_CW_CMD[]   = "*CRX1";                                                       
const char CALIBRATE_RX_ANTENNA_CCW_CMD[]  = "*CRX2";                                                       
const char CALIBRATE_TX_ANTENNA_CW_CMD[]   = "*CTX1";                                                       
const char CALIBRATE_TX_ANTENNA_CCW_CMD[]  = "*CTX2";                                                       
const char SAVE_CALLSIGN_TO_EEPROM_CMD[]   = "*SC";           // Plus up to 10 chars for My Callsign              Example: *SCN5NHJ            
const char SAVE_GRID_TO_EEPROM_CMD[]       = "*SG";           // Plus up to 6 chars for My GRID                   Example: *SGEM13pc                 
const char SEND_CONFIG_CMD[]               = "*CFG";                                                        
