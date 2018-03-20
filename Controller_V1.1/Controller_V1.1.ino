//Including Library:
#include <SX1278.h>
#include <SPI.h>

// Including Library for 1.8 TFT DispLay
#include <Adafruit_GFX.h>
#include "SPI.h"
#include "Adafruit_ST7735.h"

// TFT Display Pin For Arduino
#define TFT_vcc 6
#define TFT_led 0
#define TFT_CS A1
#define TFT_RST A2
#define TFT_DC  A3
#define TFT_SDA 11
#define TFT_SCK 13

// Color Code For Tft Display
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Variable For Display
int rect1x = 5;
int rect1y = 35;
int rect2y = 57;
int rect3y = 79;
int rect4y = 101;
int recwidth = 150;
int recheight = 20;

// Creating Object of TFT Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Lora SX1278:
#define LORA_MODE             10
#define LORA_CHANNEL          CH_6_BW_125
#define LORA_ADDRESS          4
#define LORA_SEND_TO_ADDRESS  2

char my_packet [50];
char testData[50]; //Transmits data to Gateway

//Pin def
#define pushbutton A0
// DEBUG
boolean debug_1  = true;
boolean debug_2  = true;
boolean debug_3  = true;
boolean debug_4  = true;

// Button State
boolean button1State = true;
boolean button2State = true;
boolean button3State = true;
boolean button4State = true;

int value;

// Defining Timing Variable

int interval = 1000;
long int currentButton1Time = 0;
long int currentButton2Time = 0;
long int currentButton3Time = 0;
long int currentButton4Time = 0;

long int previousButton1Time = 0;
long int previousButton2Time = 0;
long int previousButton3Time = 0;
long int previousButton4Time = 0;


void setup() {
  Serial.begin(115200);

  // Diaplay Initialization and Creating BLock
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);

  tft.fillScreen(ST7735_CYAN);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(10, 10);
  tft.println("TRAFFIC CONTROL SYSTEM");
  delay(1000);


  // Black Rect
  tft.drawRect(rect1x - 1 , rect1y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect1y, recwidth, recheight, BLACK);

  tft.drawRect(rect1x - 1 , rect2y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect2y, recwidth, recheight, BLACK);

  tft.drawRect(rect1x - 1 , rect3y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect3y, recwidth, recheight, BLACK);

  tft.drawRect(rect1x - 1 , rect4y - 1 , recwidth + 2, recheight, BLACK);
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


  // Lora Initialization
  loraSetup();
  pinMode(pushbutton, INPUT);
}

void loop() {

  value = analogRead(pushbutton);
  //Serial.println(value);
  pushButton();

}


void pushButton() {

  if (value > 350 && value < 410 && debug_1 == true) {

    Serial.println("Push button 1 is pressed");
    String("Push button 1 is pressed").toCharArray(testData, 50);
    debug_1 = false;
    debug_2 = true;
    debug_3 = true;
    debug_4 = true;
    sendData(testData);
    
    if (button1State) {
      tft.fillRect(rect1x, rect1y, recwidth, recheight, GREEN);
      button1State = false;
    }
    else {
      tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
      button1State = true;
    }
    currentButton1Time = millis();

  } else if (value > 230 && value < 280 && debug_2 == true) {

    Serial.println("Push button 2 is pressed");
    String("Push button 2 is pressed").toCharArray(testData, 50);
    debug_1 = true;
    debug_2 = false;
    debug_3 = true;
    debug_4 = true;
    sendData(testData);
    
    if (button2State) {
      tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
      button2State = false;
    }
    else {
      tft.fillRect(rect1x, rect2y, recwidth, recheight, RED);
      button2State = true;
    }

    currentButton2Time = millis();

  } else if (value > 120 && value < 180 && debug_3 == true) {

    Serial.println("Push button 3 is pressed");
    String("Push button 3 is pressed").toCharArray(testData, 50);
    debug_1 = true;
    debug_2 = true;
    debug_3 = false;
    debug_4 = true;
    sendData(testData);
    
    if (button3State) {
      tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
      button3State = false;
    }
    else {
      tft.fillRect(rect1x, rect3y, recwidth, recheight, RED);
      button3State = true;
    }
    currentButton3Time = millis();

  } else if (value > 70 && value < 100 && debug_4 == true) {

    Serial.println("Push button 4 is pressed");
    String("Push button 4 is pressed").toCharArray(testData, 50);
    debug_1 = true;
    debug_2 = true;
    debug_3 = true;
    debug_4 = false;
    sendData(testData);
    
    if (button4State) {
      tft.fillRect(rect1x, rect4y, recwidth, recheight, GREEN);
      button4State = false;
    }
    else {
      tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);
      button4State = true;
    }
    currentButton4Time = millis();

  }
  if ( currentButton1Time - previousButton1Time > interval ) {
    debug_1 = true;
    previousButton1Time = millis();
  }
  if ( currentButton2Time - previousButton2Time > interval ) {
    debug_2 = true;
    previousButton2Time = millis();
  }
  if ( currentButton3Time - previousButton3Time > interval ) {
    debug_3 = true;
    previousButton3Time = millis();
  }
  if ( currentButton4Time - previousButton4Time > interval ) {
    debug_4 = true;
    previousButton4Time = millis();
  }


}

void sendData(char message[]) {
  if (!sx1278.sendPacketTimeout(0, message))
  {
    Serial.println(F("Packet sent....."));
  }
  else {
    Serial.println(F("Packet not sent....."));
  }
}

void recieveData() {

  int  z = sx1278.receivePacketTimeoutACK(5000);
  if (z == 0) {
    delay(10);
    Serial.println(F("Package received!"));

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
    Serial.print(F("Message:  "));
    Serial.println(my_packet);
  }
}

void loraSetup() {
  // Power ON the module:
  if (sx1278.ON() == 0) {
    Serial.println(F("Setting power ON: SUCCESS "));
  } else {
    Serial.println(F("Setting power ON: ERROR "));
  }

  // Set transmission mode and print the result:
  if (sx1278.setMode(LORA_MODE) == 0) {
    Serial.println(F("Setting Mode: SUCCESS "));
  } else {
    Serial.println(F("Setting Mode: ERROR "));
  }

  // Set header:
  if (sx1278.setHeaderON() == 0) {
    Serial.println(F("Setting Header ON: SUCCESS "));
  } else {
    Serial.println(F("Setting Header ON: ERROR "));
  }

  // Select frequency channel:
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
    Serial.println(F("Setting Channel: SUCCESS "));
  } else {
    Serial.println(F("Setting Channel: ERROR "));
  }

  // Set CRC:
  if (sx1278.setCRC_ON() == 0) {
    Serial.println(F("Setting CRC ON: SUCCESS "));
  } else {
    Serial.println(F("Setting CRC ON: ERROR "));
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
    Serial.println(F("Setting Power: SUCCESS "));
  } else {
    Serial.println(F("Setting Power: ERROR "));
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
    Serial.println(F("Setting node address: SUCCESS "));
  } else {
    Serial.println(F("Setting node address: ERROR "));
  }

  // Print a success
  Serial.println(F("SX1278 configured finished"));
  Serial.println();
}

