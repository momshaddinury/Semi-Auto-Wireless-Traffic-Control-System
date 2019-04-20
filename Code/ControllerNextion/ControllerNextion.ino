// ---------------------------------------------------------------------
/*
  Project Name - Semi Automatic Traffic Control System using Lora SX1278
  Components used:
           MCU - ESP32
           Communication module - Lora SX1278 ( FSK Mode )
           Display - Nextion 3.5"
           Power Source - LC 18650
  Author - Momshad Dinury
  Contributor:
           Abdullah Zowad Khan
           Towqir Ahmed Shaem
           
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

//DEBUG FLAGS:
// #define DEBUG_ALL
// #define DEBUG_DISPLAY
#define DEBUG_TRANSMISSION
#define DEBUG_FUNCTION_CALL
#define DEBUG_INTERRUPT

#ifdef DEBUG_ALL
#define DEBUG_DISPLAY
#define DEBUG_TRANSMISSION
#define DEBUG_FUNCTION_CALL
#define DEBUG_INTERRUPT
#endif

#define CONTROLLER

//TESTCASE:
//#define TEST
//#define TESTDEBUG

/*****************************************************************************
   Definitions & Declarations
 *****************************************************************************/

//Lora SX1278:
#define LORA_SS_Pin 5 //ESP32
#define LORA_MODE 4
#define LORA_CHANNEL CH_6_BW_125
#define LORA_ADDRESS 5
uint8_t NodeAddress; //Child Address
int address;

//NEXTION Color values
#define NEXTION_RED "63488"
#define NEXTION_DARK_RED "43008"
#define NEXTION_BLACK "0"
#define NEXTION_GREEN "1632"
#define NEXTION_LIGHT_GREEN "2016"
#define NEXTION_DARK_GREEN "512"
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
#define NEXTION_TIMER_2 "t9"
#define NEXTION_TIMER_3 "t10"
#define NEXTION_TIMER_4 "t11"

#define NEXTION_COMMAND_TEXT "txt"
#define NEXTION_COMMAND_BACKGROUND "bco"
#define NEXTION_FOREGROUND_TEXT_COLOR "pco"

#define ADDR_1_INT 3
#define ADDR_1_STR "LANE 1"

#define ADDR_2_INT 4
#define ADDR_2_STR "LANE 2"

#define ADDR_3_INT 6
#define ADDR_3_STR "LANE 3"

#define ADDR_4_INT 7
#define ADDR_4_STR "LANE 4"

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

unsigned long firstTime;
unsigned long secondTime;

//Automatic Transmission Flags and Var:
// #define automation
long int autoTime1 = 0;
long int autoTime2 = 0;

//*****
int signalTimeCount = 0;

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

int syncCounter = 0;

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

  // pinMode(27, OUTPUT);
  // digitalWrite(27, LOW); // To Light Up The Display.

  //Initialization of timers
  DB_1_Signal_Runtime = millis();
  DB_2_Signal_Runtime = millis();
  DB_3_Signal_Runtime = millis();
  DB_4_Signal_Runtime = millis();

  colorRG1 = X;
  colorRG2 = X;
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

  /**************************AUTO-TRANSMISSION*************************/
  //  if (countDebug % 2 == 0) ISR_DB_3_G_32();
  //  else ISR_DB_3_R_32();

  //////////////////////////////////////////////////////////////////////
  //RSSI value:
  //   int state = sx1278.getRSSI();
  //   if (!state) {
  // #ifdef DEBUG
  //     Serial.print("RSSI");
  //     Serial.print(" ");
  //     Serial.println(sx1278._RSSI);
  // #endif
  //   }

  //Power:
  // int power = sx1278.getPower();
//   sx1278.getPower();
// #ifdef DEBUG_TRANSMISSION
//   Serial.print("Power");
//   Serial.print(" ");
//   Serial.println(sx1278._power);
// #endif
  /////////////////////////////////////////////////////////////////////
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

    if (syncCounter < 5)
    {
      if (!is1Active)
      {
        address = ADDR_1_INT;
        sendData2(ADDR_1_INT, testData);
        delay(300);
      }
      receiveSync();

      if (!is2Active)
      {
        address = ADDR_2_INT;
        sendData2(ADDR_2_INT, testData);
        delay(300);
      }
      receiveSync();

      if (!is3Active)
      {
        address = ADDR_3_INT;
        sendData2(ADDR_3_INT, testData);
        delay(300);
      }
      receiveSync();

      if (!is4Active)
      {
        address = ADDR_4_INT;
        sendData2(ADDR_4_INT, testData);
        delay(300);
      }
      receiveSync();
      syncCounter++;
    }

    if ((is1Active && is2Active && is3Active && is4Active) || syncCounter >= 5)
      resetCondition = false;
  }
}

void nextionWriter(String id, String command, String value, boolean isColor)
{

  String data;
  if (isColor)
    data = id + "." + command + "=" + value;
  else
    data = id + "." + command + "=\"" + value + "\"";
#ifdef DEBUG_DISPLAY
  Serial.println(data);
#endif
  Serial2.print(data);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}

void receiveSync()
{
  firstTime = millis();
// #ifdef DEBUG_FUNCTION_CALL
//   Serial.println(" ReceiveSync .... ");
// #endif
  R_packet_state = sx1278.receivePacketTimeoutACK();
  if (R_packet_state == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Package received!"));
    //RSSI value:
    int state = sx1278.getRSSI();
    // if (!state) {
    Serial.print("RSSI");
    Serial.print(" ");
    Serial.println(sx1278._RSSI);
    // }
#endif
    for (unsigned int i = 0; i < sx1278.packet_received.length; i++)
    {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
#ifdef DEBUG_TRANSMISSION
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
  secondTime = millis();
// #ifdef DEBUG_FUNCTION_CALL
//   Serial.print("Total Time in ReceiveSync : ");
//   Serial.println(secondTime - firstTime);
// #endif
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
#ifdef DEBUG_INTERRUPT
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
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
#ifdef DEBUG_INTERRUPT
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
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
#ifdef DEBUG_INTERRUPT
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
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
#ifdef DEBUG_INTERRUPT
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
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
#ifdef DEBUG_INTERRUPT
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
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
#ifdef DEBUG_INTERRUPT
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
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
#ifdef DEBUG_INTERRUPT
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
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
#ifdef DEBUG_INTERRUPT
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
#endif
    button4State = false;
  }
}

/* Changed so that last color state is continued on failure, note variable lastcolor and colorRGX (1, 2, 3, 4)*/

/* Changed so that last time is continued on failure, note variable DB_Process_Last_Time and DB_X_Process_Start_Time (1, 2, 3, 4)*/

//States what happens when InterruptAction Function is called:
void InterruptAction()
{
  int lastcolor = 0;
  unsigned long DB_Process_Last_Time = 0;
  if (DB_ISR_F_1)
  {
    DB_Process_Last_Time = DB_1_Process_Start_Time;
    DB_1_Process_Start_Time = millis();
    lastcolor = colorRG1;
    colorRG1 = X;

    if (button1State)
    {
#ifdef DEBUG_FUNCTION_CALL
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
        colorRG1 = lastcolor;
        DB_1_Process_Start_Time = DB_Process_Last_Time;
      }
#ifndef TEST
      DB_ISR_F_1 = false;
#endif
    }
    else if (!button1State)
    {

#ifdef DEBUG_FUNCTION_CALL
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
        colorRG1 = lastcolor;
        DB_1_Process_Start_Time = DB_Process_Last_Time;
      }

#ifndef TEST
      DB_ISR_F_1 = false;
#endif
    }
  }

  else if (DB_ISR_F_2)
  {
    DB_Process_Last_Time = DB_2_Process_Start_Time;
    DB_2_Process_Start_Time = millis();
    lastcolor = colorRG2;
    colorRG2 = X;

    if (button2State)
    {
#ifdef DEBUG_FUNCTION_CALL
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
        colorRG2 = lastcolor;
        DB_2_Process_Start_Time = DB_Process_Last_Time;
      }

      DB_ISR_F_2 = false;
    }
    else if (!button2State)
    {
#ifdef DEBUG_FUNCTION_CALL
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
        colorRG2 = lastcolor;
        DB_2_Process_Start_Time = DB_Process_Last_Time;
      }
      DB_ISR_F_2 = false;
    }
  }
  // DB 3:
  else if (DB_ISR_F_3)
  {
    DB_Process_Last_Time = DB_3_Process_Start_Time;
    DB_3_Process_Start_Time = millis();
    lastcolor = colorRG3;
    colorRG3 = X;

    if (button3State)
    {
#ifdef DEBUG_FUNCTION_CALL
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
        colorRG3 = lastcolor;
        DB_3_Process_Start_Time = DB_Process_Last_Time;
      }
      DB_ISR_F_3 = false;
    }
    else if (!button3State)
    {
#ifdef DEBUG_FUNCTION_CALL
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
        colorRG3 = lastcolor;
        DB_3_Process_Start_Time = DB_Process_Last_Time;
      }
      DB_ISR_F_3 = false;
    }
  }

  else if (DB_ISR_F_4)
  {
    DB_Process_Last_Time = DB_4_Process_Start_Time;
    DB_4_Process_Start_Time = millis();
    lastcolor = colorRG4;
    colorRG4 = X;

    if (button4State)
    {
#ifdef DEBUG_FUNCTION_CALL
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
        colorRG4 = lastcolor;
        DB_4_Process_Start_Time = DB_Process_Last_Time;
      }

      DB_ISR_F_4 = false;
    }
    else if (!button4State)
    {
#ifdef DEBUG_FUNCTION_CALL
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
        colorRG4 = lastcolor;
        DB_4_Process_Start_Time = DB_Process_Last_Time;
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
#ifdef DEBUG_TRANSMISSION
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
    nextionWriter(NEXTION_TRANSMISSION_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case ADDR_2_INT:
    isTransmissionInProgress_2 = true;
    nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_COMMAND_TEXT, "Transmitting...", false);
    nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);

    break;

  case ADDR_3_INT:
    isTransmissionInProgress_3 = true;
    nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_COMMAND_TEXT, "Transmitting...", false);
    nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case ADDR_4_INT:
    isTransmissionInProgress_4 = true;
    nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_COMMAND_TEXT, "Transmitting...", false);
    nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    ;
  }

  for (int retry = 1; retry <= 5; retry++)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.print("Attempt: ");
    Serial.println(retry);
#endif
    T_packet_state = sx1278.sendPacketTimeoutACK(NodeAddress, message);

    if (T_packet_state == 0)
    {
#ifdef DEBUG_TRANSMISSION
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
        nextionWriter(NEXTION_TRANSMISSION_1, NEXTION_COMMAND_TEXT, "Sent!!", false);
        nextionWriter(NEXTION_TRANSMISSION_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        nextionWriter(NEXTION_STATUS_1, NEXTION_COMMAND_TEXT, "Active", false);
        nextionWriter(NEXTION_STATUS_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        break;

      case ADDR_2_INT:
        isTransmissionInProgress_2 = false;
        nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_COMMAND_TEXT, "Sent!!", false);
        nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        nextionWriter(NEXTION_STATUS_2, NEXTION_COMMAND_TEXT, "Active", false);
        nextionWriter(NEXTION_STATUS_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        is2Active = true;
        break;

      case ADDR_3_INT:
        isTransmissionInProgress_3 = false;
        nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_COMMAND_TEXT, "Sent!!", false);
        nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        nextionWriter(NEXTION_STATUS_3, NEXTION_COMMAND_TEXT, "Active", false);
        nextionWriter(NEXTION_STATUS_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        is3Active = true;
        break;

      case ADDR_4_INT:
        isTransmissionInProgress_4 = false;
        nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_COMMAND_TEXT, "Sent!!", false);
        nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        nextionWriter(NEXTION_STATUS_4, NEXTION_COMMAND_TEXT, "Active", false);
        nextionWriter(NEXTION_STATUS_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_GREEN, true);
        is4Active = true;
        break;
      }
      //break;
      return;
    }
    else if (T_packet_state == 5 || T_packet_state == 6 || T_packet_state == 7)
    {
#ifdef DEBUG_TRANSMISSION
      Serial.println("Conflict!");
      Serial.print("Error code-");
      Serial.println(T_packet_state);
#endif
      sendData(address, testData);
    }
    else
    {
#ifdef DEBUG_TRANSMISSION
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
    nextionWriter(NEXTION_TRANSMISSION_1, NEXTION_COMMAND_TEXT, "Falied!!", false);
    nextionWriter(NEXTION_TRANSMISSION_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    nextionWriter(NEXTION_STATUS_1, NEXTION_COMMAND_TEXT, "Offline", false);
    nextionWriter(NEXTION_STATUS_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    break;

  case ADDR_2_INT:
    isTransmissionInProgress_2 = false;
    nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_COMMAND_TEXT, "Falied!!", false);
    nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    is2Active = false;
    nextionWriter(NEXTION_STATUS_2, NEXTION_COMMAND_TEXT, "Offline", false);
    nextionWriter(NEXTION_STATUS_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    break;

  case ADDR_3_INT:
    isTransmissionInProgress_3 = false;
    nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_COMMAND_TEXT, "Falied!!", false);
    nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    is3Active = false;
    nextionWriter(NEXTION_STATUS_3, NEXTION_COMMAND_TEXT, "Offline", false);
    nextionWriter(NEXTION_STATUS_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    break;

  case ADDR_4_INT:
    isTransmissionInProgress_4 = false;
    nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_COMMAND_TEXT, "Falied!!", false);
    nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    is4Active = false;
    nextionWriter(NEXTION_STATUS_4, NEXTION_COMMAND_TEXT, "Offline", false);
    nextionWriter(NEXTION_STATUS_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_RED, true);
    break;
  }
  isTransmissionInProgress = false;
}

void sendData2(uint8_t NodeAddress, char message[])
{
#ifdef DEBUG_TRANSMISSION
  Serial.print("Node Address : ");
  Serial.println(NodeAddress);
#endif

  for (int retry = 1; retry <= 1; retry++) //NO RETRIES ON SYNC
  {
#ifdef DEBUG_TRANSMISSION
    Serial.print("Attempt: ");
    Serial.println(retry);
#endif
    T_packet_state = sx1278.sendPacketTimeoutACK(NodeAddress, message);

    if (T_packet_state == 0)
    {
#ifdef DEBUG_TRANSMISSION
      Serial.println(F("State = 0 --> Command Executed w no errors!"));
      Serial.println(F("Packet sent..."));
#endif
      isTransmissionInProgress = false;
      return;
    }
    else if (T_packet_state == 5 || T_packet_state == 6 || T_packet_state == 7)
    {
#ifdef DEBUG_TRANSMISSION
      Serial.println("Conflict!");
      Serial.print("Error code-");
      Serial.println(T_packet_state);
#endif
      //sendData(address, testData);
    }
    else
    {
#ifdef DEBUG_TRANSMISSION
      Serial.print(F("Error Code: "));
      Serial.println(T_packet_state);
      Serial.println(F("Packet not sent...."));
#endif
    }
    showTime();
  }

  isTransmissionInProgress = false;
}

//Global receive data function
void recieveData()
{
  R_packet_state = sx1278.receivePacketTimeoutACK();
  if (R_packet_state == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Package received!"));
#endif
    for (unsigned int i = 0; i < sx1278.packet_received.length; i++)
    {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
#ifdef DEBUG_TRANSMISSION
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
#ifdef DEBUG_DISPLAY
  String yx = String(colorRG1) + String(colorRG2) + String(colorRG3) + String(colorRG4) + "";
  Serial.println(yx);
#endif
  currentMil = millis();
  int time1 = (currentMil - DB_1_Signal_Runtime) / 1000;
  int time2 = (currentMil - DB_2_Signal_Runtime) / 1000;
  int time3 = (currentMil - DB_3_Signal_Runtime) / 1000;
  int time4 = (currentMil - DB_4_Signal_Runtime) / 1000;

  char printx1[50];
  char printx2[50];
  char printx3[50];
  char printx4[50];

  char printm1[50];
  char printm2[50];
  char printm3[50];
  char printm4[50];

  sprintf(printx1, "%02d", time1 % 60);
  sprintf(printx2, "%02d", time2 % 60);
  sprintf(printx3, "%02d", time3 % 60);
  sprintf(printx4, "%02d", time4 % 60);

  sprintf(printm1, "%02d", time1 / 60);
  sprintf(printm2, "%02d", time2 / 60);
  sprintf(printm3, "%02d", time3 / 60);
  sprintf(printm4, "%02d", time4 / 60);

  String printt1 = String(printm1) + ":" + String(printx1);
  String printt2 = String(printm2) + ":" + String(printx2);
  String printt3 = String(printm3) + ":" + String(printx3);
  String printt4 = String(printm4) + ":" + String(printx4);

  switch (colorRG1)
  {

  case R:
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_TEXT, printt1, false);
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_BACKGROUND, NEXTION_DARK_RED, true);
    nextionWriter(NEXTION_TIMER_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case G:
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_TEXT, printt1, false);
    nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_BACKGROUND, NEXTION_DARK_GREEN, true);
    nextionWriter(NEXTION_TIMER_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;
  case X:
    break;
  }

  switch (colorRG2)
  {
  case R:
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_TEXT, printt2, false);
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_BACKGROUND, NEXTION_DARK_RED, true);
    nextionWriter(NEXTION_TIMER_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case G:
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_TEXT, printt2, false);
    nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_BACKGROUND, NEXTION_DARK_GREEN, true);
    nextionWriter(NEXTION_TIMER_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;
  case X:
    break;
  }

  switch (colorRG3)
  {
  case R:
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_TEXT, printt3, false);
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_BACKGROUND, NEXTION_DARK_RED, true);
    nextionWriter(NEXTION_TIMER_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case G:
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_TEXT, printt3, false);
    nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_BACKGROUND, NEXTION_DARK_GREEN, true);
    nextionWriter(NEXTION_TIMER_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;
  case X:
    break;
  }

  switch (colorRG4)
  {
  case R:
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_TEXT, printt4, false);
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_BACKGROUND, NEXTION_DARK_RED, true);
    nextionWriter(NEXTION_TIMER_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
    break;

  case G:
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_TEXT, printt4, false);
    nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_BACKGROUND, NEXTION_DARK_GREEN, true);
    nextionWriter(NEXTION_TIMER_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
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
#ifdef DEBUG_TRANSMISSION
    Serial.print("Required time (L1): ");
    Serial.println(t_time, 3);
#endif

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
#ifdef DEBUG_TRANSMISSION
    Serial.print("Required time (L2): ");
    Serial.println(((DB_2_Process_End_Time - DB_2_Process_Start_Time) / 1000.0), 3);
#endif
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
#ifdef DEBUG_TRANSMISSION
    Serial.print("Required time (L3): ");
    Serial.print(" ");
    Serial.println(((DB_3_Process_End_Time - DB_3_Process_Start_Time) / 1000.0), 3);
#endif
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
#ifdef DEBUG_TRANSMISSION
    Serial.print("Required time (L4): ");
    Serial.println(((DB_4_Process_End_Time - DB_4_Process_Start_Time) / 1000.0), 3);
#endif
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
  if (sx1278.ON(LORA_SS_Pin) == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting power ON: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting power ON: ERROR "));
#endif
  }

  // Set transmission mode and print the result:
  if (sx1278.setMode(LORA_MODE) == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting Mode: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting Mode: ERROR "));
#endif
  }

  // Set header:
  if (sx1278.setHeaderON() == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting Header ON: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting Header ON: ERROR "));
#endif
  }

  // Select frequency channel:
  if (sx1278.setChannel(LORA_CHANNEL) == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting Channel: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting Channel: ERROR "));
#endif
  }

  // Set CRC:
  if (sx1278.setCRC_ON() == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting CRC ON: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting CRC ON: ERROR "));
#endif
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting Power: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting Power: ERROR "));
#endif
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0)
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting node address: SUCCESS "));
#endif
  }
  else
  {
#ifdef DEBUG_TRANSMISSION
    Serial.println(F("Setting node address: ERROR "));
#endif
  }

  // Print a success
#ifdef DEBUG_TRANSMISSION
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

  nextionWriter(NEXTION_TIMER_1, NEXTION_COMMAND_TEXT, "00:00", false);
  nextionWriter(NEXTION_TIMER_2, NEXTION_COMMAND_TEXT, "00:00", false);
  nextionWriter(NEXTION_TIMER_3, NEXTION_COMMAND_TEXT, "00:00", false);
  nextionWriter(NEXTION_TIMER_4, NEXTION_COMMAND_TEXT, "00:00", false);

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

  nextionWriter(NEXTION_TRANSMISSION_1, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
  nextionWriter(NEXTION_TRANSMISSION_2, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
  nextionWriter(NEXTION_TRANSMISSION_3, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
  nextionWriter(NEXTION_TRANSMISSION_4, NEXTION_FOREGROUND_TEXT_COLOR, NEXTION_WHITE, true);
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
