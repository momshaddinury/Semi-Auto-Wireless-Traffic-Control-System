#ifndef Source_h
#define Source_h

/******************************************************************************
 * Includes
 ******************************************************************************/
Including Library:
 #include <Arduino.h>
// Including Library for 1.8 TFT DispLay:
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
//Lora SX1278:
#include <SX1278.h>
//Library for Ticker
#include <Ticker.h>

/******************************************************************************
 * Definitions & Declarations
 *****************************************************************************/

//// TFT Display Pin For Arduino
#define TFT_CS                            10
#define TFT_RST                           16
#define TFT_DC                            0
#define TFT_SCLK                          14
#define TFT_MOSI                          13

// Color Code For TFT Display
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Variable For TFT Display Rectangle
int rect1x = 4;
int rect1y = 25;
int rect2y = 47;
int rect3y = 69;
int rect4y = 91;
int recwidth = 122;
int recheight = 20;

// Creating Object of TFT Display
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Lora SX1278:
#define LORA_MODE             4
#define LORA_CHANNEL          CH_6_BW_125
#define LORA_ADDRESS          5
uint8_t NodeAddress;          //Child Address
int address;

//Packets var:
char my_packet [50];
char testData[50];
String receivedMsg;
int T_packet_state;
int R_packet_state;

//Pin def of Switch:
#define digitalButton_1 2
#define digitalButton_2 4
#define digitalButton_3 5
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
Ticker Location1, Location2, Location3, Location4, statusRectToggler;

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

//Serial Print and Debug:
#define DEBUG

//TESTCASE:
#define TEST
#define TESTDEBUG
// bool testDebug = true;
int yaxis = 20;
int count = 0;
boolean automation_toggle = true;

void sync();
void statusSecTiggerFunction();

void ISR_DB_1();
void ISR_DB_2();
void ISR_DB_3();

void InterruptAction();
void AnalogAction();

void sendData(uint8_t NodeAddress, char message[]);
void recieveData();

void Blink_Location_Rect_1();
void Blink_Location_Rect_2();
void Blink_Location_Rect_3();
void Blink_Location_Rect_4();

void Setting_Block_State_Color();

void loraSetup();
void displaySetup();
void printDouble(double val, unsigned int precision);
void check_config();

#endif
