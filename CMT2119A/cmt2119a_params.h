#ifndef __CMT2119A_PARAMS_H
#define __CMT2119A_PARAMS_H
#include "gpio.h"


/*
;---------------------------------------
;  CMT2119A Configuration File
;  Configuration File
;  2025.08.22 10:35
;---------------------------------------
;  Mode                = Basic
;  Part Number         = CMT2119A
;  Frequency           = 433.920 MHz
;  Modulation          = FSK
;  Symbol Rate         = 0.5-100.0 ksps
;  Tx Power            = 0 dBm
;  PA Ramping Time     = NA
;  Deviation           = 20.0 kHz
;  Xtal Cload          = 15.00 pF
;  Tx Start by         = DATA Pin Rising Edge
;  Tx Stop by          = DATA Pin Holding Low for 20 ms
;  Increase XO Current = No
;  FILE CRC            = A15E

;---------------------------------------
;  The following are the EEPROM contents
;---------------------------------------
*/

const uint16_t g_cmt2119aCfg[21] = {
0x007F,
0x5400,
0x0000,
0x0000,
0x0000,
0xF000,
0x0000,
0xC1C5,
0x4208,
0x00C9,
0x2401,
0x0081,
0x8000,
0x0000,
0xFFFF,
0x0020,
0x4F08,
0xA2D6,
0x0E13,
0x0019,
0x0000,
};




#endif
