#include <SoftwareSerial.h>

#define _DEBUG

#define PANEL_ROWS 7
#define PANEL_COLUMNS 28
#define PANELS 4

const int ROWS = PANEL_ROWS * PANELS;
const int COLUMNS = PANEL_COLUMNS;

#define SENSOR_MIN 0
#define SENSOR_MAX 99
float y_scaling_factor;

// between 0 and 99, the "percentage" of attract mode pixels to turn on
#define ATTRACT_MODE_DENSITY 40

// The pins and BAUD rate to use for the RS485 port
#define RS485_TX 3
#define RS485_RX 2
#define RS485_BAUD 9600
SoftwareSerial panelPort(RS485_RX, RS485_TX);

unsigned long int nextRefreshAt, lastRefreshedAt, lastUpdatedAt, idleAt;

bool letterMap[COLUMNS][ROWS];

// Minimum time between display updates in MS
#define REFRESH_FREQUENCY_MS 250
// Time after the last sensor update before we go to attract mode
#define IDLE_TIMEOUT_MS 3000

const byte PANEL_HEADER = 0x80u;
const byte  PANEL_WRITE_CMD = 0x84u;
const byte  PANEL_REFRESH_CMD = 0x82u;
const byte  PANEL_END = 0x8Fu;

// Track the high watermark for sensor values and expire them after a period of time
int sensorHighWatermark;
unsigned long int highWaterMarkAt;
#define SENSOR_HIGHWATERMARK_TIMEOUT_MS 1500

void refreshDisplay() {
  byte displayMessage[3];
  int messageIndex = 0;
  displayMessage[messageIndex++] = PANEL_HEADER;
  displayMessage[messageIndex++] = PANEL_REFRESH_CMD;
  displayMessage[messageIndex++] =  PANEL_END;
  writeToPanel(displayMessage, messageIndex);
}

byte panelAddress(int panelIdx) {
  return byte(panelIdx);
}

bool getDot(bool idle, int x, int y) {
  bool letterPixel = letterMap[x][y];
  if (idle) {
    if (letterPixel) {
      letterPixel = !letterPixel;
    } else {
      letterPixel = (random(0,100) < ATTRACT_MODE_DENSITY?true:false);
    }
  }
  return letterPixel;
}

// Scale the sensor value to the row, flip the orientation as needed.
// Return true if this row should be visible for the current sensorValue
bool showRow(int row, int sensorValue) {
  bool visible = (ROWS - row) <= int(sensorValue * y_scaling_factor + 1);
  return visible;
}

void setDisplay(bool idle, int sensorValue) {
  // TODO(raymond) Maintain and plot an aged high watermark for the VU meter effect
  byte displayMessage[31];
  int messageIndex;

  for (int p = 0; p < PANELS; p++) {
    messageIndex = 0;
    displayMessage[messageIndex++] = PANEL_HEADER;
    displayMessage[messageIndex++] = PANEL_WRITE_CMD;
    displayMessage[messageIndex++] = byte(panelAddress(p));
    for (int x = 0; x < PANEL_COLUMNS; x++) {
      byte val = 0x00;
      for (int y = 0; y < PANEL_ROWS; y++) {
        if (showRow(y+(p*PANEL_ROWS), sensorValue)) {
          if (getDot(idle, x, y+(p*PANEL_ROWS))) {
            val = val | (1 << (y+1) - 1);
          }
        }
      }
      displayMessage[messageIndex++] = byte(val);
    }
    displayMessage[messageIndex++] = PANEL_END;
    writeToPanel(displayMessage, messageIndex);
  }
}

void writeToPanel(byte message[], int messageLength) {
  #ifdef _DEBUG
  Serial.print("message:");
  Serial.println(messageLength);
  int msgPos;
  for (msgPos=0; msgPos < sizeof(PANEL_END)+sizeof(PANEL_WRITE_CMD)+sizeof(byte);msgPos++) {
    Serial.print(message[msgPos], HEX);
  }
  Serial.println();
  int msgBodyStart = msgPos;
  for (int i=0;i<PANEL_ROWS;i++) {
    Serial.print('\t');
    msgPos = msgBodyStart;
    for (;msgPos<messageLength-sizeof(PANEL_END); msgPos++) {
      Serial.print(((message[msgPos] >> i) & 1)?"O":" ");
    }
    Serial.println();
  }
  for (;msgPos<messageLength; msgPos++) {
    Serial.print(message[msgPos],HEX);
  }
  Serial.println();
 #else
  panelPort.write(message, messageLength);
  #endif
}

// Set the letter map
void _initLetterMap(bool val) {
  // Set all dots off
  for (int x = 0; x < COLUMNS; x++) {
    for (int y = 0; y < ROWS; y++) {
      letterMap[x][y] = val;
    }
  }
}

// Set the letter map
void _setLetterMap() {
  _initLetterMap(false);
  // Set the dots of an 'e' on
  letterMap[0][0] = true;
  letterMap[1][1] = true;
  letterMap[2][2] = true;
  letterMap[3][3] = true;
  letterMap[4][4] = true;
  letterMap[5][5] = true;
  letterMap[6][6] = true;
  letterMap[7][7] = true;
  letterMap[8][8] = true;
  letterMap[9][9] = true;
}

// Set the letter map
void setLetterMap() {
  _initLetterMap(false);
  // Set the dots of an 'e' on
  letterMap[3][11] = true;
  letterMap[3][12] = true;
  letterMap[3][13] = true;
  letterMap[3][14] = true;
  letterMap[3][15] = true;
  letterMap[3][16] = true;
  letterMap[3][17] = true;
  letterMap[4][9] = true;
  letterMap[4][10] = true;
  letterMap[4][11] = true;
  letterMap[4][12] = true;
  letterMap[4][13] = true;
  letterMap[4][14] = true;
  letterMap[4][15] = true;
  letterMap[4][16] = true;
  letterMap[4][17] = true;
  letterMap[4][18] = true;
  letterMap[4][19] = true;
  letterMap[5][7] = true;
  letterMap[5][8] = true;
  letterMap[5][9] = true;
  letterMap[5][10] = true;
  letterMap[5][11] = true;
  letterMap[5][12] = true;
  letterMap[5][13] = true;
  letterMap[5][14] = true;
  letterMap[5][15] = true;
  letterMap[5][16] = true;
  letterMap[5][17] = true;
  letterMap[5][18] = true;
  letterMap[5][19] = true;
  letterMap[5][20] = true;
  letterMap[6][6] = true;
  letterMap[6][7] = true;
  letterMap[6][8] = true;
  letterMap[6][9] = true;
  letterMap[6][10] = true;
  letterMap[6][11] = true;
  letterMap[6][12] = true;
  letterMap[6][13] = true;
  letterMap[6][14] = true;
  letterMap[6][15] = true;
  letterMap[6][16] = true;
  letterMap[6][17] = true;
  letterMap[6][18] = true;
  letterMap[6][19] = true;
  letterMap[6][20] = true;
  letterMap[6][21] = true;
  letterMap[7][5] = true;
  letterMap[7][6] = true;
  letterMap[7][7] = true;
  letterMap[7][8] = true;
  letterMap[7][9] = true;
  letterMap[7][10] = true;
  letterMap[7][11] = true;
  letterMap[7][12] = true;
  letterMap[7][13] = true;
  letterMap[7][14] = true;
  letterMap[7][15] = true;
  letterMap[7][16] = true;
  letterMap[7][17] = true;
  letterMap[7][18] = true;
  letterMap[7][19] = true;
  letterMap[7][20] = true;
  letterMap[7][21] = true;
  letterMap[7][22] = true;
  letterMap[8][4] = true;
  letterMap[8][5] = true;
  letterMap[8][6] = true;
  letterMap[8][7] = true;
  letterMap[8][8] = true;
  letterMap[8][9] = true;
  letterMap[8][10] = true;
  letterMap[8][11] = true;
  letterMap[8][12] = true;
  letterMap[8][13] = true;
  letterMap[8][14] = true;
  letterMap[8][15] = true;
  letterMap[8][16] = true;
  letterMap[8][17] = true;
  letterMap[8][18] = true;
  letterMap[8][19] = true;
  letterMap[8][20] = true;
  letterMap[8][21] = true;
  letterMap[8][22] = true;
  letterMap[8][23] = true;
  letterMap[9][3] = true;
  letterMap[9][4] = true;
  letterMap[9][5] = true;
  letterMap[9][6] = true;
  letterMap[9][7] = true;
  letterMap[9][8] = true;
  letterMap[9][9] = true;
  letterMap[9][13] = true;
  letterMap[9][14] = true;
  letterMap[9][15] = true;
  letterMap[9][16] = true;
  letterMap[9][17] = true;
  letterMap[9][18] = true;
  letterMap[9][19] = true;
  letterMap[9][20] = true;
  letterMap[9][21] = true;
  letterMap[9][22] = true;
  letterMap[9][23] = true;
  letterMap[9][24] = true;
  letterMap[10][3] = true;
  letterMap[10][4] = true;
  letterMap[10][5] = true;
  letterMap[10][6] = true;
  letterMap[10][7] = true;
  letterMap[10][8] = true;
  letterMap[10][13] = true;
  letterMap[10][14] = true;
  letterMap[10][15] = true;
  letterMap[10][16] = true;
  letterMap[10][17] = true;
  letterMap[10][18] = true;
  letterMap[10][19] = true;
  letterMap[10][20] = true;
  letterMap[10][21] = true;
  letterMap[10][22] = true;
  letterMap[10][23] = true;
  letterMap[10][24] = true;
  letterMap[11][2] = true;
  letterMap[11][3] = true;
  letterMap[11][4] = true;
  letterMap[11][5] = true;
  letterMap[11][6] = true;
  letterMap[11][7] = true;
  letterMap[11][13] = true;
  letterMap[11][14] = true;
  letterMap[11][15] = true;
  letterMap[11][16] = true;
  letterMap[11][17] = true;
  letterMap[11][20] = true;
  letterMap[11][21] = true;
  letterMap[11][22] = true;
  letterMap[11][23] = true;
  letterMap[11][24] = true;
  letterMap[11][25] = true;
  letterMap[12][2] = true;
  letterMap[12][3] = true;
  letterMap[12][4] = true;
  letterMap[12][5] = true;
  letterMap[12][6] = true;
  letterMap[12][12] = true;
  letterMap[12][13] = true;
  letterMap[12][14] = true;
  letterMap[12][15] = true;
  letterMap[12][16] = true;
  letterMap[12][17] = true;
  letterMap[12][21] = true;
  letterMap[12][22] = true;
  letterMap[12][23] = true;
  letterMap[12][24] = true;
  letterMap[12][25] = true;
  letterMap[13][2] = true;
  letterMap[13][3] = true;
  letterMap[13][4] = true;
  letterMap[13][5] = true;
  letterMap[13][6] = true;
  letterMap[13][12] = true;
  letterMap[13][13] = true;
  letterMap[13][14] = true;
  letterMap[13][15] = true;
  letterMap[13][16] = true;
  letterMap[13][21] = true;
  letterMap[13][22] = true;
  letterMap[13][23] = true;
  letterMap[13][24] = true;
  letterMap[13][25] = true;
  letterMap[14][2] = true;
  letterMap[14][3] = true;
  letterMap[14][4] = true;
  letterMap[14][5] = true;
  letterMap[14][6] = true;
  letterMap[14][11] = true;
  letterMap[14][12] = true;
  letterMap[14][13] = true;
  letterMap[14][14] = true;
  letterMap[14][15] = true;
  letterMap[14][16] = true;
  letterMap[14][21] = true;
  letterMap[14][22] = true;
  letterMap[14][23] = true;
  letterMap[14][24] = true;
  letterMap[14][25] = true;
  letterMap[15][2] = true;
  letterMap[15][3] = true;
  letterMap[15][4] = true;
  letterMap[15][5] = true;
  letterMap[15][6] = true;
  letterMap[15][11] = true;
  letterMap[15][12] = true;
  letterMap[15][13] = true;
  letterMap[15][14] = true;
  letterMap[15][15] = true;
  letterMap[15][21] = true;
  letterMap[15][22] = true;
  letterMap[15][23] = true;
  letterMap[15][24] = true;
  letterMap[15][25] = true;
  letterMap[16][2] = true;
  letterMap[16][3] = true;
  letterMap[16][4] = true;
  letterMap[16][5] = true;
  letterMap[16][6] = true;
  letterMap[16][11] = true;
  letterMap[16][12] = true;
  letterMap[16][13] = true;
  letterMap[16][14] = true;
  letterMap[16][15] = true;
  letterMap[16][21] = true;
  letterMap[16][22] = true;
  letterMap[16][23] = true;
  letterMap[16][24] = true;
  letterMap[16][25] = true;
  letterMap[17][2] = true;
  letterMap[17][3] = true;
  letterMap[17][4] = true;
  letterMap[17][5] = true;
  letterMap[17][6] = true;
  letterMap[17][10] = true;
  letterMap[17][11] = true;
  letterMap[17][12] = true;
  letterMap[17][13] = true;
  letterMap[17][14] = true;
  letterMap[17][21] = true;
  letterMap[17][22] = true;
  letterMap[17][23] = true;
  letterMap[17][24] = true;
  letterMap[17][25] = true;
  letterMap[18][3] = true;
  letterMap[18][4] = true;
  letterMap[18][5] = true;
  letterMap[18][6] = true;
  letterMap[18][7] = true;
  letterMap[18][10] = true;
  letterMap[18][11] = true;
  letterMap[18][12] = true;
  letterMap[18][13] = true;
  letterMap[18][14] = true;
  letterMap[18][21] = true;
  letterMap[18][22] = true;
  letterMap[18][23] = true;
  letterMap[18][24] = true;
  letterMap[18][25] = true;
  letterMap[19][3] = true;
  letterMap[19][4] = true;
  letterMap[19][5] = true;
  letterMap[19][6] = true;
  letterMap[19][7] = true;
  letterMap[19][8] = true;
  letterMap[19][9] = true;
  letterMap[19][10] = true;
  letterMap[19][11] = true;
  letterMap[19][12] = true;
  letterMap[19][13] = true;
  letterMap[19][20] = true;
  letterMap[19][21] = true;
  letterMap[19][22] = true;
  letterMap[19][23] = true;
  letterMap[19][24] = true;
  letterMap[19][25] = true;
  letterMap[20][4] = true;
  letterMap[20][5] = true;
  letterMap[20][6] = true;
  letterMap[20][7] = true;
  letterMap[20][8] = true;
  letterMap[20][9] = true;
  letterMap[20][10] = true;
  letterMap[20][11] = true;
  letterMap[20][12] = true;
  letterMap[20][13] = true;
  letterMap[20][19] = true;
  letterMap[20][20] = true;
  letterMap[20][21] = true;
  letterMap[20][22] = true;
  letterMap[20][23] = true;
  letterMap[20][24] = true;
  letterMap[21][5] = true;
  letterMap[21][6] = true;
  letterMap[21][7] = true;
  letterMap[21][8] = true;
  letterMap[21][9] = true;
  letterMap[21][10] = true;
  letterMap[21][11] = true;
  letterMap[21][12] = true;
  letterMap[21][18] = true;
  letterMap[21][19] = true;
  letterMap[21][20] = true;
  letterMap[21][21] = true;
  letterMap[21][22] = true;
  letterMap[21][23] = true;
  letterMap[21][24] = true;
  letterMap[22][6] = true;
  letterMap[22][7] = true;
  letterMap[22][8] = true;
  letterMap[22][9] = true;
  letterMap[22][10] = true;
  letterMap[22][11] = true;
  letterMap[22][12] = true;
  letterMap[22][18] = true;
  letterMap[22][19] = true;
  letterMap[22][20] = true;
  letterMap[22][21] = true;
  letterMap[22][22] = true;
  letterMap[22][23] = true;
  letterMap[23][7] = true;
  letterMap[23][8] = true;
  letterMap[23][9] = true;
  letterMap[23][10] = true;
  letterMap[23][11] = true;
  letterMap[23][19] = true;
  letterMap[23][20] = true;
  letterMap[23][21] = true;
  letterMap[23][22] = true;
  letterMap[24][8] = true;
  letterMap[24][9] = true;
  letterMap[24][10] = true;
  letterMap[24][11] = true;
  letterMap[24][20] = true;
  letterMap[24][21] = true;
}

void setup() {
  y_scaling_factor = (float)ROWS / (float)(SENSOR_MAX - SENSOR_MIN);
  panelPort.begin(RS485_BAUD);
  Serial.begin(9600);
  setLetterMap();
}

int getSensorValue() {
  //TODO(raymond) get a sensor value here from USB and return the int
  //return -1 if no value was read
  int val = random(0,2)<1?-1:random(0,100);
  #ifdef _DEBUG
  if (val >= 0) {
    val = 99;
    Serial.print(val);
   Serial.println("-->");
  }
  #endif
  if (val >= sensorHighWatermark) {
    sensorHighWatermark = val;
    highWaterMarkAt = millis();
  }
  return val;
}

bool setDisplayForSensor() {
  int sensorReading = getSensorValue();
  if (sensorReading < 0) {
    return false;
  }
  setDisplay(false, sensorReading);
  return true;
}

void setDisplayForIdle() {
  setDisplay(true, 99);
}

void loop() {
  if (nextRefreshAt == 0) {
    nextRefreshAt = millis();
    idleAt = millis() + IDLE_TIMEOUT_MS;
  }
  bool updated = setDisplayForSensor();
  if (updated) {
    lastUpdatedAt = millis();
    idleAt = lastUpdatedAt + IDLE_TIMEOUT_MS;
  }
  if (millis() > idleAt && idleAt > 0) {
    #ifdef _DEBUG
    Serial.print ("idle:");
    Serial.println(millis());
    #endif
    setDisplayForIdle();
  }
  if (millis() > nextRefreshAt && nextRefreshAt > 0) {
    #ifdef _DEBUG
    Serial.print ("refresh:");
    Serial.println(millis());
    #endif
    refreshDisplay();
    lastRefreshedAt = millis();
    nextRefreshAt = lastRefreshedAt + REFRESH_FREQUENCY_MS;
  }
}
