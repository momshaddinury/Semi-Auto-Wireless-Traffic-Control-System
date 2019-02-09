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
//Lora SX1278:
#include <SX1278.h>
//Library for Ticker
#include <Ticker.h>
//Library for Display
#include <U8g2lib.h>

/*****************************************************************************
   Definitions & Declarations
 *****************************************************************************/

U8G2_PCD8544_84X48_F_4W_HW_SPI u8g1(U8G2_R0, /* cs=*/4, /* dc=*/23, /* reset=*/100); // Nokia 5110 Display
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/2, /* dc=*/23, /* reset=*/100); // Nokia 5110 Display
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g3(U8G2_R0, /* cs=*/15, /* dc=*/23, /* reset=*/100); // Nokia 5110 Display
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g4(U8G2_R0, /* cs=*/17, /* dc=*/23, /* reset=*/100); // Nokia 5110 Display


//Lora SX1278:
#define LORA_MODE 		4
#define LORA_CHANNEL 	CH_6_BW_125
#define LORA_ADDRESS 	5
uint8_t NodeAddress; 	//Child Address
int 	address;

//Packets var:
char 		my_packet[50];
char 		testData[50];
String 		receivedMsg;
int 		T_packet_state;
int 		R_packet_state;

//Pin def of Switch:
#define digitalButton_1 21
#define digitalButton_2 22
#define digitalButton_3 32
#define digitalButton_4 33
#define digitalButton_5 25
#define digitalButton_6 26
#define digitalButton_7 18
#define digitalButton_8 19
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

  //PinMode:
  //Location 1 Button:
  pinMode(digitalButton_1, INPUT_PULLUP);
  pinMode(digitalButton_2, INPUT_PULLUP);
  //Location 2 Button:
  pinMode(digitalButton_3, INPUT_PULLUP);
  pinMode(digitalButton_4, INPUT_PULLUP);
  //Location 3 Button:
  pinMode(digitalButton_5, INPUT_PULLUP);
  pinMode(digitalButton_6, INPUT_PULLUP);
  //Location 4 Button:
  pinMode(digitalButton_7, INPUT_PULLUP);
  pinMode(digitalButton_8, INPUT_PULLUP);

  //Initialization of timers
  DB_1_Signal_Runtime = millis(); //L 1
  DB_2_Signal_Runtime = millis(); //L 2
  DB_3_Signal_Runtime = millis();
  DB_4_Signal_Runtime = millis();

  colorRG1 = X;	//L 1
  colorRG2 = X;	//L 2
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
  //autoTransmission();

  InterruptAction();
  //This function checks for data to receive
  recieveData();
  //Show time on display:
  showTime();







}

//RESETs connected Nodes
void sync()
{
  if (resetCondition == true)
  {
    String("S").toCharArray(testData, 50);
    address = 0;
    sendData(address, testData);

    if (T_packet_state == 0)
    {
      resetCondition = false;
    }
  }
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

void ISR_DB_3_G_32()
{
  if ((long(millis()) - DB_priv_time_3) >= interval)
  {
    DB_ISR_F_3 = true;
    DB_priv_time_3 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button3State = true;
    // DB_priv_time_2 = millis();
  }
}

void ISR_DB_3_R_32()
{
  if ((long(millis()) - DB_priv_time_3) >= interval)
  {
    DB_ISR_F_3 = true;
    DB_priv_time_3 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button3State = false;
    // DB_priv_time_2 = millis();
  }
}

void ISR_DB_4_G_32()
{
  if ((long(millis()) - DB_priv_time_4) >= interval)
  {
    DB_ISR_F_4 = true;
    DB_priv_time_4 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button4State = true;
    // DB_priv_time_2 = millis();
  }
}

void ISR_DB_4_R_32()
{
  if ((long(millis()) - DB_priv_time_4) >= interval)
  {
    DB_ISR_F_4 = true;
    DB_priv_time_4 = millis();
    Serial.print("\n##");
    Serial.print(count += 1);
    Serial.print("\t");
    button4State = false;
    // DB_priv_time_2 = millis();
  }
}

//States what happens when InterruptAction Function is called:
void InterruptAction()
{
  //DB 1:
  if (DB_ISR_F_1)
  {
    DB_1_Process_Start_Time = millis();

    colorRG1 = X;

    if (button1State)
    {
      //DB_1_Signal_Runtime = millis();
      //location1Sec.attach(1, showTime);
      // signalTimeCount = 0;

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
      //DB_1_Signal_Runtime = millis();
      //signalTimeCount = 0;
      //location1Sec.attach(1, showTime);

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

  //DB 2:
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
  //DB 4:
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

  //delay(1000);

  T_packet_state = sx1278.sendPacketTimeoutACKRetries(NodeAddress, message);

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

  String printt1 = String(time1) + "S";
  String printt2 = String(time2) + "S";
  String printt3 = String(time3) + "S";
  String printt4 = String(time4) + "S";

  switch (colorRG1)
  {
    case R:

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

      u8g2.clearBuffer();                 // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt2 = "Red " + printt2;
      u8g2.drawStr(20, 40, printt2.c_str()); // write something to the internal memory
      u8g2.drawStr(0, 10, "LOCATION BAI");
      u8g2.sendBuffer();
      break;
    case G:
      u8g2.clearBuffer();                 // clear the internal memory
      u8g2.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt2 = "Green " + printt2;
      u8g2.drawStr(20, 40, printt2.c_str()); // write something to the internal memory
      u8g2.drawStr(0, 10, "LOCATION BAI");
      u8g2.sendBuffer();
      break;
    case X:
      break;
  }

  switch (colorRG3)
  {
    case R:
      u8g3.clearBuffer();                 // clear the internal memory
      u8g3.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt3 = "Red " + printt3;
      u8g3.drawStr(20, 40, printt3.c_str()); // write something to the internal memory
      u8g3.drawStr(0, 10, "LOCATION OXY");
      u8g3.sendBuffer();
      break;
    case G:
      u8g3.clearBuffer();                 // clear the internal memory
      u8g3.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt3 = "Green " + printt3;
      u8g3.drawStr(20, 40, printt3.c_str()); // write something to the internal memory
      u8g3.drawStr(0, 10, "LOCATION OXY");
      u8g3.sendBuffer();
      break;
    case X:
      break;
  }

  switch (colorRG4)
  {
    case R:
      u8g4.clearBuffer();                 // clear the internal memory
      u8g4.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt4 = "Red " + printt4;
      u8g4.drawStr(20, 40, printt4.c_str()); // write something to the internal memory
      u8g4.drawStr(0, 10, "LOCATION MUR");
      u8g4.sendBuffer();
      break;
    case G:
      u8g4.clearBuffer();                 // clear the internal memory
      u8g4.setFont(u8g2_font_ncenB08_tr); // choose a suitable font
      printt4 = "Green " + printt4;
      u8g4.drawStr(20, 40, printt4.c_str()); // write something to the internal memory
      u8g4.drawStr(0, 10, "LOCATION MUR");
      u8g4.sendBuffer();
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
      colorRG1 = G;
      DB_1_Signal_Runtime = millis();
      // location1Sec.attach(2, showTime);
    }
    else if (!blockStateColor)
    {
      colorRG1 = R;
      DB_1_Signal_Runtime = millis();
      // location1Sec.attach(2, showTime);
    }
    DB_1_Process_End_Time = millis();
    t_time = ((DB_1_Process_End_Time - DB_1_Process_Start_Time) / 1000.0);
    Serial.print("Required time (L1): ");
    Serial.println(t_time, 3);
  }

  //Sets the block state for SECOND location
  if (receivedMsg.equals("KL2"))
  {
    if (blockStateColor)
    {
      colorRG2 = G;
      DB_2_Signal_Runtime = millis();
      // location1Sec.attach(5, showTime);
    }
    else if (!blockStateColor)
    {
      colorRG2 = R;
      DB_2_Signal_Runtime = millis();
      // location1Sec.attach(5, showTime);
    }
    DB_2_Process_End_Time = millis();
    Serial.print("Required time (L2): ");
    Serial.println(((DB_2_Process_End_Time - DB_2_Process_Start_Time) / 1000.0), 3);
  }

  // Sets the block state for THIRD location
  if (receivedMsg.equals("KL3"))
  {
    if (blockStateColor)
    {
      colorRG3 = G;
      DB_3_Signal_Runtime = millis();
      // location1Sec.attach(5, showTime);
    }
    else if (!blockStateColor)
    {
      colorRG3 = R;
      DB_3_Signal_Runtime = millis();
      // location1Sec.attach(5, showTime);
    }
    DB_3_Process_End_Time = millis();
    Serial.print("Required time (L3): ");
    Serial.println(((DB_3_Process_End_Time - DB_3_Process_Start_Time) / 1000.0), 3);
  }

  //Sets the block state for FOURTH location
  if (receivedMsg.equals("KL4"))
  {
    if (blockStateColor)
    {
      colorRG4 = G;
      DB_4_Signal_Runtime = millis();
      // location1Sec.attach(5, showTime);
    }
    else if (!blockStateColor)
    {
      colorRG4 = R;
      DB_4_Signal_Runtime = millis();
      // location1Sec.attach(5, showTime);
    }
    DB_4_Process_End_Time = millis();
    Serial.print("Required time (L4): ");
    Serial.println(((DB_4_Process_End_Time - DB_4_Process_Start_Time) / 1000.0), 3);
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
  u8g1.begin();
  u8g2.begin();
  u8g3.begin();
  u8g4.begin();

  u8g1.clearBuffer();                  	  // clear the internal memory
  u8g1.setFont(u8g2_font_ncenB08_tr);  	  // choose a suitable font
  u8g1.drawStr(0, 10, "LOCATION GEC"); 	  // write something to the internal memory
  u8g1.sendBuffer();                   	  // transfer internal memory to the display
  delay(1000);

  u8g2.clearBuffer();                     // clear the internal memory
  u8g2.setFont(u8g2_font_ncenB08_tr);     // choose a suitable font
  u8g2.drawStr(0, 10, "LOCATION BAIZID"); // write something to the internal memory
  u8g2.sendBuffer();                      // transfer internal memory to the display
  delay(1000);

  u8g3.clearBuffer();                     // clear the internal memory
  u8g3.setFont(u8g2_font_ncenB08_tr);     // choose a suitable font
  u8g3.drawStr(0, 10, "LOCATION OXY"); 	  // write something to the internal memory
  u8g3.sendBuffer();                      // transfer internal memory to the display
  delay(1000);

  u8g4.clearBuffer();                     // clear the internal memory
  u8g4.setFont(u8g2_font_ncenB08_tr);     // choose a suitable font
  u8g4.drawStr(0, 10, "LOCATION MUR"); 	  // write something to the internal memory
  u8g4.sendBuffer();                      // transfer internal memory to the display
  delay(1000);
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