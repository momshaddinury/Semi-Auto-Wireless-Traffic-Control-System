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
#include "Source.h"
//MAIN SETUP FUNCTION
void setup() {
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
  pinMode(digitalButton_1, INPUT);
  pinMode(digitalButton_2, INPUT);
  pinMode(digitalButton_3, INPUT);
  pinMode(analogButton,    INPUT);

  //Interrupt:
  attachInterrupt(digitalPinToInterrupt(digitalButton_1), ISR_DB_1, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_2), ISR_DB_2, FALLING);
  attachInterrupt(digitalPinToInterrupt(digitalButton_3), ISR_DB_3, FALLING);
  }

//MAIN LOOP
void loop() {
  //After the device is booted it automatically re-boots other device:
  sync();

  //To use pin 2 as switch:
  digitalWrite(digitalButton_1, HIGH);
  digitalWrite(0, LOW);

  //Check Config:
  //check_config();
  
  //This functions is for InterruptAction:
  InterruptAction();

  //This function checks for data to receive
  recieveData();
  
  //Analog functions:
  AnalogAction();

  //This function checks for data to receive
  recieveData();
  }

