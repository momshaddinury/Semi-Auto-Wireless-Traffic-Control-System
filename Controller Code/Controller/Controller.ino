/*
  Project Name : Lora Traffic Control System
  Developers : Towqir Ahmed Shaem , Momshad Dinury
  Email : towqirahmedshaem@gmail.com, md.dinury@gmail.com
  Company : Stellar Technology, Bangladesh
*/

//Including Library:
#include <SX1278.h>
#include <SPI.h>

// Including Library for 1.8 TFT DispLay
#include <Adafruit_GFX.h>
#include "SPI.h"
#include "Adafruit_ST7735.h"

//// TFT Display Pin For Arduino
#define TFT_CS  5
#define TFT_RST 17
#define TFT_DC  0

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
char testData[50];

//Pin def
#define pushbutton A0
#define button1 2
#define button2 16


// DEBUG
boolean debug_1  = true;
boolean debug_2  = true;
boolean debug_3  = true;
boolean debug_4  = true;

// TIMER DEBUG FLAG:
boolean timer_debug_1 = false;
boolean timer_debug_2 = false;
boolean timer_debug_3 = false;
boolean timer_debug_4 = false;

// Button State
boolean button1State = true;
boolean button2State = true;
boolean button3State = true;
boolean button4State = true;

int value3,value4;
boolean value1,value2;



// Defining Timing Variable
int interval = 800;

long int currentTime ;


long int previousButton1Time ;
long int previousButton2Time ;
long int previousButton3Time ;
long int previousButton4Time ;


//#define debug false

void setup() {

  Serial.begin(115200);
  SPI.begin();
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
  tft.fillRect(rect1x, rect1y, recwidth, recheight, YELLOW);
  // BLOCK 2
  tft.fillRect(rect1x, rect2y, recwidth, recheight, YELLOW);
  // BLOCK 3
  tft.fillRect(rect1x, rect3y, recwidth, recheight, YELLOW);
  // BLOCK 4
  tft.fillRect(rect1x, rect4y, recwidth, recheight, YELLOW);

  //Location Names in display (Max 4 location)
  //Location name can be  changed according to location preference.
  //Location -1: GEC
  tft.setCursor(60, 40);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("GEC"); 

  //Location -2 :Baizid
  tft.setCursor(60, 65);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("BAIZID");

  //Location -3 :Muradpur
  tft.setCursor(60, 85);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("MURADPUR");

  //Location -4 :PROBARTAK
  tft.setCursor(60, 107);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("PROBARTAK ");


  // Lora Initialization
  loraSetup();

  //PinMode:
  pinMode(pushbutton, INPUT);
  pinMode(button1,INPUT);
  pinMode(button2,INPUT);


}

void loop() {

  value1 = digitalRead(button1);
  //Serial.println(value1);
  value2 = digitalRead(button2);
  //Serial.println(value2);
  value3 = analogRead(pushbutton);
  //Serial.println(value3);
  //delay(1000);
  pushButton();  
  digitalWrite(button1, HIGH);
  digitalWrite(button2, HIGH);
}


void pushButton() {
   currentTime = millis();
  
   int button1HighValue = 500;
   int button2HighValue = 500;
   int button3HighValue = 1000;
   int button4HighValue = 450;

   int button3LowValue =  500;
   int button4LowValue = 150;
   

  if (value1 == false && debug_1 == true) {
    Serial.print("Push button 1 is pressed. ");
    Serial.println(value1);
    //String("Push button 1 is pressed").toCharArray(testData, 50);

    timer_debug_1 = true;
    debug_1 = false;

    //sendData(testData);
    
    if (button1State) {

      String("G").toCharArray(testData, 50);
      sendData(testData);

      tft.fillRect(rect1x, rect1y, recwidth, recheight, GREEN);
      tft.setCursor(60, 40);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("GEC");

      button1State = false;
    }
    else {

      String("R").toCharArray(testData, 50);
      sendData(testData);

      tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
      tft.setCursor(60, 40);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("GEC");

      button1State = true;
    }
    previousButton1Time = millis();

  
  } else if ( value2 == false && debug_2 == true) {

    Serial.print("Push button 2 is pressed. ");
    Serial.println(value2);
    //String("Push button 2 is pressed").toCharArray(testData, 50);

    timer_debug_2 = true; 
    debug_2 = false;
  
    //sendData(testData);
    
    if (button2State) {

      String("G").toCharArray(testData, 50);
      sendData(testData);

      tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
      tft.setCursor(60, 65);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("BAIZID");

      button2State = false;
    }
    else {

      String("R").toCharArray(testData, 50);
      sendData(testData);

      tft.fillRect(rect1x, rect2y, recwidth, recheight, RED);
      tft.setCursor(60, 65);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("BAIZID");

      button2State = true;
    }

    previousButton2Time = millis();


 
  } 

  else if (value3 > button3LowValue && value3 < button3HighValue && debug_3 == true) {
    Serial.println(value3);
    Serial.print("Push button 3 is pressed. ");
    Serial.println(value3);
    // String("Push button 3 is pressed").toCharArray(testData, 50);

    timer_debug_3 = true;

    debug_3 = false;
   
    // sendData(testData);
    
    if (button3State) {

      String("G").toCharArray(testData, 50);
      sendData(testData);

      tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
      tft.setCursor(60, 85);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("MURADPUR");
      button3State = false;
    }
    else {

      String("R").toCharArray(testData, 50);
      sendData(testData);

      tft.fillRect(rect1x, rect3y, recwidth, recheight, RED);
      tft.setCursor(60, 85);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("MURADPUR");
      button3State = true;
    }
    previousButton3Time = millis();


  
  } 
  else if (value3 > button4LowValue && value3 < button4HighValue && debug_4 == true) {
    Serial.println(value3);
    Serial.print("Push button 4 is pressed. ");
    Serial.println(value3);
    // String("Push button 4 is pressed").toCharArray(testData, 50);

    timer_debug_4 = true;

    debug_4 = false;

    // sendData(testData);
    
    if (button4State) {

      String("G").toCharArray(testData, 50);
      sendData(testData);

      tft.fillRect(rect1x, rect4y, recwidth, recheight, GREEN);
      tft.setCursor(60, 107);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("PROBARTAK ");

      button4State = false;
    }
    else {

      String("R").toCharArray(testData, 50);
      sendData(testData);

      tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);
      tft.setCursor(60, 107);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("PROBARTAK ");

      button4State = true;
    }
    previousButton4Time = millis();


  }
  

  if ( (currentTime - previousButton1Time > interval ) && timer_debug_1 == true) {

    Serial.println("Time over & Flag 1 is true");
    debug_1 = true;
    timer_debug_1 = false;
    previousButton1Time = millis();

    #ifdef debug
    Serial.print("Button 1 Flag Time : ");
    Serial.println(previousButton1Time);
    #endif
  }
  if ( ( currentTime - previousButton2Time > interval ) && timer_debug_2 == true) {

    Serial.println("Time over & Flag 2 is true");
    debug_2 = true;
    timer_debug_2 = false;
    previousButton2Time = millis();

    #ifdef debug
    Serial.print("Button 2 Flag Time : ");
    Serial.println(previousButton2Time);
    #endif
  }
  
  if ( (currentTime - previousButton3Time > interval ) && timer_debug_3 == true) {

    Serial.println("Time over & Flag 3 is true");
    debug_3 = true;
    timer_debug_3 = false;
    previousButton3Time = millis();

    #ifdef debug
    Serial.print("Button 3 Flag Time : ");
    Serial.println(previousButton3Time);
    #endif
  }
  
  if ( (currentTime - previousButton4Time > interval ) && timer_debug_4 == true) {

    Serial.println("Time over & Flag 4 is true");
    debug_4 = true;
    timer_debug_4 = false;
    previousButton4Time = millis();

    #ifdef debug
    Serial.print("Button 4 Flag Time : ");
    Serial.println(previousButton4Time);
    #endif
  }


}

void sendData(char message[]) {
  int state;
  state = sx1278.sendPacketTimeoutACK(2, message);
  if (state == 0)
  {
    Serial.println(F("State = 0 --> Command Executed w no errors!"));
    Serial.println(F("Packet sent....."));
  }
  else {
    Serial.println(state);
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


