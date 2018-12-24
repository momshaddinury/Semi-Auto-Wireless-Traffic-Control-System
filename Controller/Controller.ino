// ---------------------------------------------------------------------
/*
  Project Name - Semi Automatic Traffic Control System using Lora SX1278
  Components used:
           MCU - ESP12E
           Communication module - Lora SX1278
           Indicator - LED Custom Made Strip
           Power Source - LC 18650
  Author - Momshad Dinury
  Contributor:
           Towqir Ahmed Shaem
           Abdullah Zawad Khan
*/
// ---------------------------------------------------------------------
/******************************************************************************
   Includes
 ******************************************************************************/
#include <Arduino.h>
//Library for 1.8 TFT DispLay:
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
//Lora SX1278:
#include <SX1278.h>
//Library for Ticker
#include <Ticker.h>
#include <U8g2lib.h>

/*****************************************************************************
   Definitions & Declarations
 *****************************************************************************/
//
//// TFT Display Pin For Arduino
#define TFT_CS 10
#define TFT_RST 16
#define TFT_DC 0
#define TFT_SCLK 14
#define TFT_MOSI 13

// Color Code For TFT Display
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

// Variable For TFT Display Rectangle
int rect1x = 4;
int rect1y = 25;
int rect2y = 47;
int rect3y = 69;
int rect4y = 91;
int recwidth = 122;
int recheight = 20;

U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/2, /* dc=*/23, /* reset=*/100); // Nokia 5110 Display
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g1(U8G2_R0, /* cs=*/4, /* dc=*/23, /* reset=*/100); // Nokia 5110 Display

// Creating Object of TFT Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Lora SX1278:
#define LORA_MODE 4
#define LORA_CHANNEL CH_6_BW_125
#define LORA_ADDRESS 5
uint8_t NodeAddress; //Child Address
int address;

//Packets var:
char my_packet[50];
char testData[50];
String receivedMsg;
int T_packet_state;
int R_packet_state;

//Pin def of Switch:
#define digitalButton_1 21
#define digitalButton_2 22
// #define digitalButton_3 5
//Experimental
#define digitalButton_3 32
#define digitalButton_4 33

#define analogButton A0

//Analog button value storing variable:
int AB_value;

//Locations on display:
String Location;
String Location_1 = "GEC";
String Location_2 = "BAIZID";
String Location_3 = "MURADPUR";
String Location_4 = "PROBARTAK";

//Block State Color:
boolean blockStateColor;
//boolean blockStateColor2;

//Ticker:
Ticker Location1, Location2, Location3, Location4, statusRectToggler, location1Sec;

boolean locationBlock_1 = true;
boolean locationBlock_2 = true;
boolean locationBlock_3 = true;
boolean locationBlock_4 = true;

// Button State var:
boolean button1State = true;
boolean button2State = true;
boolean button3State = true;
boolean button4State = true;

//Sync:
boolean resetCondition = true;

//Timer:
long interval = 3000;
volatile unsigned long DB_priv_time_1;
volatile unsigned long DB_priv_time_2;
volatile unsigned long DB_priv_time_3;
volatile unsigned long AB_priv_time;

//Interrupt Flag:
volatile boolean DB_ISR_F_1 = false;
volatile boolean DB_ISR_F_2 = false;
volatile boolean DB_ISR_F_3 = false;

//Transmission Time Var:
unsigned long DB_1_Process_Start_Time;
unsigned long DB_1_Process_End_Time;
unsigned long DB_2_Process_Start_Time;
unsigned long DB_2_Process_End_Time;
unsigned long DB_3_Process_Start_Time;
unsigned long DB_3_Process_End_Time;

//Signal Time Var:
unsigned long DB_1_Signal_Runtime, DB_2_Signal_Runtime, DB_3_Signal_Runtime, DB_4_Signal_Runtime;
unsigned long currentMil;

//Experimental variables, Color Flags
int colorRG1 = 1, colorRG2 = 1, colorRG3 = 1, colorRG4 = 1;
const int R = 1;
const int G = 2;
const int X = 0;

long int autoTime1 = 0;
long int autoTime2 = 0;

//Serial Print and Debug:
#define DEBUG

//TESTCASE:
//#define TEST
//#define TESTDEBUG
float t_time;
// bool testDebug = true;
int yaxis = 20;
int yaxis_2 = 20;
int print_count = 1;
int count = 0;
int signalTimeCount = 0;
boolean automation_toggle = true;
//MAIN SETUP FUNCTION
void setup()
{
  // #ifdef DEBUG
  Serial.begin(9600);
  // #endif
  //Display Setup:
  displaySetup();
  // Lora Initialization
  loraSetup();
  //To make button 2 work
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  //PinMode:
  pinMode(digitalButton_1, INPUT_PULLUP);
  pinMode(digitalButton_2, INPUT_PULLUP);
  pinMode(digitalButton_3, INPUT_PULLUP);
  pinMode(digitalButton_4, INPUT_PULLUP);

  //Initialization of timers
  DB_1_Signal_Runtime = millis();
  DB_2_Signal_Runtime = millis();
  DB_3_Signal_Runtime = millis();
  DB_4_Signal_Runtime = millis();

  colorRG1 = X;
  colorRG2 = X;
  colorRG3 = X;
  colorRG4 = X;

  location1Sec.attach(5, showTime);

  //Interrupt:
  // attachInterrupt(digitalPinToInterrupt(digitalButton_1), ISR_DB_1, FALLING);
  // attachInterrupt(digitalPinToInterrupt(digitalButton_2), ISR_DB_2, FALLING);
  // attachInterrupt(digitalPinToInterrupt(digitalButton_3), ISR_DB_1, FALLING);

  //Interrupt Experimental
  attachInterrupt(digitalPinToInterrupt(digitalButton_1), ISR_DB_1_G_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_2), ISR_DB_1_R_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_3), ISR_DB_2_G_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_4), ISR_DB_2_R_32, FALLING);
}

//MAIN LOOP
void loop()
{
  //After the device is booted it automatically re-boots other device:
  sync();

  //To use pin 2 as switch:
  //digitalWrite(digitalButton_1, HIGH);
  //digitalWrite(0, LOW);

  //Check Config:
  //check_config();

  //This functions is for InterruptAction:
  autoTime1 = millis();
  if((autoTime1 - autoTime2) > 10000){
    ISR_DB_1_G_32();
    autoTime2 = autoTime1;
  }
  
  
  InterruptAction();

  //This function checks for data to receive
  //recieveData();

  //Analog functions:
  //AnalogAction();

  //This function checks for data to receive
  recieveData();
  
}

//RESETs connected Nodes
void sync()
{
  if (resetCondition == true)
  {

    // ----------------------------------
    // tft.fillRect(rect1x, 117, recwidth, 20, RED);
    // tft.setCursor(20, 122);
    // tft.setTextColor(BLACK);
    // tft.setTextSize(1);
    // tft.print("Sync in progress");
    // ----------------------------------

    String("S").toCharArray(testData, 50);
    address = 0;
    sendData(address, testData);

    if (T_packet_state == 0)
    {
      resetCondition = false;
      // ----------------------------------
      // tft.fillRect(rect1x, 117, recwidth, 20, GREEN);
      // tft.setCursor(22, 122);
      // tft.setTextColor(BLACK);
      // tft.setTextSize(1);
      // tft.print("Sync completed!");
      statusRectToggler.attach(2, statusSecTiggerFunction);
      // ----------------------------------
    }
  }
}

void statusSecTiggerFunction()
{
  //tft.fillRect(rect1x, 117, recwidth, 40, YELLOW);
  statusRectToggler.detach();
}

//Experimental esp32 ISRs
void ISR_DB_1_G_32()
{

  if ((long(millis()) - DB_priv_time_1) >= interval)
  {
    DB_ISR_F_1 = true;
    DB_priv_time_1 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button1State = true;
    // DB_priv_time_1 = millis();
  }
}

void ISR_DB_1_R_32()
{
  if ((long(millis()) - DB_priv_time_1) >= interval)
  {
    DB_ISR_F_1 = true;
    DB_priv_time_1 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button1State = false;
    // DB_priv_time_1 = millis();
  }
}

void ISR_DB_2_G_32()
{
  if ((long(millis()) - DB_priv_time_2) >= interval)
  {
    DB_ISR_F_2 = true;
    DB_priv_time_2 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button2State = true;
    // DB_priv_time_2 = millis();
  }
}

void ISR_DB_2_R_32()
{
  if ((long(millis()) - DB_priv_time_2) >= interval)
  {
    DB_ISR_F_2 = true;
    DB_priv_time_2 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button2State = false;
    // DB_priv_time_2 = millis();
  }
}

//ISRs for DIGITAL Button 1,2,3:
void ISR_DB_1()
{ //ISR handler 1
  if ((long(millis()) - DB_priv_time_1) >= interval)
  {

#ifdef TEST
    if (automation_toggle)
    {
      Serial.println("\nStarting Automated Transmission.");
      ///////////////
      tft.drawRect(3, 4, recwidth + 2, 107, BLACK);
      tft.fillRect(3, 4, recwidth + 2, 107, BLACK);
      tft.setTextSize(0);
      tft.setTextColor(WHITE);
      tft.setCursor(37, 7);
      tft.println("TEST MODE");

      // tft.setCursor(6,20);
      // tft.setTextColor(RED);
      // tft.setTextSize(1);
      //////////////
      DB_ISR_F_1 = true;
      DB_priv_time_1 = millis();
      automation_toggle = false;
    }
    else
    {
      DB_ISR_F_1 = false;
      Serial.println("Switching off Automated Transmission.");
      automation_toggle = true;
      digitalWrite(digitalButton_1, HIGH);
    }
#endif

#ifndef TEST
    DB_ISR_F_1 = true;
    DB_priv_time_1 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
  }
}
void ISR_DB_2()
{ //ISR handler 2
  if ((long(millis()) - DB_priv_time_2) >= interval)
  {
    DB_ISR_F_2 = true;
    DB_priv_time_2 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
  }
}
void ISR_DB_3()
{ //ISR handler 3
  if ((long(millis()) - DB_priv_time_3) >= interval)
  {
    DB_ISR_F_3 = true;
    DB_priv_time_3 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
  }
}

//States what happens when InterruptAction Function is called:
void InterruptAction()
{
  //DB 1:
  if (DB_ISR_F_1)
  {
    DB_1_Process_Start_Time = millis();
    Location = Location_1;

    // location1Sec.attach(1, showTime);
    colorRG1 = X;

    if (button1State)
    {

      //DB_1_Signal_Runtime = millis();
      //location1Sec.attach(1, showTime);

      signalTimeCount = 0;

#ifdef DEBUG
      Serial.println("\nButton 1 was pressed once!");
#endif

#ifdef TEST
      Serial.print("##");
      Serial.print(count += 1);
      Serial.print("\t");
#endif

      String("GL1").toCharArray(testData, 50);
      address = 3;
      sendData(address, testData);

      // -----------------------------------------------
      // tft.fillRect(rect1x, 117, recwidth, 20, YELLOW);
      // tft.setCursor(14, 122);
      // tft.setTextColor(BLACK);
      // tft.setTextSize(1);
      // tft.print("Transmitting Data");
      // -----------------------------------------------

      if (T_packet_state == 0)
      {
        blockStateColor = true;
        //Location1.attach(1, Blink_Location_Rect_1);

        // -----------------------------------------------
        // tft.fillRect(rect1x, 117, recwidth, 20, GREEN);
        // tft.setCursor(6, 122);
        // tft.setTextColor(BLACK);
        // tft.setTextSize(1);
        // tft.print("Transmission:Success");
        // -----------------------------------------------

        button1State = false;
      }
      else
      {
        // -----------------------------------------------
        // tft.fillRect(rect1x, 117, recwidth, 20, RED);
        // tft.setCursor(6, 122);
        // tft.setTextColor(WHITE);
        // tft.setTextSize(1);
        // tft.print("Transmission:Failed");
        // -----------------------------------------------

        button1State = true;
      }
#ifndef TEST
      DB_ISR_F_1 = false;
#endif
    }
    else if (!button1State)
    {

      //DB_1_Signal_Runtime = millis();
      //signalTimeCount = 0;
      //      location1Sec.attach(1, showTime);

#ifdef DEBUG
      Serial.println("\nButton 1 was pressed twice!");
#endif

#ifdef TEST
      Serial.print("##");
      Serial.print(count += 1);
      Serial.print("\t");
#endif

      String("RL1").toCharArray(testData, 50);
      address = 3;
      sendData(address, testData);

      // -----------------------------------------------
      // tft.fillRect(rect1x, 117, recwidth, 20, YELLOW);
      // tft.setCursor(14, 122);
      // tft.setTextColor(BLACK);
      // tft.setTextSize(1);
      // tft.print("Transmitting Data");
      // -----------------------------------------------

      if (T_packet_state == 0)
      {
        blockStateColor = false;
        //Location1.attach(1, Blink_Location_Rect_1);

        // -----------------------------------------------
        // tft.fillRect(rect1x, 117, recwidth, 20, GREEN);
        // tft.setCursor(6, 122);
        // tft.setTextColor(BLACK);
        // tft.setTextSize(1);
        // tft.print("Transmission:Success");
        // -----------------------------------------------

        button1State = true;
      }
      else
      {

        // -----------------------------------------------
        // tft.fillRect(rect1x, 117, recwidth, 20, RED);
        // tft.setCursor(6, 122);
        // tft.setTextColor(WHITE);
        // tft.setTextSize(1);
        // tft.print("Transmission:Failed");
        // -----------------------------------------------

        button1State = false;
      }

#ifndef TEST
      DB_ISR_F_1 = false;
#endif
    }
  }
  //DB 2:
  else if (DB_ISR_F_2)
  {
    DB_2_Process_Start_Time = millis();
    Location = Location_2;

    colorRG2 = X;

    if (button2State)
    {
#ifdef DEBUG
      Serial.println("\nButton 2 was pressed once!");
#endif

      colorRG2 = G;
      DB_2_Signal_Runtime = millis();

      String("GL2").toCharArray(testData, 50);
      address = 4;
      sendData(address, testData);

      // -----------------------------------------------
      // tft.fillRect(rect1x, 117, recwidth, 20, YELLOW);
      // tft.setCursor(14, 122);
      // tft.setTextColor(BLACK);
      // tft.setTextSize(1);
      // tft.print("Transmitting Data");
      // -----------------------------------------------

      if (T_packet_state == 0)
      {
        blockStateColor = true;
        //Location2.attach(0.9, Blink_Location_Rect_2);

        // -----------------------------------------------
        // tft.fillRect(rect1x, 117, recwidth, 20, GREEN);
        // tft.setCursor(6, 122);
        // tft.setTextColor(BLACK);
        // tft.setTextSize(1);
        // tft.print("Transmission:Success");
        // -----------------------------------------------

        button2State = false;
      }
      else
      {
        // -----------------------------------------------
        // tft.fillRect(rect1x, 117, recwidth, 20, RED);
        // tft.setCursor(6, 122);
        // tft.setTextColor(WHITE);
        // tft.setTextSize(1);
        // tft.print("Transmission:Failed");
        // -----------------------------------------------

        button2State = true;
      }

      DB_ISR_F_2 = false;
    }
    else if (!button2State)
    {
#ifdef DEBUG
      Serial.println("\nButton 2 was pressed twice!");
#endif

      colorRG2 = R;
      DB_2_Signal_Runtime = millis();

      String("RL2").toCharArray(testData, 50);
      address = 4;
      sendData(address, testData);

      // -----------------------------------------------
      // tft.fillRect(rect1x, 117, recwidth, 20, YELLOW);
      // tft.setCursor(14, 122);
      // tft.setTextColor(BLACK);
      // tft.setTextSize(1);
      // tft.print("Transmitting Data");
      // -----------------------------------------------

      if (T_packet_state == 0)
      {
        blockStateColor = false;
        //Location2.attach(0.9, Blink_Location_Rect_2);

        // -----------------------------------------------
        // tft.fillRect(rect1x, 117, recwidth, 20, GREEN);
        // tft.setCursor(6, 122);
        // tft.setTextColor(BLACK);
        // tft.setTextSize(1);
        // tft.print("Transmission:Success");
        // -----------------------------------------------

        button2State = true;
      }
      else
      {
        // -----------------------------------------------
        // tft.fillRect(rect1x, 117, recwidth, 20, RED);
        // tft.setCursor(6, 122);
        // tft.setTextColor(WHITE);
        // tft.setTextSize(1);
        // tft.print("Transmission:Failed");
        // -----------------------------------------------

        button2State = false;
      }
      DB_ISR_F_2 = false;
    }
  }
  //DB 3:
  else if (DB_ISR_F_3)
  {
    DB_3_Process_Start_Time = millis();
    Location = Location_3;

    colorRG3 = X;

    if (button3State)
    {
#ifdef DEBUG
      Serial.println("\nButton 3 was pressed once!");
#endif

      String("GL3").toCharArray(testData, 50);
      address = 6;
      sendData(address, testData);

      // -----------------------------------------------
      tft.fillRect(rect1x, 117, recwidth, 20, YELLOW);
      tft.setCursor(14, 122);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("Transmitting Data");
      // -----------------------------------------------

      if (T_packet_state == 0)
      {
        blockStateColor = true;
        Location3.attach(0.7, Blink_Location_Rect_3);

        // -----------------------------------------------
        tft.fillRect(rect1x, 117, recwidth, 20, GREEN);
        tft.setCursor(6, 122);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("Transmission:Success");
        // -----------------------------------------------

        button3State = false;
      }
      else
      {
        // -----------------------------------------------
        tft.fillRect(rect1x, 117, recwidth, 20, RED);
        tft.setCursor(6, 122);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("Transmission:Failed");
        // -----------------------------------------------

        button3State = true;
      }
      DB_ISR_F_3 = false;
    }
    else if (!button3State)
    {
#ifdef DEBUG
      Serial.println("\nButton 3 was pressed twice!");
#endif

      String("RL3").toCharArray(testData, 50);
      address = 6;
      sendData(address, testData);

      // -----------------------------------------------
      tft.fillRect(rect1x, 117, recwidth, 20, YELLOW);
      tft.setCursor(14, 122);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("Transmitting Data");
      // -----------------------------------------------

      if (T_packet_state == 0)
      {
        blockStateColor = false;
        Location3.attach(0.7, Blink_Location_Rect_3);

        // -----------------------------------------------
        tft.fillRect(rect1x, 117, recwidth, 20, GREEN);
        tft.setCursor(6, 122);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("Transmission:Success");
        // -----------------------------------------------

        button3State = true;
      }
      else
      {
        // -----------------------------------------------
        tft.fillRect(rect1x, 117, recwidth, 20, RED);
        tft.setCursor(6, 122);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("Transmission:Failed");
        // -----------------------------------------------

        button3State = false;
      }
      DB_ISR_F_3 = false;
    }
  }
}
//Analog button action:
void AnalogAction()
{
  AB_value = analogRead(analogButton);
  if (AB_value < 100)
  {
    if ((long(millis()) - AB_priv_time) >= interval)
    {
      Location = Location_4;

      colorRG4 = X;

      if (button4State)
      {
#ifdef DEBUG
        Serial.println("Button 4 was pressed once!");
#endif

        String("GL4").toCharArray(testData, 50);
        address = 7;
        sendData(address, testData);

        if (T_packet_state == 0)
        {
          blockStateColor = true;
          Location4.attach(0.8, Blink_Location_Rect_4);
          button4State = false;
        }
      }
      else if (!button4State)
      {
#ifdef DEBUG
        Serial.println("Button 4 was pressed twice!");
#endif

        String("RL4").toCharArray(testData, 50);
        address = 7;
        sendData(address, testData);

        if (T_packet_state == 0)
        {
          blockStateColor = false;
          Location4.attach(0.8, Blink_Location_Rect_4);
          button4State = true;
        }
      }
      AB_priv_time = millis();
    }
  }
}

/*
  Function: Configures the module to transmit information and receive an ACK.
  Returns: Integer that determines if there has been any error [T_packet_state]
   state = 9  --> The ACK lost (no data available)
   state = 8  --> The ACK lost
   state = 7  --> The ACK destination incorrectly received
   state = 6  --> The ACK source incorrectly received
   state = 5  --> The ACK number incorrectly received
   state = 4  --> The ACK length incorrectly received
   state = 3  --> N-ACK received
   state = 2  --> The command has not been executed
   state = 1  --> There has been an error while executing the command
   state = 0  --> The command has been executed with no errors
*/

//Global send data function
void sendData(uint8_t NodeAddress, char message[])
{
#ifdef DEBUG
  Serial.print("Node Address : ");
  Serial.println(address);
#endif

  delay(1000);

  T_packet_state = sx1278.sendPacketTimeoutACKRetries(NodeAddress, message);
  if (T_packet_state == 1)
  {
#ifdef DEBUG
    Serial.println(F("State = 1 --> Modified By Shaem"));
#endif
    SPI.end();
    delay(10000);

    loraSetup();
    displaySetup();
  }


  if (T_packet_state == 0)
  {
#ifdef DEBUG
    Serial.println(F("State = 0 --> Command Executed w no errors!"));
    Serial.println(F("Packet sent..."));
#endif
  }
  else if (T_packet_state == 5 || T_packet_state == 6 || T_packet_state == 7)
  {
    Serial.println("Conflict!");
    sendData(address, testData);
  }
  
  else
  {
#ifdef DEBUG
    Serial.print(F("Error Code: "));
    Serial.println(T_packet_state);
    Serial.println(F("Packet not sent...."));
#endif
  }
}
//Global receive data function
void recieveData()
{
  R_packet_state = sx1278.receivePacketTimeoutACK();
  if (R_packet_state == 0)
  {
#ifdef DEBUG
    Serial.println(F("Package received!"));
#endif
    for (unsigned int i = 0; i < sx1278.packet_received.length; i++)
    {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
#ifdef DEBUG
    Serial.print(F("Message:  "));
    Serial.println(my_packet);
#endif
    receivedMsg = String(my_packet);
    Setting_Block_State_Color();
  }
}

//BLINKING LOCATION BLOCKS
//Blinking the FIRST Location
void Blink_Location_Rect_1()
{
  tft.setCursor(40, 30);
  tft.setTextSize(1);

  signalTimeCount++;

  //Green Block Blink:
  if (blockStateColor)
  {
    if (locationBlock_1)
    {
#ifndef TESTDEBUG
      tft.fillRect(rect1x, rect1y, recwidth, recheight, GREEN);
      tft.setTextColor(BLACK);
      tft.print("GEC");
#endif
      locationBlock_1 = false;
    }
    else if (!locationBlock_1)
    {
#ifndef TESTDEBUG
      tft.fillRect(rect1x, rect1y, recwidth, recheight, BLACK);
      tft.setTextColor(WHITE);
      tft.print("GEC");
#endif
      locationBlock_1 = true;
    }
  }
  //Red Block Blink:
  else if (!blockStateColor)
  {
    if (locationBlock_1)
    {
#ifndef TESTDEBUG
      tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
      tft.setTextColor(BLACK);
      tft.print("GEC");
#endif
      locationBlock_1 = false;
    }
    else if (!locationBlock_1)
    {
#ifndef TESTDEBUG
      tft.fillRect(rect1x, rect1y, recwidth, recheight, BLACK);
      tft.setTextColor(WHITE);
      tft.print("GEC");
#endif
      locationBlock_1 = true;
    }
  }
}
//Blinking the SECOND Location
void Blink_Location_Rect_2()
{
  tft.setCursor(40, 55);
  tft.setTextSize(1);

  //Green Block Blink
  if (blockStateColor)
  {
    if (locationBlock_2)
    {
      tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
      tft.setTextColor(BLACK);
      tft.print("BAIZID");
      locationBlock_2 = false;
    }
    else if (!locationBlock_2)
    {
      tft.fillRect(rect1x, rect2y, recwidth, recheight, BLACK);
      tft.setTextColor(WHITE);
      tft.print("BAIZID");
      locationBlock_2 = true;
    }
  }
  //Red Block Blink
  else if (!blockStateColor)
  {
    if (locationBlock_2)
    {
      tft.fillRect(rect1x, rect2y, recwidth, recheight, RED);
      tft.setTextColor(BLACK);
      tft.print("BAIZID");
      locationBlock_2 = false;
    }
    else if (!locationBlock_2)
    {
      tft.fillRect(rect1x, rect2y, recwidth, recheight, BLACK);
      tft.setTextColor(WHITE);
      tft.print("BAIZID");
      locationBlock_2 = true;
    }
  }
}
//Blinking the THIRD Location
void Blink_Location_Rect_3()
{
  tft.setCursor(40, 75);
  tft.setTextSize(1);

  //Green Block Blink
  if (blockStateColor)
  {
    if (locationBlock_3)
    {
      tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
      tft.setTextColor(BLACK);
      tft.print("MURADPUR");
      locationBlock_3 = false;
    }
    else if (!locationBlock_3)
    {
      tft.fillRect(rect1x, rect3y, recwidth, recheight, BLACK);
      tft.setTextColor(WHITE);
      tft.print("MURADPUR");
      locationBlock_3 = true;
    }
  }
  //Red Block Blink
  else if (!blockStateColor)
  {
    if (locationBlock_3)
    {
      tft.fillRect(rect1x, rect3y, recwidth, recheight, RED);
      tft.setTextColor(BLACK);
      tft.print("MURADPUR");
      locationBlock_3 = false;
    }
    else if (!locationBlock_3)
    {
      tft.fillRect(rect1x, rect3y, recwidth, recheight, BLACK);
      tft.setTextColor(WHITE);
      tft.print("MURADPUR");
      locationBlock_3 = true;
    }
  }
}
//Blinking the FOURTH Location
void Blink_Location_Rect_4()
{
  //Green Block Blink
  if (blockStateColor)
  {

    if (locationBlock_4)
    {

      tft.fillRect(rect1x, rect4y, recwidth, recheight, GREEN);
      tft.setCursor(40, 97);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("PROBARTAK");

      locationBlock_4 = false;
    }
    else if (!locationBlock_4)
    {
      tft.fillRect(rect1x, rect4y, recwidth, recheight, BLACK);
      tft.setCursor(40, 97);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("PROBARTAK");

      locationBlock_4 = true;
    }
  }
  //Red Block Blink
  else if (!blockStateColor)
  {

    if (locationBlock_4)
    {

      tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);
      tft.setCursor(40, 97);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("PROBARTAK");

      locationBlock_4 = false;
    }
    else if (!locationBlock_4)
    {

      tft.fillRect(rect1x, rect4y, recwidth, recheight, BLACK);
      tft.setCursor(40, 97);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("PROBARTAK");

      locationBlock_4 = true;
    }
  }
}

//Experimental show signal time in seconds
void showTime()
{
  currentMil = millis();
  tft.setTextSize(1);
  int time1 = (currentMil - DB_1_Signal_Runtime) / 1000;
  int time2 = (currentMil - DB_2_Signal_Runtime) / 1000;
  int time3 = (currentMil - DB_3_Signal_Runtime) / 1000;
  int time4 = (currentMil - DB_4_Signal_Runtime) / 1000;

  String printt1 = String(time1) + "S";
  String printt2 = String(time2) + "S";
  String printt3 = String(time3) + "S";
  String printt4 = String(time4) + "S";

  switch (colorRG1)
  {
    case R:
      //tft.fillRect(rect1x + recwidth - 30, rect1y, 30, recheight, RED);
      //tft.setCursor(rect1x + recwidth - 25, 30);
      //tft.setTextColor(WHITE);
      //tft.print(printt1);

      u8g1.clearBuffer();                 // clear the internal memory
      u8g1.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt1 = "Red " + printt1;
      u8g1.drawStr(20, 40, printt1.c_str()); // write something to the internal memory
      u8g1.drawStr(0, 10, "LOCATION GEC");   // write something to the internal memory
      u8g1.sendBuffer();
      //state1 = !state1;

      break;
    case G:
      u8g1.clearBuffer();                 // clear the internal memory
      u8g1.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt1 = "Green " + printt1;
      u8g1.drawStr(20, 40, printt1.c_str()); // write something to the internal memory
      u8g1.drawStr(0, 10, "LOCATION GEC");   // write something to the internal memory
      u8g1.sendBuffer();
      break;
    case X:
      break;
  }

  switch (colorRG2)
  {
    case R:
      // tft.fillRect(rect1x + recwidth - 30, rect2y, 30, recheight, RED);
      // tft.setCursor(rect1x + recwidth - 25, 55);
      // tft.setTextColor(WHITE);
      // tft.print(printt2);
      u8g1.clearBuffer();                 // clear the internal memory
      u8g1.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt2 = "Red " + printt2;
      u8g1.drawStr(20, 40, printt2.c_str()); // write something to the internal memory
      u8g1.sendBuffer();
      break;
    case G:
      // tft.fillRect(rect1x + recwidth - 30, rect2y, 30, recheight, GREEN);
      // tft.setCursor(rect1x + recwidth - 25, 55);
      // tft.setTextColor(BLACK);
      // tft.print(printt2);
      u8g1.clearBuffer();                 // clear the internal memory
      u8g1.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt2 = "Green " + printt2;
      u8g1.drawStr(20, 40, printt2.c_str()); // write something to the internal memory
      u8g1.sendBuffer();
      break;
    case X:
      break;
  }

  switch (colorRG3)
  {
    case R:
      tft.fillRect(rect1x + recwidth - 30, rect3y, 30, recheight, RED);
      tft.setCursor(rect1x + recwidth - 25, 75);
      tft.print(printt3);
      break;
    case G:
      tft.fillRect(rect1x + recwidth - 30, rect3y, 30, recheight, GREEN);
      tft.setCursor(rect1x + recwidth - 25, 75);
      tft.print(printt3);
      break;
    case X:
      break;
  }

  switch (colorRG4)
  {
    case R:
      tft.fillRect(rect1x + recwidth - 30, rect4y, 30, recheight, RED);
      tft.setCursor(rect1x + recwidth - 25, 97);
      tft.print(printt4);
      break;
    case G:
      tft.fillRect(rect1x + recwidth - 30, rect4y, 30, recheight, GREEN);
      tft.setCursor(rect1x + recwidth - 25, 97);
      tft.print(printt4);
      break;
    case X:
      break;
  }
}

//FINAL LOCATION BLOCK STATE
void Setting_Block_State_Color()
{
  //Sets the block state for FIRST Location
  if (receivedMsg.equals("KL1"))
  {

    if (blockStateColor)
    {
      Location1.detach();
      colorRG1 = G;
      DB_1_Signal_Runtime = millis();
      location1Sec.attach(5, showTime);

#ifndef TESTDEBUG
      tft.fillRect(rect1x, rect1y, recwidth, recheight, GREEN);
      tft.setCursor(40, 30);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("GEC");
#endif

      // -----------------------------------------------
      // tft.fillRect(rect1x, 137, recwidth, 20, GREEN);
      // tft.setCursor(20, 142);
      // tft.setTextColor(BLACK);
      // tft.setTextSize(1);
      // tft.print("Packet Arrived!");

      statusRectToggler.attach(2, statusSecTiggerFunction);
      // -----------------------------------------------
    }
    else if (!blockStateColor)
    {

      Location1.detach();
      colorRG1 = R;
      DB_1_Signal_Runtime = millis();
      location1Sec.attach(5, showTime);

#ifndef TESTDEBUG
      tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
      tft.setCursor(40, 30);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("GEC");
#endif

      // -----------------------------------------------
      // tft.fillRect(rect1x, 137, recwidth, 20, RED);
      // tft.setCursor(20, 142);
      // tft.setTextColor(WHITE);
      // tft.setTextSize(1);
      // tft.print("Packet Arrived!");

      statusRectToggler.attach(2, statusSecTiggerFunction);
      // -----------------------------------------------
    }
    DB_1_Process_End_Time = millis();
    t_time = ((DB_1_Process_End_Time - DB_1_Process_Start_Time) / 1000.0);
    Serial.print("Required time: ");
    Serial.println(t_time, 3);
    testlog();
    digitalWrite(0, LOW);
  }
  //Sets the block state for SECOND location
  if (receivedMsg.equals("KL2"))
  {
    if (blockStateColor)
    {
      Location2.detach();

      colorRG2 = G;
      DB_2_Signal_Runtime = millis();
      location1Sec.attach(5, showTime);

      // tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
      // tft.setCursor(40, 55);
      // tft.setTextColor(BLACK);
      // tft.setTextSize(1);
      // tft.print("BAIZID");
      // // -----------------------------------------------
      // tft.fillRect(rect1x, 137, recwidth, 20, GREEN);
      // tft.setCursor(20, 142);
      // tft.setTextColor(BLACK);
      // tft.setTextSize(1);
      // tft.print("Packet Arrived!");

      statusRectToggler.attach(2, statusSecTiggerFunction);
      // -----------------------------------------------
    }
    else if (!blockStateColor)
    {
      Location2.detach();

      colorRG2 = R;
      DB_2_Signal_Runtime = millis();
      location1Sec.attach(5, showTime);

      // tft.fillRect(rect1x, rect2y, recwidth, recheight, RED);
      // tft.setCursor(40, 55);
      // tft.setTextColor(WHITE);
      // tft.setTextSize(1);
      // tft.print("BAIZID");
      // // -----------------------------------------------
      // tft.fillRect(rect1x, 137, recwidth, 20, RED);
      // tft.setCursor(20, 142);
      // tft.setTextColor(WHITE);
      // tft.setTextSize(1);
      // tft.print("Packet Arrived!");

      statusRectToggler.attach(2, statusSecTiggerFunction);
      // -----------------------------------------------
    }
    DB_2_Process_End_Time = millis();
    Serial.print("2 - Required time: ");
    Serial.println(((DB_2_Process_End_Time - DB_2_Process_Start_Time) / 1000.0), 3);
  }
  //Sets the block state for THIRD location
  if (receivedMsg.equals("KL3"))
  {
    if (blockStateColor)
    {
      Location3.detach();

      colorRG3 = G;
      DB_3_Signal_Runtime = millis();
      location1Sec.attach(5, showTime);

      tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
      tft.setCursor(40, 75);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("MURADPUR");
      // -----------------------------------------------
      tft.fillRect(rect1x, 137, recwidth, 20, GREEN);
      tft.setCursor(20, 142);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.print("Packet Arrived!");

      statusRectToggler.attach(2, statusSecTiggerFunction);
      // -----------------------------------------------
    }
    else if (!blockStateColor)
    {
      Location3.detach();

      colorRG3 = R;
      DB_3_Signal_Runtime = millis();
      location1Sec.attach(5, showTime);

      tft.fillRect(rect1x, rect3y, recwidth, recheight, RED);
      tft.setCursor(40, 75);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("MURADPUR");
      // -----------------------------------------------
      tft.fillRect(rect1x, 137, recwidth, 20, RED);
      tft.setCursor(20, 142);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("Packet Arrived!");

      statusRectToggler.attach(2, statusSecTiggerFunction);
      // -----------------------------------------------
    }
    DB_3_Process_End_Time = millis();
    Serial.print("3 - Required time: ");
    Serial.println(((DB_3_Process_End_Time - DB_3_Process_Start_Time) / 1000.0), 3);
  }
  //Sets the block state for FOURTH location
  if (receivedMsg.equals("KL4"))
  {
    if (blockStateColor)
    {
      Location4.detach();

      colorRG4 = G;
      DB_4_Signal_Runtime = millis();
      location1Sec.attach(5, showTime);

      tft.fillRect(rect1x, rect4y, recwidth, recheight, GREEN);
      tft.setCursor(40, 97);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("PROBARTAK");
    }
    else if (!blockStateColor)
    {
      Location4.detach();

      colorRG4 = R;
      DB_4_Signal_Runtime = millis();
      location1Sec.attach(5, showTime);

      tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);
      tft.setCursor(40, 97);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.print("PROBARTAK");
    }
  }
}

//Sets Important Lora Modes and returns 'true' if it was successful or 'false' if it wasn't
void loraSetup()
{
#ifdef DEBUG
  Serial.println("");
#endif
  // Power ON the module:
  if (sx1278.ON() == 0)
  {
#ifdef DEBUG
    Serial.println(F("Setting power ON: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("Setting power ON: ERROR "));
#endif
  }

  // Set transmission mode and print the result:
  if (sx1278.setMode(LORA_MODE) == 0)
  {
#ifdef DEBUG
    Serial.println(F("Setting Mode: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("Setting Mode: ERROR "));
#endif
  }

  // Set header:
  if (sx1278.setHeaderON() == 0)
  {
#ifdef DEBUG
    Serial.println(F("Setting Header ON: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("Setting Header ON: ERROR "));
#endif
  }

  // Select frequency channel:
  if (sx1278.setChannel(LORA_CHANNEL) == 0)
  {
#ifdef DEBUG
    Serial.println(F("Setting Channel: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("Setting Channel: ERROR "));
#endif
  }

  // Set CRC:
  if (sx1278.setCRC_ON() == 0)
  {
#ifdef DEBUG
    Serial.println(F("Setting CRC ON: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("Setting CRC ON: ERROR "));
#endif
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0)
  {
#ifdef DEBUG
    Serial.println(F("Setting Power: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("Setting Power: ERROR "));
#endif
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0)
  {
#ifdef DEBUG
    Serial.println(F("Setting node address: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG
    Serial.println(F("Setting node address: ERROR "));
#endif
  }

  // Print a success
#ifdef DEBUG
  Serial.println(F("SX1278 configured finished"));
  Serial.println();
#endif
}
//Creates the UI layout
void displaySetup()
{
  u8g2.begin();

  u8g1.begin();
  loraSetup();

  u8g1.clearBuffer();                  // clear the internal memory
  u8g1.setFont(u8g2_font_ncenB08_tr);  // choose a suitable font
  u8g1.drawStr(0, 10, "LOCATION GEC"); // write something to the internal memory
  u8g1.sendBuffer();                   // transfer internal memory to the display
  delay(1000);

  u8g2.clearBuffer();                     // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);     // choose a suitable font
  u8g2.drawStr(0, 10, "LOCATION BAIZID"); // write something to the internal memory
  u8g2.sendBuffer();                      // transfer internal memory to the display
  delay(1000);
}
//Checks SF,BW,CR
void check_config()
{

  if (sx1278.getBW() == 0)
    Serial.println("BW-Success");
  else
    Serial.println("BW-Error");

  if (sx1278.getSF() == 0)
    Serial.println("SF-Success");
  else
    Serial.println("SF-Error");

  if (sx1278.getCR() == 0)
    Serial.println("CR-Success");
  else
    Serial.println("CR-Error");
}

void testlog()
{

#ifdef TESTDEBUG

  if (print_count < 11)
  {
    tft.setCursor(6, yaxis);
    yaxis += 9;
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println(t_time, 3);
    print_count += 1;

    if (print_count == 9)
    {
      tft.drawRect(79, 20, 40, 107 - 17, BLACK);
      tft.fillRect(79, 20, 40, 107 - 17, BLACK);
    }
  }
  else
  {
    tft.setCursor(80, yaxis_2);
    yaxis_2 += 9;
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    tft.println(t_time, 3);
    print_count += 1;
  }
  if (print_count == 21)
  {
    print_count = 1;
    yaxis = 20;
    yaxis_2 = 20;
    tft.drawRect(4, 20, 40, 107 - 17, BLACK);
    tft.fillRect(4, 20, 40, 107 - 17, BLACK);
    //
    // tft.drawRect(3, 20, recwidth + 2, 107-15, BLACK);
    // tft.fillRect(3,20,recwidth+2,107-15,BLACK);
  }

#endif
}
