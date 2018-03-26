/*
 *This is a Simple Code of 1.8 TFT Screen for Arduino UNO
 */
#include <Adafruit_GFX.h>
#include "SPI.h"
#include "Adafruit_ST7735.h"

#define TFT_vcc 6
#define TFT_led 0
#define TFT_CS A1
#define TFT_RST A2
#define TFT_DC  A3
#define TFT_SDA 11
#define TFT_SCK 13

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

int rect1x = 5;
int rect1y = 35;
int rect2y = 57;
int rect3y = 79;
int rect4y = 101;
int recwidth = 150;
int recheight = 20;



Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


//void blockColour(int block, String color) {
//  if (block == 1) {
//    tft.fillRect(rect1x, rect1y, recwidth, recheight, color);
//  }
//  if (block == 2) {
//    tft.fillRect(rect1x, rect2y, recwidth, recheight, color);
//  }
//  if (block == 3) {
//    tft.fillRect(rect1x, rect3y, recwidth, recheight, color);
//  }
//  if (block == 4) {
//    tft.fillRect(rect1x, rect4y, recwidth, recheight, color);
//  }
//}



void setup() {

  Serial.println(115200);

  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);

  tft.fillScreen(ST7735_CYAN);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(10, 10);
  tft.println("TRAFFIC CONTROL SYSTEM");
  delay(1000);


  // Black Rect
  tft.drawRect(rect1x-1 , rect1y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect1y, recwidth, recheight, BLACK);

  tft.drawRect(rect1x -1 , rect2y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect2y, recwidth, recheight, BLACK);

  tft.drawRect(rect1x -1 , rect3y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect3y, recwidth, recheight, BLACK);

  tft.drawRect(rect1x -1 , rect4y - 1 , recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect4y, recwidth, recheight, BLACK);

  // Full Fill REC

  // BLOCK 1
  tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
  // BLOCK 2
  tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
  // BLOCK 3
  tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
  // BLOCK 4
  tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);

  tft.setCursor(60, 40);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("GEC");

  tft.setCursor(60, 65);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("BAIZID");

  tft.setCursor(60, 85);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("MURADPUR");

  tft.setCursor(60, 107);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.print("PROBARTAK ");

  delay(3000);
}

void loop() {

}
