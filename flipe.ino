#include <SoftwareSerial.h>

#define ROWS 7
#define COLUMNS 28
#define PANELS 4

#define RS485_TX 2
#define RS485_RX 3
#define RS485_BAUD 9600
SoftwareSerial panelPort(RS485_RX, RS485_TX);

// Minimum time between display updates in MS
#define REFRESH_FREQUENCY 500
// Time after the last sensor update before we go to attract mode
#define IDLE_TIMEOUT 3000

typedef struct _column { bool row[ROWS]; } _column;
typedef struct _panel { _column column[COLUMNS]; } _panel;
struct kiosk { _panel panel[PANELS]; } kiosk;

#define PANEL_HEADER 0x80
#define PANEL_WRITE_CMD 0x84
#define PANEL_REFRESH_CMD 0x82
#define PANEL_END 0x8F

// Set a specified dot on a specified panel
void setPanelDot(int panelIdx, int x, int y, bool dotOn) {
  kiosk.panel[panelIdx].column[x].row[y] = dotOn;
}

// Get a specified dot on a specified panel
bool getPanelDot(int panelIdx, int x, int y) {
  return kiosk.panel[panelIdx].column[x].row[y];
}

// Set all dots on a specified panel
void setPanel(int panelIdx, bool dotsOn) {
  for (int x=0; x<COLUMNS; x++) {
    for (int y=0; y<ROWS; y++) {
      setPanelDot(panelIdx, x, y, dotsOn);
    }
  }
}

char** emitPanel(int panelIdx) {
  char **panel = new(char*[28]);
  for (int x=0; x<COLUMNS; x++) {
    char *row = new(char[7]);
    panel[x] = row;
    for (int y=0; y<ROWS; y++) {
      row[y]=getPanelDot(panelIdx, x, y)?'X':' ';
    }
  }
  return panel;
}

void refreshPanels() {
  String displayMessage = "";
  displayMessage+=PANEL_HEADER;
  displayMessage+=PANEL_REFRESH_CMD;
  displayMessage+=PANEL_END;
  writeToPanels(displayMessage);
}

void writeDisplayToPanels() {
  for (int p=0; p<PANELS; p++) {
    String displayMessage = "";
    displayMessage+=PANEL_HEADER;
    displayMessage+=PANEL_WRITE_CMD;
    displayMessage+=byte(p);
    for (int x=0; x<COLUMNS; x++) {
      byte val = 0x00;
      for (int y=0; y<ROWS; y++) {
        if (getPanelDot(p, x, y)) {
          val |= y;
        }
      }
      displayMessage += val;
    }
    displayMessage+=PANEL_END;
    writeToPanels(displayMessage);
  }
}

void writeToPanels(String message) {
  panelPort.print(message);    
}

// Set all dots on all panels
void setPanels(bool dotsOn) {
  for (int p=0; p<PANELS; p++) {
    setPanel(p, dotsOn);
  }
}

void setup() {
  setPanels(false);
  setPanelDot(0, 2, 2, true);
  setPanel(1, true);
  setPanelDot(1, 2, 2, false);
  panelPort.begin(RS485_BAUD);
  Serial.begin(9600);
  Serial.println("/setup");
  dumpPanels();
}

/** Call this once the panels contents are set and you wish to display them
 */
void updateDisplay() {
  writeDisplayToPanels();
  refreshPanels();
}

void dumpPanels() {
  for (int p=0; p<PANELS; p++) {
    dumpPanel(p);
  }
}
void dumpPanel(int panelIdx) {
  Serial.print("<");
  Serial.print(panelIdx);
  Serial.println(">");
  char** panel = emitPanel(panelIdx);
  for (int x=0; x<COLUMNS; x++) {
    for (int y=0; y<ROWS; y++) {
      Serial.print(panel[x][y]);
    }
    Serial.println();
  }
  Serial.print("</");
  Serial.print(panelIdx);
  Serial.println(">");
}

int getSensorValue() {
  //TODO(raymond) get a sensor value here from USB and return the int
  //return -1 if no value was read
  return 10;
}

bool setPanelsFromSensor() {
  int sensorReading = getSensorValue();
  if (sensorReading < 0) {
    return false;
  }
  //TODO(raymond) set the panels here
  return true;
}

void setPanelIdlePattern() {
  //TODO(raymond) Set some random pixels outside of the "e" to on, set the "e" off
}

void loop() {
  unsigned long int nextRefreshAt, lastRefreshed, lastUpdated, idleAt;
  bool updated = setPanelsFromSensor();
  if (updated) {
    lastUpdated = millis();
    idleAt = lastUpdated + IDLE_TIMEOUT;
  }
  if (millis() > idleAt && idleAt > 0) {
    setPanelIdlePattern();
  }
  if (millis() > nextRefreshAt && nextRefreshAt > 0) {
    updateDisplay();
    lastRefreshed = millis();
    nextRefreshAt = lastRefreshed + REFRESH_FREQUENCY;    
  }
}
