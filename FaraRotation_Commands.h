// NEXTION CONTROL STRINGS
const char STOP[]  = "*0";
const char ROTATE_RX_ANTENNA_CW[]      = "*RRX1";
const char ROTATE_RX_ANTENNA_CCW[]     = "*RRX2";
const char ROTATE_TX_ANTENNA_CW[]      = "*RTX1";
const char ROTATE_TX_ANTENNA_CCW[]     = "*RTX2";
const char SYNC_TX_2_RX[]              = "*RTX3";
const char ROTATE_RX_AND_TX_ENABLE[]   = "*RRXTX1";
const char ROTATE_RX_AND_TX_DISABLE[]  = "*RRXTX0";
const char CALIBRATE_RX_ANTENNA_CW[]   = "*CRX1";
const char CALIBRATE_RX_ANTENNA_CCW[]  = "*CRX2";
const char CALIBRATE_TX_ANTENNA_CW[]   = "*CTX1";
const char CALIBRATE_TX_ANTENNA_CCW[]  = "*CTX2";
const char SAVE_CALLSIGN_TO_EEPROM[]   = "*SC";
const char SAVE_GRID_TO_EEPROM[]       = "*SG";
const char SEND_CONFIG[]               = "*CFG";

// NEXTION COMMAND STRINGS
const char NEXTION_POST_COMMAND[] = "ÿÿÿ";

// COMMAND ENUMERATORS
const byte ROTATE_RX_ANTENNA_CW_ENU  = 1;
const byte ROTATE_RX_ANTENNA_CCW_ENU = 2;
const byte ROTATE_TX_ANTENNA_CW_ENU  = 3;
const byte ROTATE_TX_ANTENNA_CCW_ENU = 4;
