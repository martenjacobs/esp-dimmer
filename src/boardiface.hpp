#ifndef __BOARDIFACE_INCLUDED__
#define __BOARDIFACE_INCLUDED__

#include <Arduino.h>

#define GATE_1_ON    0x20
#define GATE_1_OFF    0x21
#define DIMM_1_ON   0x22
#define DIMM_1_OFF    0x23
#define DIMM_1_TBL    0x24
#define DIMM_1_VAL    0x25

#define GATE_2_ON   0x30
#define GATE_2_OFF    0x31
#define DIMM_2_ON   0x32
#define DIMM_2_OFF    0x33
#define DIMM_2_TBL    0x34
#define DIMM_2_VAL    0x35

#define GATES_ON    0x40
#define GATES_OFF   0x41

#define DIMMS_ON    0x50
#define DIMMS_OFF   0x51

#define CH_LOCK     0x60

#define GET_VALUES    0xFA

#define WRITE_EE    0xFC
#define READ_EE     0xFD

#define GET_VERSION   0xFE

#define RESET     0xFF
//************************************************************************************
#define USE_CRC_TABLE 0
#define PAYLOAD_SIZE 16
//************************************************************************************


typedef struct {
    uint8_t   version_major       = 0;
    uint8_t   version_minor       = 0;

    uint8_t   gate1_on            = 0;
//    uint8_t   gate1_bright_proz   = 0;
    uint8_t   gate1_bright_tbl    = 0;
    uint8_t   gate1_dimm          = 0;
    uint8_t   gate1_impuls_start  = 0;
    uint32_t  gate1_impuls_len    = 0;

    uint8_t   gate2_on            = 0;
//    uint8_t   gate2_bright_proz   = 0;
    uint8_t   gate2_bright_tbl    = 0;
    uint8_t   gate2_dimm          = 0;
    uint8_t   gate2_impuls_start  = 0;
    uint32_t  gate2_impuls_len    = 0;

    uint8_t   gate_lock           = 0;
} Dimmer;

Dimmer get_state();

void board_loop();
void board_setup();
int set_gate1(int val);
int set_gate2(int val);
int get_gate1();
int get_gate2();
void set_dimm1_tbl(uint8_t value);
void set_dimm2_tbl(uint8_t value);
uint8_t get_dim1();
uint8_t get_dim2();
void get_values(void);

#endif
