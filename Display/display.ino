#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

#define TFT_CS   10
#define TFT_DC    8
#define TFT_RST  -1 

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

struct Item {
  String name;
  String location;
};

Item pouchItems[] = {
  {"Medkit", "Front"}, {"Battery", "Middle"}, {"Compass", "Back"},
  {"Map Case", "Left"}, {"Multi-tool", "Front"}, {"Flashlight", "Middle"},
  {"Protein Bar", "Back"}, {"Water Filter", "Left"}
};

const int itemCount = 8;
const int itemsPerPage = 5; 
int currentStartIndex = 0;

void setup() {
  tft.begin(2000000); 
  tft.setRotation(1);
  tft.setTextWrap(false);

  // Draw the "Permanent" UI once
  tft.fillScreen(ILI9341_WHITE);
  tft.fillRect(0, 0, 320, 40, ILI9341_NAVY);
  tft.setCursor(10, 10);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("POUCH INVENTORY");
}

void loop() {
  updateInventoryList();
  
  delay(3000); 

  if (itemCount > itemsPerPage) {
    currentStartIndex++;
    if (currentStartIndex > (itemCount - itemsPerPage)) {
      currentStartIndex = 0;
    }
  }
}

void updateInventoryList() {
  int yPos = 50; 

  for (int i = 0; i < itemsPerPage; i++) {
    int idx = currentStartIndex + i;
    
    // 1. ERASE ONLY THIS ROW
    // We draw a white box over the old text area before writing new text
    // We keep the width to 220 to stay inside your "square" safe zone
    tft.fillRect(10, yPos, 220, 30, ILI9341_WHITE); 

    if (idx < itemCount) {
      // 2. DRAW NEW TEXT
      tft.setCursor(20, yPos);
      tft.setTextColor(ILI9341_BLACK);
      tft.setTextSize(2);
      tft.print(pouchItems[idx].name);
      
      tft.setCursor(170, yPos + 5);
      tft.setTextColor(ILI9341_RED);
      tft.setTextSize(1);
      tft.print("["); tft.print(pouchItems[idx].location); tft.print("]");
      
      // Separator
      tft.drawFastHLine(10, yPos + 25, 220, ILI9341_LIGHTGREY);
    }
    yPos += 40; 
  }
}