
#include <boardiface.hpp>

//TODO: clean up this mess

//SoftwareSerial dimSerial(10, 11); // Anschluß am Dimmermodul RX, TX

Dimmer dimmer;


char payload[PAYLOAD_SIZE];
static boolean recvInProgress = false;

static boolean newData = false;
static boolean wait_for_ACK = false;

unsigned long currentMillis = 0;
unsigned long BasicMillis = 0;
unsigned int flop = 0;
unsigned long timeOutMillis = 0;
const long timeOut = 100;
byte timeCnt = 0;
//************************************************************************************
#if USE_CRC_TABLE

const uint8_t crc8_table[256] PROGMEM = {
  0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
  0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
  0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
  0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
  0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
  0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
  0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
  0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
  0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
  0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
  0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
  0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
  0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
  0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
  0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
  0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
  0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
  0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
  0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
  0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
  0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
  0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
  0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
  0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
  0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
  0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
  0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
  0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
  0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
  0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
  0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
  0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3

};
//************************************************************************************
uint8_t crc8(const char *p, uint8_t  length) {
  uint8_t  crc8;
  uint8_t  i;
  uint8_t  offset;
  crc8 = 0;

  for (i = 0; i < length; i++) {
    offset = crc8 ^ *(p + i);
    crc8 = pgm_read_byte(&(crc8_table[offset]));
  }
  return crc8 ^ 0x55;
}
#else
//************************************************************************************
uint8_t crc8(const char *p, uint8_t  length) {
  const uint8_t  table[] = { 0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
                             0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D
                           };

  const uint8_t  table_rev[] = { 0x00, 0x70, 0xE0, 0x90, 0xC1, 0xB1, 0x21, 0x51,
                                 0x83, 0xF3, 0x63, 0x13, 0x42, 0x32, 0xA2, 0xD2
                               };

  uint8_t  crc8; uint8_t  i; uint8_t  offset; uint8_t  temp, crc8_temp;
  crc8 = 0x00;

  for (i = 0; i < length; i++) {
    temp = *(p + i);
    offset = temp ^ crc8;
    offset >>= 4;
    crc8_temp = crc8 & 0x0f;
    crc8 = crc8_temp ^ table_rev[offset];
    offset = crc8 ^ temp;
    offset &= 0x0f;
    crc8_temp = crc8 & 0xf0;
    crc8 = crc8_temp ^ table[offset];
  }
  return crc8 ^ 0x55;
}
#endif

//************************************************************************************
void serialData(byte len_Of_Payload) {
  uint8_t Tmp;
  Tmp = payload[0];
  //Serial.print("Serial in->"); Serial.println(payload);

  switch (Tmp) {
    //*********************************************************************************
    case GET_VALUES:
        if (len_Of_Payload > 9) {
            dimmer.version_major = payload[1];
            dimmer.version_minor = payload[2];
            dimmer.gate1_on = payload[3];
            dimmer.gate1_dimm = payload[4];
            dimmer.gate1_bright_tbl = payload[5];
            //dimmer.gate1_bright_proz = (uint8_t)((dimmer.gate1_bright_tbl * 100) / 255) + 0.5;
            dimmer.gate2_on = payload[6];
            dimmer.gate2_dimm = payload[7];
            dimmer.gate2_bright_tbl = payload[8];
            //dimmer.gate2_bright_proz = (uint8_t)((dimmer.gate2_bright_tbl * 100) / 255) + 0.5;
            dimmer.gate_lock = payload[9];
        }
        else {
            dimmer.version_major = 1;
            dimmer.version_minor = 0;

            dimmer.gate1_on = payload[1] - 48;
            dimmer.gate1_dimm = payload[2] - 48;
            dimmer.gate2_on = payload[4] - 48;
            dimmer.gate2_dimm = payload[5] - 48;
            dimmer.gate_lock = payload[7] - 48;

            //dimmer.gate1_bright_proz = (uint8_t)((dimmer.gate1_bright_tbl * 100) / 255) + 0.5;
            //dimmer.gate2_bright_proz = (uint8_t)((dimmer.gate2_bright_tbl * 100) / 255) + 0.5;
        }

        break;

  } // switch

  newData = false;

}
//*****************************************************************************************
// String format
// 1. byte  = startMarker
// 2. byte  = crc
// 3. byte  = len_Of_Payload
// 4. bytes = 1. byte = cmd
//                 2. byte = payload (data)
void recv() {
  static byte ndx = 0;
  static byte cnt = 0;
  static byte crc = 0;
  static byte len_Of_Payload;

  byte chksum = 0;
  byte cmd;
  static byte rc;

  while (Serial.available() > 0) {
    rc = Serial.read();

    if (recvInProgress == true) {

      switch (cnt) {

      case 0:
        crc = rc;
        cnt++;
        break;

      case 1:
        len_Of_Payload = rc;
        cnt++;
        break;

      case 2:
        payload[ndx] = (uint8_t)rc;
        ndx++;

        if (ndx >= PAYLOAD_SIZE) ndx = PAYLOAD_SIZE - 1;
        if (len_Of_Payload >= PAYLOAD_SIZE) len_Of_Payload = PAYLOAD_SIZE - 1;

        if ( ndx >= len_Of_Payload ) {
          payload[ndx] = '\0'; // terminate the string
          cnt = 0;
          ndx = 0;
          recvInProgress = false;
          chksum = crc8(payload, len_Of_Payload);
          if (chksum == crc) {
            Serial.write((byte)0xFF); //send ACK
            newData = true;
          }
          else {
            Serial.write((byte)0x00); //send NoACK
          }
        }
        break;

      }// switch

    }
    else {
      if (rc == 0x7B) {
        recvInProgress = true;
        cnt = 0;
        ndx = 0;
      }
    }// if (recvInProgress == true)

  }// while (Serial.available() > 0)

  if (newData) {
    serialData(len_Of_Payload);
    recvInProgress = false;
  }
}
//************************************************************************************
void send_cmd(char* sendCMD, boolean wait_for_ACK) {
  char *s = sendCMD;
  int rc;
  static const long timeOut = 200;
  byte timeCnt = 0;
  static unsigned long timeOutMillis;

  //Serial.print("Serial out->"); Serial.print(s); Serial.print("\n");

  recvInProgress = false;
  Serial.print(s);
  timeOutMillis = millis();

  while (wait_for_ACK) {

    if (millis() - timeOutMillis <= timeOut) {
      if (Serial.available() > 0) {
        rc = Serial.read();
        if (rc == 0xFF) {
          wait_for_ACK = false;
        }
      }
    }
    else {
      if (timeCnt > 3) {
        wait_for_ACK = false;
        //Serial.print("->Timeout!\n");
      }
      else {
        timeCnt++;
        //Serial.print("->resend\n");
        Serial.print(s);
        Serial.flush();
        timeOutMillis = millis();
      }
    }

  }
}
//*****************************************************************************************
int send_seriell(const char *value) {

  byte startMarker = 0x7B;
  char payload[PAYLOAD_SIZE];
  strcpy(payload, value);
  byte len_Of_Payload = strlen(payload);
  byte crc = crc8(payload, len_Of_Payload);
  char header[] = { startMarker, crc, len_Of_Payload, '\0' };
  char values[PAYLOAD_SIZE];

  strcpy(values, header);
  strcat(values, payload);

  send_cmd(values, true);

}

// Beispiel
//************************************************************************************
void set_dimm1_tbl(uint8_t value) {

  char sendVal[3];

  sendVal[0] = DIMM_1_TBL;
  sendVal[1] = value;
  sendVal[2] = '\0';
  send_seriell(sendVal);
  dimmer.gate1_bright_tbl = value;
  if(value == 0){
    // if dim level is 0, attiny switches off gate
    dimmer.gate1_on = 0;
  }
}
uint8_t get_dim1(){
  return dimmer.gate1_bright_tbl;
}

void set_dimm2_tbl(uint8_t value) {

  char sendVal[3];

  sendVal[0] = DIMM_2_TBL;
  sendVal[1] = value;
  sendVal[2] = '\0';
  send_seriell(sendVal);
  dimmer.gate2_bright_tbl = value;
  if(value == 0){
    // if dim level is 0, attiny switches off gate
    dimmer.gate2_on = 0;
  }
}
uint8_t get_dim2(){
  return dimmer.gate2_bright_tbl;
}

uint8_t get_dim(uint8_t gate){
  if (gate == 1){
    return dimmer.gate1_bright_tbl;
  }
  else{
    return dimmer.gate2_bright_tbl;
  }
}

//************************************************************************************
void get_values(void) {
  //Serial.print("Get modul values\n");

  char value[3];
  value[0] = GET_VALUES;
  value[1] = '\0';

  send_seriell(value);
}
int get_gate1(){
  return dimmer.gate1_on;
}
//************************************************************************************
int set_gate1(int value) {
  if (value == dimmer.gate1_on) {
    //Serial.println("value == dimmer.gate1_on -> return\n");
    return 1;
  }
  //Serial.print("set_gate1("); Serial.print(value);Serial.println(")");

  char sendVal[3];

  if (!value) {
    sendVal[0] = GATE_1_OFF;
    dimmer.gate1_on = 0;
  }
  else {
    sendVal[0] = GATE_1_ON;
    dimmer.gate1_on = 1;
    if (dimmer.gate_lock) { dimmer.gate2_dimm = 0; set_gate2(0); }
  }
  sendVal[1] = '\0';

  send_seriell(sendVal);
  return 1;
}
int get_gate2(){
  return dimmer.gate2_on;
}
//************************************************************************************
int set_gate2(int value) {
  if (value == dimmer.gate2_on) {
    //Serial.println("value == dimmer.gate1_on -> return\n");
    return 1;
  }
  //Serial.print("set_gate1("); Serial.print(value);Serial.println(")");

  char sendVal[3];

  if (!value) {
    sendVal[0] = GATE_2_OFF;
    dimmer.gate2_on = 0;
  }
  else {
    sendVal[0] = GATE_2_ON;
    dimmer.gate2_on = 1;
    if (dimmer.gate_lock) { dimmer.gate1_dimm = 0; set_gate1(0); }
  }
  sendVal[1] = '\0';

  send_seriell(sendVal);
  return 1;
}

void set_channel_lock(uint8_t value){
  char sendVal[3];
  sendVal[0] = CH_LOCK;
  sendVal[1] = value;
  sendVal[2] = '\0';
  send_seriell(sendVal);
  dimmer.gate_lock = value;
}
void write_eeprom(){
  //TODO: test
  char value[3];
  value[0] = WRITE_EE;
  value[1] = '\0';

  send_seriell(value);
}
void read_eeprom(){
  //TODO: test
  char value[3];
  value[0] = READ_EE;
  value[1] = '\0';

  send_seriell(value);
}

//************************************************************************************
void board_setup() {
  // Dimmer Port
  Serial.begin(9600);
  Serial.flush();


  // Debug Port
  //Serial.begin(115200);
  //Serial.println("Lets go!");


  BasicMillis = millis();
}
//************************************************************************************
void board_loop() { // run over and over

  recv();

  currentMillis = millis();
  if(currentMillis - BasicMillis > 5000){
    BasicMillis = currentMillis;
    //flop=1-flop;
    //set_gate1(flop);
    get_values();
  }

}

Dimmer get_state(){
  return dimmer;
}
