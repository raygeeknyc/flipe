#include <SoftwareSerial.h>

#define ROWS 7
#define COLUMNS 28
#define PANELS 4

typedef struct _column { bool row[ROWS]; } _column;
typedef struct _panel { _column column[COLUMNS]; } _panel;
struct kiosk { _panel panel[PANELS]; } kiosk;

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
  Serial.begin(9600);
  Serial.println("/setup");
  dumpPanel(0);
  dumpPanel(1);
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

void loop() {
}
