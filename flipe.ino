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
#define ATTRACT_MODE_DENSITY 20

// The pins and BAUD rate to use for the RS485 port
#define RS485_TX 3
#define RS485_RX 2
#define RS485_BAUD 9600
SoftwareSerial panelPort(RS485_RX, RS485_TX);

unsigned long int nextRefreshAt, lastRefreshedAt, lastUpdatedAt, idleAt;

bool letterMap[COLUMNS][ROWS];

// Minimum time between display updates in MS
#define REFRESH_FREQUENCY 250
// Time after the last sensor update before we go to attract mode
#define IDLE_TIMEOUT 3000

const byte PANEL_HEADER = 0x80u;
const byte  PANEL_WRITE_CMD = 0x84u;
const byte  PANEL_REFRESH_CMD = 0x82u;
const byte  PANEL_END = 0x8Fu;

void refreshDisplay() {
  byte displayMessage[3];
  int messageIndex = 0;
  displayMessage[messageIndex++] = PANEL_HEADER;
  displayMessage[messageIndex++] = PANEL_REFRESH_CMD;
  displayMessage[messageIndex++] =  PANEL_END;
  writeToPanels(displayMessage, messageIndex);
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
    writeToPanels(displayMessage, messageIndex);
  }
}

void writeToPanels(byte message[], int messageLength) {
  #ifdef _DEBUG
  for (int i=0; i<messageLength; i++) {
    if (message[i] < 0x10) {
      Serial.print("0");
    }
    Serial.print(message[i],HEX);
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
  letterMap[16][3] = true;
  letterMap[15][3] = true;
  letterMap[14][3] = true;
  letterMap[13][3] = true;
  letterMap[12][3] = true;
  letterMap[11][3] = true;
  letterMap[10][3] = true;
  letterMap[18][4] = true;
  letterMap[17][4] = true;
  letterMap[16][4] = true;
  letterMap[15][4] = true;
  letterMap[14][4] = true;
  letterMap[13][4] = true;
  letterMap[12][4] = true;
  letterMap[11][4] = true;
  letterMap[10][4] = true;
  letterMap[9][4] = true;
  letterMap[8][4] = true;
  letterMap[20][5] = true;
  letterMap[19][5] = true;
  letterMap[18][5] = true;
  letterMap[17][5] = true;
  letterMap[16][5] = true;
  letterMap[15][5] = true;
  letterMap[14][5] = true;
  letterMap[13][5] = true;
  letterMap[12][5] = true;
  letterMap[11][5] = true;
  letterMap[10][5] = true;
  letterMap[9][5] = true;
  letterMap[8][5] = true;
  letterMap[7][5] = true;
  letterMap[21][6] = true;
  letterMap[20][6] = true;
  letterMap[19][6] = true;
  letterMap[18][6] = true;
  letterMap[17][6] = true;
  letterMap[16][6] = true;
  letterMap[15][6] = true;
  letterMap[14][6] = true;
  letterMap[13][6] = true;
  letterMap[12][6] = true;
  letterMap[11][6] = true;
  letterMap[10][6] = true;
  letterMap[9][6] = true;
  letterMap[8][6] = true;
  letterMap[7][6] = true;
  letterMap[6][6] = true;
  letterMap[22][7] = true;
  letterMap[21][7] = true;
  letterMap[20][7] = true;
  letterMap[19][7] = true;
  letterMap[18][7] = true;
  letterMap[17][7] = true;
  letterMap[16][7] = true;
  letterMap[15][7] = true;
  letterMap[14][7] = true;
  letterMap[13][7] = true;
  letterMap[12][7] = true;
  letterMap[11][7] = true;
  letterMap[10][7] = true;
  letterMap[9][7] = true;
  letterMap[8][7] = true;
  letterMap[7][7] = true;
  letterMap[6][7] = true;
  letterMap[5][7] = true;
  letterMap[23][8] = true;
  letterMap[22][8] = true;
  letterMap[21][8] = true;
  letterMap[20][8] = true;
  letterMap[19][8] = true;
  letterMap[18][8] = true;
  letterMap[17][8] = true;
  letterMap[16][8] = true;
  letterMap[15][8] = true;
  letterMap[14][8] = true;
  letterMap[13][8] = true;
  letterMap[12][8] = true;
  letterMap[11][8] = true;
  letterMap[10][8] = true;
  letterMap[9][8] = true;
  letterMap[8][8] = true;
  letterMap[7][8] = true;
  letterMap[6][8] = true;
  letterMap[5][8] = true;
  letterMap[4][8] = true;
  letterMap[24][9] = true;
  letterMap[23][9] = true;
  letterMap[22][9] = true;
  letterMap[21][9] = true;
  letterMap[20][9] = true;
  letterMap[19][9] = true;
  letterMap[18][9] = true;
  letterMap[14][9] = true;
  letterMap[13][9] = true;
  letterMap[12][9] = true;
  letterMap[11][9] = true;
  letterMap[10][9] = true;
  letterMap[9][9] = true;
  letterMap[8][9] = true;
  letterMap[7][9] = true;
  letterMap[6][9] = true;
  letterMap[5][9] = true;
  letterMap[4][9] = true;
  letterMap[3][9] = true;
  letterMap[24][10] = true;
  letterMap[23][10] = true;
  letterMap[22][10] = true;
  letterMap[21][10] = true;
  letterMap[20][10] = true;
  letterMap[19][10] = true;
  letterMap[14][10] = true;
  letterMap[13][10] = true;
  letterMap[12][10] = true;
  letterMap[11][10] = true;
  letterMap[10][10] = true;
  letterMap[9][10] = true;
  letterMap[8][10] = true;
  letterMap[7][10] = true;
  letterMap[6][10] = true;
  letterMap[5][10] = true;
  letterMap[4][10] = true;
  letterMap[3][10] = true;
  letterMap[25][11] = true;
  letterMap[24][11] = true;
  letterMap[23][11] = true;
  letterMap[22][11] = true;
  letterMap[21][11] = true;
  letterMap[20][11] = true;
  letterMap[14][11] = true;
  letterMap[13][11] = true;
  letterMap[12][11] = true;
  letterMap[11][11] = true;
  letterMap[10][11] = true;
  letterMap[7][11] = true;
  letterMap[6][11] = true;
  letterMap[5][11] = true;
  letterMap[4][11] = true;
  letterMap[3][11] = true;
  letterMap[2][11] = true;
  letterMap[25][12] = true;
  letterMap[24][12] = true;
  letterMap[23][12] = true;
  letterMap[22][12] = true;
  letterMap[21][12] = true;
  letterMap[15][12] = true;
  letterMap[14][12] = true;
  letterMap[13][12] = true;
  letterMap[12][12] = true;
  letterMap[11][12] = true;
  letterMap[10][12] = true;
  letterMap[6][12] = true;
  letterMap[5][12] = true;
  letterMap[4][12] = true;
  letterMap[3][12] = true;
  letterMap[2][12] = true;
  letterMap[25][13] = true;
  letterMap[24][13] = true;
  letterMap[23][13] = true;
  letterMap[22][13] = true;
  letterMap[21][13] = true;
  letterMap[15][13] = true;
  letterMap[14][13] = true;
  letterMap[13][13] = true;
  letterMap[12][13] = true;
  letterMap[11][13] = true;
  letterMap[6][13] = true;
  letterMap[5][13] = true;
  letterMap[4][13] = true;
  letterMap[3][13] = true;
  letterMap[2][13] = true;
  letterMap[25][14] = true;
  letterMap[24][14] = true;
  letterMap[23][14] = true;
  letterMap[22][14] = true;
  letterMap[21][14] = true;
  letterMap[16][14] = true;
  letterMap[15][14] = true;
  letterMap[14][14] = true;
  letterMap[13][14] = true;
  letterMap[12][14] = true;
  letterMap[11][14] = true;
  letterMap[6][14] = true;
  letterMap[5][14] = true;
  letterMap[4][14] = true;
  letterMap[3][14] = true;
  letterMap[2][14] = true;
  letterMap[25][15] = true;
  letterMap[24][15] = true;
  letterMap[23][15] = true;
  letterMap[22][15] = true;
  letterMap[21][15] = true;
  letterMap[16][15] = true;
  letterMap[15][15] = true;
  letterMap[14][15] = true;
  letterMap[13][15] = true;
  letterMap[12][15] = true;
  letterMap[6][15] = true;
  letterMap[5][15] = true;
  letterMap[4][15] = true;
  letterMap[3][15] = true;
  letterMap[2][15] = true;
  letterMap[25][16] = true;
  letterMap[24][16] = true;
  letterMap[23][16] = true;
  letterMap[22][16] = true;
  letterMap[21][16] = true;
  letterMap[16][16] = true;
  letterMap[15][16] = true;
  letterMap[14][16] = true;
  letterMap[13][16] = true;
  letterMap[12][16] = true;
  letterMap[6][16] = true;
  letterMap[5][16] = true;
  letterMap[4][16] = true;
  letterMap[3][16] = true;
  letterMap[2][16] = true;
  letterMap[25][17] = true;
  letterMap[24][17] = true;
  letterMap[23][17] = true;
  letterMap[22][17] = true;
  letterMap[21][17] = true;
  letterMap[17][17] = true;
  letterMap[16][17] = true;
  letterMap[15][17] = true;
  letterMap[14][17] = true;
  letterMap[13][17] = true;
  letterMap[6][17] = true;
  letterMap[5][17] = true;
  letterMap[4][17] = true;
  letterMap[3][17] = true;
  letterMap[2][17] = true;
  letterMap[24][18] = true;
  letterMap[23][18] = true;
  letterMap[22][18] = true;
  letterMap[21][18] = true;
  letterMap[20][18] = true;
  letterMap[17][18] = true;
  letterMap[16][18] = true;
  letterMap[15][18] = true;
  letterMap[14][18] = true;
  letterMap[13][18] = true;
  letterMap[6][18] = true;
  letterMap[5][18] = true;
  letterMap[4][18] = true;
  letterMap[3][18] = true;
  letterMap[2][18] = true;
  letterMap[24][19] = true;
  letterMap[23][19] = true;
  letterMap[22][19] = true;
  letterMap[21][19] = true;
  letterMap[20][19] = true;
  letterMap[19][19] = true;
  letterMap[18][19] = true;
  letterMap[17][19] = true;
  letterMap[16][19] = true;
  letterMap[15][19] = true;
  letterMap[14][19] = true;
  letterMap[7][19] = true;
  letterMap[6][19] = true;
  letterMap[5][19] = true;
  letterMap[4][19] = true;
  letterMap[3][19] = true;
  letterMap[2][19] = true;
  letterMap[23][20] = true;
  letterMap[22][20] = true;
  letterMap[21][20] = true;
  letterMap[20][20] = true;
  letterMap[19][20] = true;
  letterMap[18][20] = true;
  letterMap[17][20] = true;
  letterMap[16][20] = true;
  letterMap[15][20] = true;
  letterMap[14][20] = true;
  letterMap[8][20] = true;
  letterMap[7][20] = true;
  letterMap[6][20] = true;
  letterMap[5][20] = true;
  letterMap[4][20] = true;
  letterMap[3][20] = true;
  letterMap[22][21] = true;
  letterMap[21][21] = true;
  letterMap[20][21] = true;
  letterMap[19][21] = true;
  letterMap[18][21] = true;
  letterMap[17][21] = true;
  letterMap[16][21] = true;
  letterMap[15][21] = true;
  letterMap[9][21] = true;
  letterMap[8][21] = true;
  letterMap[7][21] = true;
  letterMap[6][21] = true;
  letterMap[5][21] = true;
  letterMap[4][21] = true;
  letterMap[3][21] = true;
  letterMap[21][22] = true;
  letterMap[20][22] = true;
  letterMap[19][22] = true;
  letterMap[18][22] = true;
  letterMap[17][22] = true;
  letterMap[16][22] = true;
  letterMap[15][22] = true;
  letterMap[9][22] = true;
  letterMap[8][22] = true;
  letterMap[7][22] = true;
  letterMap[6][22] = true;
  letterMap[5][22] = true;
  letterMap[4][22] = true;
  letterMap[20][23] = true;
  letterMap[19][23] = true;
  letterMap[18][23] = true;
  letterMap[17][23] = true;
  letterMap[16][23] = true;
  letterMap[8][23] = true;
  letterMap[7][23] = true;
  letterMap[6][23] = true;
  letterMap[5][23] = true;
  letterMap[19][24] = true;
  letterMap[18][24] = true;
  letterMap[17][24] = true;
  letterMap[16][24] = true;
  letterMap[7][24] = true;
  letterMap[6][24] = true;
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
    idleAt = millis() + IDLE_TIMEOUT;
  }
  bool updated = setDisplayForSensor();
  if (updated) {
    lastUpdatedAt = millis();
    idleAt = lastUpdatedAt + IDLE_TIMEOUT;
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
    nextRefreshAt = lastRefreshedAt + REFRESH_FREQUENCY;
  }
}
