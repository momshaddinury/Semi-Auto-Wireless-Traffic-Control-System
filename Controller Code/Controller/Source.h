//Including Library:
#include <Arduino.h>
#include <SX1278.h>

// Including Library for 1.8 TFT DispLay
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

//Library for Ticker
#include <Ticker.h>

//// TFT Display Pin For Arduino
#define TFT_CS  5 //10
#define TFT_RST 17
#define TFT_DC  0
// #define TFT_SCLK 14
// #define TFT_MOSI 13

// Color Code For Tft Display
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Variable For TFT Display
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
#define LORA_ADDRESS          5
uint8_t NodeAddress = 3;

char my_packet [50];
char testData[50];

int T_packet_state;
int R_packet_state;

//Pin def;
#define interruptButton 2
#define digitalButton 16
#define analogButton A0
//Analog button value storing variable:
int AB_value;
//Locations:
String Location;
String Location_1 = "GEC";
String Location_2 = "BAIZID";
String Location_3 = "MURADPUR";
String Location_4 = "PROBARTAK";
boolean blockStateColor;
// boolean blockStateColor2;
Ticker Location1, Location2, Location3, Location4;

boolean locationBlock_1 = true;
boolean locationBlock_2 = true;
boolean locationBlock_3 = true;
boolean locationBlock_4 = true;
// Button State
boolean button1State = true;
boolean button2State = true;
boolean button3State = true;
boolean button4State = true;
//Flags, Var for interrupt:
boolean B_ISR_F_F = false;
boolean B_F_B_F = true;
//Sync:
boolean resetCondition = true;
boolean resetStop;
//Timer:
long interval = 2000;
volatile unsigned long DB_priv_time;
volatile unsigned long AB_priv_time_3;
volatile unsigned long AB_priv_time_4;

