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