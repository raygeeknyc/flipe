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

// Set all dots on a specified panel
void setPanel(int panelIdx, bool dotsOn) {
  for (int x=0; x<COLUMNS; x++) {
    for (int y=0; y<ROWS; y++) {
      setPanelDot(panelIdx, x, y, dotsOn);
    }
  }
}

// Set all dots on all panels
void setPanels(bool dotsOn) {
  for (int p=0; p<PANELS; p++) {
    setPanel(p, dotsOn);
  }
}

void setup() {
  setPanels(false);
}

void loop() {
}
