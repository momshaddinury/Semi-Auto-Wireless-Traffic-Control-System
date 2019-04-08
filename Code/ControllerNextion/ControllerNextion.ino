// ---------------------------------------------------------------------
/*
  Project Name - Semi Automatic Traffic Control System using Lora SX1278
  Components used:
           MCU - ESP32
           Communication module - Lora SX1278 ( FDK Mode )
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
//Lora SX1278:
#include <SX1278.h>
//Library for Ticker
#include <Ticker.h>
//Library for Display
#include <U8g2lib.h>
#include <WiFi.h>

/*****************************************************************************
   Definitions & Declarations
 *****************************************************************************/

U8G2_PCD8544_84X48_F_4W_HW_SPI u8g1(U8G2_R0, /* cs=*/2, /* dc=*/23, /* reset=*/100);  // Nokia 5110 Display
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/4, /* dc=*/23, /* reset=*/100);  // Nokia 5110 Display
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g3(U8G2_R0, /* cs=*/15, /* dc=*/23, /* reset=*/100); // Nokia 5110 Display
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g4(U8G2_R0, /* cs=*/17, /* dc=*/23, /* reset=*/100); // Nokia 5110 Display

//Lora SX1278:
#define LORA_MODE 4
#define LORA_CHANNEL CH_6_BW_125
#define LORA_ADDRESS 5
uint8_t NodeAddress; //Child Address
int address;

#define NEXTION_RED "63488"
#define NEXTION_BLACK "0"
#define NEXTION_GREEN "2016"
#define NEXTION_BLUE "31"
#define NEXTION_WHITE "65535"

#define NEXTION_LOCATION_1 "t0"
#define NEXTION_LOCATION_2 "t1"
#define NEXTION_LOCATION_3 "t2"
#define NEXTION_LOCATION_4 "t3"

#define NEXTION_STATUS_1 "t4"
#define NEXTION_STATUS_2 "t5"
#define NEXTION_STATUS_3 "t6"
#define NEXTION_STATUS_4 "t7"

#define NEXTION_TRANSMISSION_1 "t12"
#define NEXTION_TRANSMISSION_2 "t13"
#define NEXTION_TRANSMISSION_3 "t14"
#define NEXTION_TRANSMISSION_4 "t15"

#define NEXTION_TIMER_1 "t8"
#define NEXTION_TIMER_2 "t19"
#define NEXTION_TIMER_3 "t10"
#define NEXTION_TIMER_4 "t11"

#define NEXTION_COMMAND_TEXT "txt"
#define NEXTION_COMMAND_BACKGROUND "bco"
#define NEXTION_FOREGROUND_TEXT_COLOR "pco"

#define ADDR_1_INT 3
#define ADDR_1_STR "  GEC CIRCLE"

#define ADDR_2_INT 4
#define ADDR_2_STR "      BAYEZID"

#define ADDR_3_INT 6
#define ADDR_3_STR "      OXYGEN"

#define ADDR_4_INT 7
#define ADDR_4_STR "  MURADPUR"

//Packets var:
char my_packet[50];
char testData[50];
String receivedMsg;
int T_packet_state;
int R_packet_state;

//Pin def of Switch:

#define digitalButton_1 25
#define digitalButton_2 26
#define digitalButton_3 32
#define digitalButton_4 33
#define digitalButton_5 18
#define digitalButton_6 19
#define digitalButton_7 21
#define digitalButton_8 22

// #define digitalButton_3 5
//Experimental

//Signal Time Var:
unsigned long DB_1_Signal_Runtime, DB_2_Signal_Runtime, DB_3_Signal_Runtime, DB_4_Signal_Runtime;
unsigned long currentMil;

//Experimental variables, Color Flags
int colorRG1 = 1, colorRG2 = 1, colorRG3 = 1, colorRG4 = 1;
const int R = 1;
const int G = 2;
const int X = 0;

int attemptDebug = 0;

Ticker location1Sec;

//Sync:
boolean resetCondition = true;

//Timer used to stop debounce @Interrupt Button Press:
long interval = 3000;
volatile unsigned long DB_priv_time_1;
volatile unsigned long DB_priv_time_2;
volatile unsigned long DB_priv_time_3;
volatile unsigned long DB_priv_time_4;
//Interrupt Flag @ISRs Function:
volatile boolean DB_ISR_F_1 = false;
volatile boolean DB_ISR_F_2 = false;
volatile boolean DB_ISR_F_3 = false;
volatile boolean DB_ISR_F_4 = false;

// Button State var @ISRs function:
boolean button1State = true;
boolean button2State = true;
boolean button3State = true;
boolean button4State = true;

boolean is1First = true;
boolean is2First = true;
boolean is3First = true;
boolean is4First = true;

boolean isSyncDone = false;
boolean is1Active = false;
boolean is2Active = false;
boolean is3Active = false;
boolean is4Active = false;

//Transmission Time Calculation Var @InterruptAction():
unsigned long DB_1_Process_Start_Time;
unsigned long DB_1_Process_End_Time;
unsigned long DB_2_Process_Start_Time;
unsigned long DB_2_Process_End_Time;
unsigned long DB_3_Process_Start_Time;
unsigned long DB_3_Process_End_Time;
unsigned long DB_4_Process_Start_Time;
unsigned long DB_4_Process_End_Time;

//Automatic Transmission Flags and Var:
// #define automation
long int autoTime1 = 0;
long int autoTime2 = 0;

//*****
int signalTimeCount = 0;

//Serial Print and Debug:
#define DEBUG
//TESTCASE:
//#define TEST
//#define TESTDEBUG

float t_time;
//Block State Color:
boolean blockStateColor;
int count = 0;

boolean isTransmissionInProgress = false;
boolean isTransmissionInProgress_1 = false;
boolean isTransmissionInProgress_2 = false;
boolean isTransmissionInProgress_3 = false;
boolean isTransmissionInProgress_4 = false;
int countDebug = 1;

//MAIN SETUP FUNCTION
void setup()
{
  // #ifdef DEBUG
  Serial.begin(9600);
  Serial2.begin(57600);

  // #endif
  WiFi.mode(WIFI_OFF);
  btStop();
  delay(2000);

  //Display Setup:
  displaySetup();

  // Lora Initialization
  loraSetup();

  //PinMode:
  // 1 Button:
  pinMode(digitalButton_1, INPUT_PULLUP);
  pinMode(digitalButton_2, INPUT_PULLUP);
  // 2 Button:
  pinMode(digitalButton_3, INPUT_PULLUP);
  pinMode(digitalButton_4, INPUT_PULLUP);
  // 3 Button:
  pinMode(digitalButton_5, INPUT_PULLUP);
  pinMode(digitalButton_6, INPUT_PULLUP);
  // 4 Button:
  pinMode(digitalButton_7, INPUT_PULLUP);
  pinMode(digitalButton_8, INPUT_PULLUP);

  pinMode(27, OUTPUT);
  digitalWrite(27, LOW); // To Light Up The Display.

  //Initialization of timers
  DB_1_Signal_Runtime = millis(); //L 1
  DB_2_Signal_Runtime = millis(); //L 2
  DB_3_Signal_Runtime = millis();
  DB_4_Signal_Runtime = millis();

  colorRG1 = X; //L 1
  colorRG2 = X; //L 2
  colorRG3 = X;
  colorRG4 = X;

  //Interrupt Experimental
  attachInterrupt(digitalPinToInterrupt(digitalButton_1), ISR_DB_1_G_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_2), ISR_DB_1_R_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_3), ISR_DB_2_G_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_4), ISR_DB_2_R_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_5), ISR_DB_3_G_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_6), ISR_DB_3_R_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_7), ISR_DB_4_G_32, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_8), ISR_DB_4_R_32, FALLING);
}

//MAIN LOOP
void loop()
{
  //After the device is booted it automatically re-boots other device:
  sync();

  //  if (countDebug % 2 == 0) ISR_DB_3_G_32();
  //  else ISR_DB_3_R_32();
  //autoTransmission();
  InterruptAction();

  //This function checks for data to receive
  //  if (!isSyncDone)
  //  {
  receiveSync();
  //  }
  //  else recieveData();

  //Show time on display:
  showTime();
  //  delay(500);
}

//RESETs connected Nodes
void sync()
{
  if (resetCondition == true)
  {

    String("S").toCharArray(testData, 50);
    address = ADDR_1_INT;
    sendData2(ADDR_1_INT, testData);
    delay(500);

    receiveSync();

    address = ADDR_2_INT;
    sendData2(ADDR_2_INT, testData);
    delay(500);

    receiveSync();

    address = ADDR_3_INT;
    sendData2(ADDR_3_INT, testData);
    delay(500);

    receiveSync();

    address = ADDR_4_INT;
    sendData2(ADDR_4_INT, testData);
    delay(500);

    //    sendData2(0, testData);

    //    if (T_packet_state == 0)
    //    {
    resetCondition = false;
    delay(1000);
    //    }
  }
}

void nextionWriter(String id, String command, String value, boolean isColor)
{

  String data;
  if (isColor)
    data = id + "." + command + "=" + value;
  else
    data = id + "." + command + "=\"" + value + "\"";
  Serial.println(data);
  Serial2.print(data);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void receiveSync()
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
    if (receivedMsg == "K01")
    {
      is1Active = is1First = true;
      if (colorRG1 == X)
      {
        nextionWriter(NEXTION_STATUS_1, NEXTION_COMMAND_TEXT, "Active", false);
        nextionWriter(NEXTION_STATUS_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
      }
      else if (colorRG1 == R)
      {
        ISR_DB_1_R_32();
      }
      else if (colorRG1 == G)
      {
        ISR_DB_1_G_32();
      }
    }
    else if (receivedMsg == "K02")
    {
      is2Active = is2First = true;
      if (colorRG2 == X)
      {
        nextionWriter(NEXTION_STATUS_2, NEXTION_COMMAND_TEXT, "Active", false);
        nextionWriter(NEXTION_STATUS_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
      }
      else if (colorRG2 == R)
      {
        ISR_DB_2_R_32();
      }
      else if (colorRG2 == G)
      {
        ISR_DB_2_G_32();
      }
    }
    else if (receivedMsg == "K03")
    {
      is3Active = is3First = true;
      if (colorRG3 == X)
      {
        nextionWriter(NEXTION_STATUS_3, NEXTION_COMMAND_TEXT, "Active", false);
        nextionWriter(NEXTION_STATUS_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
      }
      else if (colorRG3 == R)
      {
        ISR_DB_3_R_32();
      }
      else if (colorRG3 == G)
      {
        ISR_DB_3_G_32();
      }
    }
    else if (receivedMsg == "K04")
    {
      is4Active = is4First = true;
      if (colorRG4 == X)
      {
        nextionWriter(NEXTION_STATUS_4, NEXTION_COMMAND_TEXT, "Active", false);
        nextionWriter(NEXTION_STATUS_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
      }
      else if (colorRG4 == R)
      {
        ISR_DB_4_R_32();
      }
      else if (colorRG4 == G)
      {
        ISR_DB_4_G_32();
      }
    }
    else
    {
      Setting_Block_State_Color();
    }
    if (is1Active && is2Active && is3Active && is4Active)
    {
      isSyncDone = true;
    }
  }
}

//Experimental esp32 ISRs
void ISR_DB_1_G_32()
{
  if (!button1State && !is1First)
    return;
  if ((long(millis()) - DB_priv_time_1) >= interval)
  {
    DB_ISR_F_1 = true;
    DB_priv_time_1 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button1State = true;
  }
}

void ISR_DB_1_R_32()
{
  if (button1State && !is1First)
    return;
  if ((long(millis()) - DB_priv_time_1) >= interval)
  {
    DB_ISR_F_1 = true;
    DB_priv_time_1 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button1State = false;
  }
}

void ISR_DB_2_G_32()
{
  if (!button2State && !is2First)
    return;
  if ((long(millis()) - DB_priv_time_2) >= interval)
  {
    DB_ISR_F_2 = true;
    DB_priv_time_2 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button2State = true;
  }
}

void ISR_DB_2_R_32()
{
  if (button2State && !is2First)
    return;
  if ((long(millis()) - DB_priv_time_2) >= interval)
  {
    DB_ISR_F_2 = true;
    DB_priv_time_2 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button2State = false;
  }
}

void ISR_DB_3_G_32()
{
  if (!button3State && !is3First)
    return;
  if ((long(millis()) - DB_priv_time_3) >= interval)
  {
    DB_ISR_F_3 = true;
    DB_priv_time_3 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button3State = true;
  }
}

void ISR_DB_3_R_32()
{
  if (button3State && !is3First)
    return;
  if ((long(millis()) - DB_priv_time_3) >= interval)
  {
    DB_ISR_F_3 = true;
    DB_priv_time_3 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button3State = false;
  }
}

void ISR_DB_4_G_32()
{
  if (!button4State && !is4First)
    return;
  if ((long(millis()) - DB_priv_time_4) >= interval)
  {
    DB_ISR_F_4 = true;
    DB_priv_time_4 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button4State = true;
  }
}

void ISR_DB_4_R_32()
{
  if (button4State && !is4First)
    return;
  if ((long(millis()) - DB_priv_time_4) >= interval)
  {
    DB_ISR_F_4 = true;
    DB_priv_time_4 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button4State = false;
  }
}

//States what happens when InterruptAction Function is called:
void InterruptAction()
{

  if (DB_ISR_F_1)
  {
    DB_1_Process_Start_Time = millis();

    colorRG1 = X;

    if (button1State)
    {
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

      if (T_packet_state == 0)
      {
        blockStateColor = true;
        button1State = false;
      }
      else
      {
        button1State = true;
      }
#ifndef TEST
      DB_ISR_F_1 = false;
#endif
    }
    else if (!button1State)
    {

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

      if (T_packet_state == 0)
      {
        blockStateColor = false;
        button1State = true;
      }
      else
      {
        button1State = false;
      }

#ifndef TEST
      DB_ISR_F_1 = false;
#endif
    }
  }

  else if (DB_ISR_F_2)
  {
    DB_2_Process_Start_Time = millis();

    colorRG2 = X;

    if (button2State)
    {
#ifdef DEBUG
      Serial.println("\nButton 2 was pressed once!");
#endif

      String("GL2").toCharArray(testData, 50);
      address = 4;
      sendData(address, testData);

      if (T_packet_state == 0)
      {
        blockStateColor = true;
        button2State = false;
      }
      else
      {
        button2State = true;
      }

      DB_ISR_F_2 = false;
    }
    else if (!button2State)
    {
#ifdef DEBUG
      Serial.println("\nButton 2 was pressed twice!");
#endif

      String("RL2").toCharArray(testData, 50);
      address = 4;
      sendData(address, testData);

      if (T_packet_state == 0)
      {
        blockStateColor = false;
        button2State = true;
      }
      else
      {
        button2State = false;
      }
      DB_ISR_F_2 = false;
    }
  }
  // DB 3:
  else if (DB_ISR_F_3)
  {
    DB_3_Process_Start_Time = millis();

    colorRG3 = X;

    if (button3State)
    {
#ifdef DEBUG
      Serial.println("\nButton 3 was pressed once!");
#endif

      String("GL3").toCharArray(testData, 50);
      address = 6;
      sendData(address, testData);

      if (T_packet_state == 0)
      {
        blockStateColor = true;
        button3State = false;
      }
      else
      {
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

      if (T_packet_state == 0)
      {
        blockStateColor = false;
        button3State = true;
      }
      else
      {
        button3State = false;
      }
      DB_ISR_F_3 = false;
    }
  }

  else if (DB_ISR_F_4)
  {
    DB_4_Process_Start_Time = millis();

    colorRG4 = X;

    if (button4State)
    {
#ifdef DEBUG
      Serial.println("\nButton 4 was pressed once!");
#endif

      String("GL4").toCharArray(testData, 50);
      address = 7;
      sendData(address, testData);

      if (T_packet_state == 0)
      {
        blockStateColor = true;
        button4State = false;
      }
      else
      {
        button4State = true;
      }

      DB_ISR_F_4 = false;
    }
    else if (!button4State)
    {
#ifdef DEBUG
      Serial.println("\nButton 4 was pressed twice!");
#endif

      String("RL4").toCharArray(testData, 50);
      address = 7;
      sendData(address, testData);

      if (T_packet_state == 0)
      {
        blockStateColor = false;
        button4State = true;
      }
      else
      {
        button4State = false;
      }
      DB_ISR_F_4 = false;
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

  if (!isTransmissionInProgress)
    isTransmissionInProgress = true;
  else
    return;
  //delay(1000);
  switch (NodeAddress)
  {

  case ADDR_1_INT:
    isTransmissionInProgress_1 = true;
    nextionWriter(NEXTION_TRANSMISSION_1, NEXTION_COMMAND_TEXT, "Transmitting...", false);
    break;

  case ADDR_2_INT:
    isTransmissionInProgress_2 = true;
    nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_COMMAND_TEXT, "Transmitting...", false);

    break;

  case ADDR_3_INT:
    isTransmissionInProgress_3 = true;
    nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_COMMAND_TEXT, "Transmitting...", false);
    break;

  case ADDR_4_INT:
    isTransmissionInProgress_4 = true;
    nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_COMMAND_TEXT, "Transmitting...", false);
    ;
  }

  for (int retry = 1; retry <= 5; retry++)
  {
#ifdef DEBUG
    Serial.print("Attempt: ");
    Serial.println(retry);
#endif
    T_packet_state = sx1278.sendPacketTimeoutACK(NodeAddress, message);

    if (T_packet_state == 0)
    {
#ifdef DEBUG
      Serial.println(F("State = 0 --> Command Executed w no errors!"));
      Serial.println(F("Packet sent..."));
#endif
      isTransmissionInProgress = false;
      attemptDebug = retry;
      switch (NodeAddress)
      {

      case ADDR_1_INT:
        isTransmissionInProgress_1 = false;
        is1Active = true;
        nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_TEXT, "Sent!!", false);
        nextionWriter(NEXTION_TIMER_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        break;

      case ADDR_2_INT:
        isTransmissionInProgress_2 = false;
        nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_TEXT, "Sent!!", false);
        nextionWriter(NEXTION_TIMER_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        is2Active = true;
        break;

      case ADDR_3_INT:
        isTransmissionInProgress_3 = false;
        nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_TEXT, "Sent!!", false);
        nextionWriter(NEXTION_TIMER_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        is3Active = true;
        break;

      case ADDR_4_INT:
        isTransmissionInProgress_4 = false;
        nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_TEXT, "Sent!!", false);
        nextionWriter(NEXTION_TIMER_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        is4Active = true;
        break;
      }
      //break;
      return;
    }
    else if (T_packet_state == 5 || T_packet_state == 6 || T_packet_state == 7)
    {
      Serial.println("Conflict!");
      Serial.print("Error code-");
      Serial.println(T_packet_state);
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
    showTime();
  }

  switch (NodeAddress)
  {
  case ADDR_1_INT:
    isTransmissionInProgress_1 = false;
    is1Active = false;
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_TEXT, "Falied!!", false);
    nextionWriter(NEXTION_TIMER_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    nextionWriter(NEXTION_STATUS_1, NEXTION_COMMAND_TEXT, "Offline", false);
    nextionWriter(NEXTION_STATUS_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    break;

  case ADDR_2_INT:
    isTransmissionInProgress_2 = false;
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_TEXT, "Falied!!", false);
    nextionWriter(NEXTION_TIMER_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    is2Active = false;
    nextionWriter(NEXTION_STATUS_1, NEXTION_COMMAND_TEXT, "Offline", false);
    nextionWriter(NEXTION_STATUS_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    break;

  case ADDR_3_INT:
    isTransmissionInProgress_3 = false;
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_TEXT, "Falied!!", false);
    nextionWriter(NEXTION_TIMER_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    is3Active = false;
    nextionWriter(NEXTION_STATUS_1, NEXTION_COMMAND_TEXT, "Offline", false);
    nextionWriter(NEXTION_STATUS_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    break;

  case ADDR_4_INT:
    isTransmissionInProgress_4 = false;
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_TEXT, "Falied!!", false);
    nextionWriter(NEXTION_TIMER_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    is4Active = false;
    nextionWriter(NEXTION_STATUS_1, NEXTION_COMMAND_TEXT, "Offline", false);
    nextionWriter(NEXTION_STATUS_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    break;
  }
  isTransmissionInProgress = false;
}

void sendData2(uint8_t NodeAddress, char message[])
{
#ifdef DEBUG
  Serial.print("Node Address : ");
  Serial.println(NodeAddress);
#endif

  //  if (!isTransmissionInProgress)
  //    isTransmissionInProgress = true;
  //  else
  //    return;
  //  //delay(1000);
  //  switch (NodeAddress)
  //  {
  //
  //    case ADDR_1_INT:
  //      u8g1.clearBuffer();                 // clear the internal memory
  //      u8g1.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //      u8g1.drawStr(0, 10, ADDR_1_STR); // write something to the internal memory
  //      u8g1.drawStr(15, 30, "Transmitting..");
  //      u8g1.sendBuffer();
  //      break;
  //
  //    case ADDR_2_INT:
  //      u8g2.clearBuffer();                 // clear the internal memory
  //      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //      u8g2.drawStr(0, 10, ADDR_2_STR);
  //      u8g2.drawStr(15, 30, "Transmitting..");
  //      u8g2.sendBuffer();
  //      break;
  //
  //    case ADDR_3_INT:
  //      u8g3.clearBuffer();                 // clear the internal memory
  //      u8g3.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //      u8g3.drawStr(0, 10, ADDR_3_STR);
  //      u8g3.drawStr(15, 30, "Transmitting..");
  //      u8g3.sendBuffer();
  //      break;
  //
  //    case ADDR_4_INT:
  //      u8g4.clearBuffer();                 // clear the internal memory
  //      u8g4.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //      u8g4.drawStr(0, 10, ADDR_4_STR);
  //      u8g4.drawStr(15, 30, "Transmitting..");
  //      u8g4.sendBuffer();
  //      break;
  //  }

  for (int retry = 1; retry <= 5; retry++) //NO RETRIES ON SYNC
  {
#ifdef DEBUG
    Serial.print("Attempt: ");
    Serial.println(retry);
#endif
    T_packet_state = sx1278.sendPacketTimeoutACK(NodeAddress, message);

    if (T_packet_state == 0)
    {
#ifdef DEBUG
      Serial.println(F("State = 0 --> Command Executed w no errors!"));
      Serial.println(F("Packet sent..."));
#endif
      isTransmissionInProgress = false;

      //      switch (NodeAddress)
      //      {
      //
      //        case ADDR_1_INT:
      //          u8g1.clearBuffer();                 // clear the internal memory
      //          u8g1.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      //          u8g1.drawStr(0, 10, ADDR_1_STR); // write something to the internal memory
      //          u8g1.drawStr(15, 30, "Sent!");
      //          is1Active = true;
      //          u8g1.sendBuffer();
      //          break;
      //
      //        case ADDR_2_INT:
      //          u8g2.clearBuffer();                 // clear the internal memory
      //          u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      //          u8g2.drawStr(0, 10, ADDR_2_STR);
      //          u8g2.drawStr(15, 30, "Sent!");
      //          is2Active = true;
      //          u8g2.sendBuffer();
      //          break;
      //
      //        case ADDR_3_INT:
      //          u8g3.clearBuffer();                 // clear the internal memory
      //          u8g3.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      //          u8g3.drawStr(0, 10, ADDR_3_STR);
      //          u8g3.drawStr(15, 30, "Sent!");
      //          is3Active = true;
      //          u8g3.sendBuffer();
      //          break;
      //
      //        case ADDR_4_INT:
      //          u8g4.clearBuffer();                 // clear the internal memory
      //          u8g4.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      //          u8g4.drawStr(0, 10, ADDR_4_STR);
      //          u8g4.drawStr(15, 30, "Sent!");
      //          is4Active = true;
      //          u8g4.sendBuffer();
      //          break;
      //      }
      //break;
      return;
    }
    else if (T_packet_state == 5 || T_packet_state == 6 || T_packet_state == 7)
    {
      Serial.println("Conflict!");
      Serial.print("Error code-");
      Serial.println(T_packet_state);
      //sendData(address, testData);
    }
    else
    {
#ifdef DEBUG
      Serial.print(F("Error Code: "));
      Serial.println(T_packet_state);
      Serial.println(F("Packet not sent...."));
#endif
    }
    showTime();
  }

  //  switch (NodeAddress)
  //  {
  //
  //    case ADDR_1_INT:
  //      u8g1.clearBuffer();                 // clear the internal memory
  //      u8g1.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //      u8g1.drawStr(0, 10, ADDR_1_STR); // write something to the internal memory
  //      u8g1.drawStr(15, 30, "Falied!");
  //      is1Active = false;
  //      u8g1.drawStr(15, 30, "Offline");
  //      u8g1.sendBuffer();
  //      break;
  //
  //    case ADDR_2_INT:
  //      u8g2.clearBuffer();                 // clear the internal memory
  //      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //      u8g2.drawStr(0, 10, ADDR_2_STR);
  //      u8g2.drawStr(15, 30, "Falied!");
  //      is2Active = false;
  //      u8g2.drawStr(15, 30, "Offline");
  //      u8g2.sendBuffer();
  //      break;
  //
  //    case ADDR_3_INT:
  //      u8g3.clearBuffer();                 // clear the internal memory
  //      u8g3.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //      u8g3.drawStr(0, 10, ADDR_3_STR);
  //      u8g3.drawStr(15, 30, "Falied!");
  //      is3Active = false;
  //      u8g3.drawStr(15, 30, "Offline");
  //      u8g3.sendBuffer();
  //      break;
  //
  //    case ADDR_4_INT:
  //      u8g4.clearBuffer();                 // clear the internal memory
  //      u8g4.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  //      u8g4.drawStr(0, 10, ADDR_4_STR);
  //      u8g4.drawStr(15, 30, "Falied!");
  //      is4Active = false;
  //      u8g4.drawStr(15, 30, "Offline");
  //      u8g4.sendBuffer();
  //      break;
  //  }

  isTransmissionInProgress = false;
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

// //Experimental show signal time in seconds
void showTime()
{
  currentMil = millis();
  int time1 = (currentMil - DB_1_Signal_Runtime) / 1000;
  int time2 = (currentMil - DB_2_Signal_Runtime) / 1000;
  int time3 = (currentMil - DB_3_Signal_Runtime) / 1000;
  int time4 = (currentMil - DB_4_Signal_Runtime) / 1000;

  String printt1 = String(time1 % 60) + " Sec";
  String printt2 = String(time2 % 60) + " Sec";
  String printt3 = String(time3 % 60) + " Sec";
  String printt4 = String(time4 % 60) + " Sec";

  if (time1 / 60 != 0)
    printt1 = String(time1 / 60) + " Min " + printt1;
  if (time2 / 60 != 0)
    printt2 = String(time2 / 60) + " Min " + printt2;
  if (time3 / 60 != 0)
    printt3 = String(time3 / 60) + " Min " + printt3;
  if (time4 / 60 != 0)
    printt4 = String(time4 / 60) + " Min " + printt4;

  switch (colorRG1)
  {

  case R:
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_TEXT, printt1, false);
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_BACKGROUND, NEXTION_RED, true);
    nextionWriter(NEXTION_TIMER_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case G:
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_TEXT, printt1, false);
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_BACKGROUND, NEXTION_GREEN, true);
    nextionWriter(NEXTION_TIMER_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_BLACK, true);
    break;
  case X:
    break;
  }

  switch (colorRG2)
  {
  case R:
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_TEXT, printt2, false);
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_BACKGROUND, NEXTION_RED, true);
    nextionWriter(NEXTION_TIMER_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case G:
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_TEXT, printt2, false);
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_BACKGROUND, NEXTION_GREEN, true);
    nextionWriter(NEXTION_TIMER_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_BLACK, true);
    break;
  case X:
    break;
  }

  switch (colorRG3)
  {
  case R:
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_TEXT, printt3, false);
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_BACKGROUND, NEXTION_RED, true);
    nextionWriter(NEXTION_TIMER_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case G:
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_TEXT, printt3, false);
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_BACKGROUND, NEXTION_GREEN, true);
    nextionWriter(NEXTION_TIMER_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_BLACK, true);
    break;
  case X:
    break;
  }

  switch (colorRG4)
  {
  case R:
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_TEXT, printt4, false);
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_BACKGROUND, NEXTION_RED, true);
    nextionWriter(NEXTION_TIMER_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case G:
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_TEXT, printt4, false);
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_BACKGROUND, NEXTION_GREEN, true);
    nextionWriter(NEXTION_TIMER_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_BLACK, true);
    break;
  case X:
    break;
  }
}

//FINAL  BLOCK STATE
void Setting_Block_State_Color()
{
  //Sets the block state for FIRST Location
  if (receivedMsg.equals("KL1"))
  {

    if (blockStateColor)
    {
      colorRG1 = G;
      DB_1_Signal_Runtime = millis();
    }
    else if (!blockStateColor)
    {
      colorRG1 = R;
      DB_1_Signal_Runtime = millis();
    }
    DB_1_Process_End_Time = millis();
    t_time = ((DB_1_Process_End_Time - DB_1_Process_Start_Time) / 1000.0);
    Serial.print("Required time (L1): ");
    Serial.println(t_time, 3);

    is1First = false;
  }

  //Sets the block state for SECOND location
  if (receivedMsg.equals("KL2"))
  {
    if (blockStateColor)
    {
      colorRG2 = G;
      DB_2_Signal_Runtime = millis();
    }
    else if (!blockStateColor)
    {
      colorRG2 = R;
      DB_2_Signal_Runtime = millis();
    }
    DB_2_Process_End_Time = millis();
    Serial.print("Required time (L2): ");
    Serial.println(((DB_2_Process_End_Time - DB_2_Process_Start_Time) / 1000.0), 3);
    is2First = false;
  }

  // Sets the block state for THIRD location
  if (receivedMsg.equals("KL3"))
  {
    if (blockStateColor)
    {
      colorRG3 = G;
      DB_3_Signal_Runtime = millis();
    }
    else if (!blockStateColor)
    {
      colorRG3 = R;
      DB_3_Signal_Runtime = millis();
      // location1Sec.attach(5, showTime);
    }
    DB_3_Process_End_Time = millis();
    Serial.print("Required time (L3): ");
    //    if (countDebug < 10)
    //      Serial.print(" ");
    //    Serial.print(countDebug++);
    Serial.print(" ");
    Serial.println(((DB_3_Process_End_Time - DB_3_Process_Start_Time) / 1000.0), 3);
    //    Serial.print(" ");
    //    Serial.println(attemptDebug);
    is3First = false;
  }

  //Sets the block state for FOURTH location
  if (receivedMsg.equals("KL4"))
  {
    if (blockStateColor)
    {
      colorRG4 = G;
      DB_4_Signal_Runtime = millis();
    }
    else if (!blockStateColor)
    {
      colorRG4 = R;
      DB_4_Signal_Runtime = millis();
    }
    DB_4_Process_End_Time = millis();
    Serial.print("Required time (L4): ");
    Serial.println(((DB_4_Process_End_Time - DB_4_Process_Start_Time) / 1000.0), 3);
    is4First = false;
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

  nextionWriter(NEXTION_LOCATION_1, NEXTION_COMMAND_TEXT, ADDR_1_STR, false);
  nextionWriter(NEXTION_LOCATION_2, NEXTION_COMMAND_TEXT, ADDR_2_STR, false);
  nextionWriter(NEXTION_LOCATION_3, NEXTION_COMMAND_TEXT, ADDR_3_STR, false);
  nextionWriter(NEXTION_LOCATION_4, NEXTION_COMMAND_TEXT, ADDR_4_STR, false);

  nextionWriter(NEXTION_STATUS_1, NEXTION_COMMAND_TEXT, "Offline", false);
  nextionWriter(NEXTION_STATUS_2, NEXTION_COMMAND_TEXT, "Offline", false);
  nextionWriter(NEXTION_STATUS_3, NEXTION_COMMAND_TEXT, "Offline", false);
  nextionWriter(NEXTION_STATUS_4, NEXTION_COMMAND_TEXT, "Offline", false);

  nextionWriter(NEXTION_TRANSMISSION_1, NEXTION_COMMAND_TEXT, "", false);
  nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_COMMAND_TEXT, "", false);
  nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_COMMAND_TEXT, "", false);
  nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_COMMAND_TEXT, "", false);

  nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_TEXT, "0 Min 0 Sec", false);
  nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_TEXT, "0 Min 0 Sec", false);
  nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_TEXT, "0 Min 0 Sec", false);
  nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_TEXT, "0 Min 0 Sec", false);

  nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_BACKGROUND, NEXTION_BLACK, true);
  nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_BACKGROUND, NEXTION_BLACK, true);
  nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_BACKGROUND, NEXTION_BLACK, true);
  nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_BACKGROUND, NEXTION_BLACK, true);

  nextionWriter(NEXTION_TIMER_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
  nextionWriter(NEXTION_TIMER_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
  nextionWriter(NEXTION_TIMER_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
  nextionWriter(NEXTION_TIMER_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);

  nextionWriter(NEXTION_STATUS_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
  nextionWriter(NEXTION_STATUS_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
  nextionWriter(NEXTION_STATUS_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
  nextionWriter(NEXTION_STATUS_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);

  // u8g1.begin();
  // u8g2.begin();
  // u8g3.begin();
  // u8g4.begin();

  // u8g1.clearBuffer();                 // clear the internal memory
  // u8g1.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  // u8g1.drawStr(0, 10, ADDR_1_STR);    // write something to the internal memory
  // u8g1.drawStr(20, 20, "Offline");
  // u8g1.sendBuffer(); // transfer internal memory to the display
  // delay(1000);

  // u8g2.clearBuffer();                 // clear the internal memory
  // u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  // u8g2.drawStr(0, 10, ADDR_2_STR);    // write something to the internal memory
  // u8g2.drawStr(20, 20, "Offline");
  // u8g2.sendBuffer(); // transfer internal memory to the display
  // delay(1000);

  // u8g3.clearBuffer();                 // clear the internal memory
  // u8g3.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  // u8g3.drawStr(0, 10, ADDR_3_STR);    // write something to the internal memory
  // u8g3.drawStr(20, 20, "Offline");
  // u8g3.sendBuffer(); // transfer internal memory to the display
  // delay(1000);

  // u8g4.clearBuffer();                 // clear the internal memory
  // u8g4.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
  // u8g4.drawStr(0, 10, ADDR_4_STR);    // write something to the internal memory
  // u8g4.drawStr(20, 20, "Offline");
  // u8g4.sendBuffer(); // transfer internal memory to the display
  // delay(1000);
}

// void autoTransmission() {
//   //This portion below is for  Automatic InterruptAction:
//   //**************************************************
//   #ifdef automation
//   autoTime1 = millis();
//   if((autoTime1 - autoTime2) > 2000){
//     ISR_DB_1_G_32();
//     autoTime2 = autoTime1;
//   }
//   #endif
//   //**************************************************
// }
