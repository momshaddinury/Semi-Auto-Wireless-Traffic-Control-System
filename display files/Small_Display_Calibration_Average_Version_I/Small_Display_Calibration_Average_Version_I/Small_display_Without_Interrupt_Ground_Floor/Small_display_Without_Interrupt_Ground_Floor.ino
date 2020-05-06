//Non_Zero_Bits average in Calibration version_1


#include <EEPROM.h>
#include <WiFi.h>
HardwareSerial Serial2(2);
//#define DEBUG true;

unsigned long prev_millis = 0;
//boolean variables
bool Initial_Error_Detection_FLAG = true;
bool Tacho_Pulse_Missing = false;
bool Buzzer_ON_FLAG = false;
bool Bundle_Mode_FLAG = false;
bool Sheet_Start_FLAG = false;
bool Sajib = false;
bool Debug_Pinstate = 0;
bool FLAG = 0;
bool Error_Sheet_Pinstate = 0;
bool CalibrationPinstate = 0;
bool Calibration_Select = 0;
bool Cali_Loop_Select = 0;
bool UDstate = 0;
bool UDstate1 = 0;
bool Unit_Change_Pinstate = 0;
bool Bundle_Selection_Pinstate = 0;
bool ReSet_Pinstate = 0;
bool Already_On_FLAG = false;
bool Area_Calculate_In_Calibraion_FLAG = false;

//byte variables
byte Changed_From_Zero = 0;
byte Debug_Press_Counter = 0;
byte Unit_Select_Count = 0;
byte Bundle_Selection_Countrer = 0;

//integer variables
int Non_Zero_Bits_Previous_Scan = 0;
int Non_Zero_Bits_Current_Scan = 0;
int Non_Zero_Bits_Current_Scan_C = 0;
int Non_Zero_Bits_Previous_Scan_C = 0;
int zero_RST = 0;
int Zero_Bits_One_Data_Set = 0;
int i = 0;
int Sheet_in_Bundle = 0;
int Sheet_Running_Bundle = 0;
int Scan_Counter = 0;
int Sheet_Passed_In_Calibration = 0;


//integer array variables
int One_Set_Data[80] = {0};
int One_Set_Data_For_Error[80] = {0};
int Previous_Data_Error[80] = {0};
int Was_Once_Non_Zero[80] = {0};


//float variables
float Bit_Adjustment = 0.0;
float Bit_Adjustment_C = 0.0;
float Non_Zero_Bits_One_Sheet = 0.0;
float Area = 0.0;
float Previous_Area = 0.0;
float Plus_Minus = 0.0;
float Pre_Value_Plus_Minus = 0.0;
float Bundle_Area = 0.00;

//double variables
double X = 0.00104822;
double X_Read_EEPROM_Fraction_Part = 0.00;
double Total_Area = 0;
double Fraction_X = 0.00;
double Full = 0.0;

//long variables
long Sheet_Count = 0;
long Make_X_Integer = 0;
long Make_Fraction_X_Int = 0;
long Sheet_Count_Bundle_Press = 0;


//unsigned long variables
unsigned long Tacho_Missing_Time = 0;
unsigned long Latch_Width = 50;//50 // 100;//1000//
unsigned long Clock_width = 10;//10//2;//10//
unsigned long To = 0;
unsigned long Td = 0;
unsigned long T3t = 0;
unsigned long Debounce_Delay = 0;
unsigned long Buzzer_Beep_Time_Off = 0;
unsigned long Buzzer_Beep_Time_On = 0;
unsigned long Time_Delay_Bit_Read = 0;
unsigned long Time_Delay_Bit_Read_C = 0;
unsigned long Buzzer_Beep_Time = 0;
unsigned long Interrupt_Time = 0;
unsigned long Previous_Interrupt_Time = 0;
unsigned long TIME_WIDTH = 0;
unsigned long Entering_Before_While = 0;

//static unsigned long variables
static unsigned long DELAY = 0;

//Pin configuration...........
int ReSetPin = 18;
int ClockPin = 4;
int DataPin =  2;
int LatchPin = 5;
int TachoPin = 15;
int CalibrationPin = 19;
int DownPin = 13;
int UpPin = 21;
int Error_Sheet_Pin = 23;
int Unit_Change_Pin = 22;
int Bundle_Selection_Pin = 12;
int BuzzerPin = 26;
int Debug_Pin = 14;
//int EXIT_Pin = 19;


//diclarations of all functions
void Initialization();
void Initial_Error_Detection();
void Buzzer_On_Off();
void Data_Collection();
void Operation();
void X_Read_From_EEPROM();
void Error_Sheet_Cancel();
float Up_Down_Button(float UD);
void CALCULATION(float Area_Input_Calculation, float Non_Zero_Bit_In_Calculation);
bool CALIBRATION(bool Cali_Select);
void Area_Sheet_Count_Saved_EEPROM(double Total_Area_in_Function);
double Area_Sheet_Count_Read_EEPROM();
void Memory_Clean();
void  Zero_Error_Detection_Part_One();
void Zero_Error_Detection_Part_Two();
void ReSet_Button_Function();
byte Unit_Change_Function();
float Area_Round_Up(float Area_in_Function_Round_Up);
double Total_Area_Round_Up(double Total_Area_in_Function_Round_Up);
void Bundle_Mode_Selection();
void Bundle_Sheet_Set_Mode();
void Control_Latch();
void Tacho_Pulse_Check();
void When_Sheet_Count_9999();
void Bit_Arrangement();

void setup() {
  //#ifdef DEBUG
  Serial.begin(115200);
  //#endif
  Serial2.begin(57600);
  EEPROM.begin(512);
  WiFi.mode(WIFI_OFF);
  btStop();
  delay(2000);
  pinMode(ReSetPin, INPUT_PULLUP);
  pinMode(ClockPin, OUTPUT);
  pinMode(DataPin, INPUT);
  pinMode(LatchPin, OUTPUT);
  pinMode(TachoPin, INPUT_PULLUP);
  pinMode(CalibrationPin, INPUT_PULLUP);
  pinMode(DownPin, INPUT_PULLUP);
  pinMode(UpPin, INPUT_PULLUP);
  pinMode(Error_Sheet_Pin, INPUT_PULLUP);
  pinMode(Unit_Change_Pin, INPUT_PULLUP);
  pinMode(Bundle_Selection_Pin, INPUT_PULLUP);
  pinMode(BuzzerPin, OUTPUT);
  pinMode(Debug_Pin, INPUT_PULLUP);
  digitalWrite(BuzzerPin, LOW);



  //                    for (int cv = 0; cv < 512; cv++)
  //                    {
  //                      EEPROM.write(cv, 255);
  //                    }
  //                    EEPROM.commit();
  //  digitalWrite(ClockPin, HIGH);
  //  digitalWrite(LatchPin, HIGH);
  Initialization();
  delay(1000);
}

void loop() {

  if (digitalRead(Debug_Pin) == LOW)
  {
    Debug_Pinstate = 1;
    Debounce_Delay = micros();
  }
  else if ((micros() - Debounce_Delay) > 10000 && Debug_Pinstate == 1)
  {
    if (digitalRead(Debug_Pin) == HIGH)
    {
      Debounce_Delay = micros();
      Debug_Pinstate = 0;
      Serial2.print("page 2");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);

      Debug_Press_Counter = 1;
      Tacho_Pulse_Check();
    }
  }




  if (Sheet_Start_FLAG == false)
  {
    Serial2.print("p1.pic=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }

  if ((Sheet_Start_FLAG == true) && (Already_On_FLAG == false))
  {
    Serial2.print("p1.pic=5");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }

  Buzzer_On_Off();

  if (Initial_Error_Detection_FLAG == true)
  {
    Initial_Error_Detection();
  }

  Bundle_Mode_Selection();

  if (digitalRead(ReSetPin) == LOW)
  {
    ReSet_Pinstate = 1;
    Debounce_Delay = micros();
  }
  else if ((micros() - Debounce_Delay) > 10000 && ReSet_Pinstate == 1)
  {
    if (digitalRead(ReSetPin) == HIGH)
    {
      Debounce_Delay = micros();
      ReSet_Pinstate = 0;
      ReSet_Button_Function();
    }
  }

  if (digitalRead(CalibrationPin) == LOW)
  {
    CalibrationPinstate = 1;
    Debounce_Delay = micros();
  }
  if ((micros() - Debounce_Delay) > 10000 && CalibrationPinstate == 1)
  {
    if ((digitalRead(CalibrationPin) == HIGH) && (Unit_Select_Count == 0))
    {
      Debounce_Delay = micros();
      Calibration_Select = 1;
      CalibrationPinstate = 0;
      Area = 0;
      Zero_Bits_One_Data_Set = 0;
      Non_Zero_Bits_One_Sheet = 0;
      Sheet_Running_Bundle = 0;
      Sheet_Count_Bundle_Press = Sheet_Count;
      Bundle_Area = 0;
      Area_Sheet_Count_Saved_EEPROM(Total_Area);

      Serial2.print("t6.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print("0"); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print("0"); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t3.pco=63488");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print("0.0"); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
    else
    {
      Calibration_Select = 0;
      CalibrationPinstate = 0;
    }
  }

  while (Calibration_Select == 1)
  {
    if (Sheet_Start_FLAG == false)
    {
      Serial2.print("p1.pic=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }

    if (Sheet_Start_FLAG == true)
    {
      Serial2.print("p1.pic=5");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
    if (digitalRead(TachoPin) == LOW)
    {
      Calibration_Select = CALIBRATION(Calibration_Select);
      while (digitalRead(TachoPin) == LOW) {}
    }
  }

  if (digitalRead(TachoPin) == LOW)
  {
    //    Serial.print("LT");
    //    Serial.print(millis() - TIME_WIDTH);
    //    Serial.print("CC");
    //    Serial.println(millis() - To);
    To = millis();
    Operation();
    Td = millis();
    T3t = Td - To;
    //    #ifdef DEBUG
    //    Serial.println();
    //    Serial.print("Time difference=  ");
    //    Serial.print(T3t);
    //    Serial.println();
    //     #endif
    Tacho_Pulse_Missing = false;
    Tacho_Missing_Time = millis();
    Entering_Before_While = millis();
    while (digitalRead(TachoPin) == LOW)
    {
      Already_On_FLAG = false;
      if ((millis() - Entering_Before_While) >= 400)
      {
        Serial2.print("p0.pic=1");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.print("p1.pic=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
      }
    }
    TIME_WIDTH = millis();
  }
  else
  {
    Tacho_Pulse_Missing = true;
  }

  if ((Tacho_Pulse_Missing == true) && ((millis() - Tacho_Missing_Time) >= 200))
  {
    Already_On_FLAG = true;
    Serial2.print("p0.pic=1");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("p1.pic=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  else
  {
    Serial2.print("p0.pic=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }

}


//****************************************************************//
//****************************************************************//

void Initialization()
{
  Serial2.print("page 1");
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(3);

  Total_Area = Area_Sheet_Count_Read_EEPROM();
  if (EEPROM.read(506) == 1)
  {
    Bundle_Mode_FLAG = true;
    Sheet_Count_Bundle_Press = Sheet_Count - Sheet_Running_Bundle;
    if (Sheet_Running_Bundle == Sheet_in_Bundle)
    {
      Sheet_Count_Bundle_Press = Sheet_Count;
    }
  }
  if ((EEPROM.read(507)) != 255)
  {
    Unit_Select_Count = (EEPROM.read(507));

    if (Unit_Select_Count == 0)
    {
      Serial2.print("t0.txt=");
      Serial2.print("\"");
      Serial2.print("sft");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
      Serial2.print("t9.txt=");
      Serial2.print("\"");
      Serial2.print("sft");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
    }
    if (Unit_Select_Count == 1)
    {
      Changed_From_Zero = 1;
      Serial2.print("t0.txt=");
      Serial2.print("\"");
      Serial2.print("qsf");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
      Serial2.print("t9.txt=");
      Serial2.print("\"");
      Serial2.print("qsf");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
    }
    if (Unit_Select_Count == 2)
    {
      Changed_From_Zero = 2;
      Serial2.print("t0.txt=");
      Serial2.print("\"");
      Serial2.print("sdm");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
      Serial2.print("t9.txt=");
      Serial2.print("\"");
      Serial2.print("sdm");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
    }
    if (Unit_Select_Count == 3)
    {
      Changed_From_Zero = 3;
      Serial2.print("t0.txt=");
      Serial2.print("\"");
      Serial2.print("sm");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
      Serial2.print("t9.txt=");
      Serial2.print("\"");
      Serial2.print("sm");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
    }
    if ((Unit_Select_Count == 0) || (Unit_Select_Count == 2) || (Unit_Select_Count == 3))
    {
      Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Total_Area, 1); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
      Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Bundle_Area, 1); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
    }
    if (Unit_Select_Count == 1)
    {
      Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Total_Area, 2); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
      Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Bundle_Area, 2); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(3);
    }
  }
  else
  {
    Unit_Select_Count = 0;
    Serial2.print("t0.txt=");
    Serial2.print("\"");
    Serial2.print("sft");
    Serial2.print("\"");
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(3);
    Serial2.print("t9.txt=");
    Serial2.print("\"");
    Serial2.print("sft");
    Serial2.print("\"");
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(3);
    Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Total_Area, 1); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(3);
    Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Bundle_Area, 1); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(3);
  }
  Serial2.print("t5.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print(Sheet_Count);  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(3);
  Serial2.print("t6.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print(Sheet_Running_Bundle);  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(3);
  Serial2.print("t7.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print(Sheet_in_Bundle);  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(3);
  Serial2.print("p0.pic=1");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(3);
  Serial2.print("p1.pic=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  delay(3);
}

//****************************************************************//
//****************************************************************//


void Initial_Error_Detection()
{
  if (digitalRead(TachoPin) == LOW)
  {
    Control_Latch();
    Data_Collection();
    if (Zero_Bits_One_Data_Set == 80)
    {
      Serial2.print("t1.txt=");
      Serial2.print("\"");
      Serial2.print(" ");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t12.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print("");
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Zero_Bits_One_Data_Set = 0;
    }
    else
    {
      int SENSOR_NUMBER_ONE;
      String Broken_One;
      for (int z = 0; z < 80; z++)
      {
        if (One_Set_Data_For_Error[z] == 1)
        {
          SENSOR_NUMBER_ONE = z + 1;
          Broken_One = (String("") + Broken_One + SENSOR_NUMBER_ONE + ",");
        }
      }
      Serial2.print("t1.txt=");
      Serial2.print("\"");
      Serial2.print("FAULT:");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t12.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Broken_One);
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Zero_Bits_One_Data_Set = 0;
    }
    for (int IED = 0; IED < 80; IED++)
    {
      One_Set_Data[IED] = 0;
      One_Set_Data_For_Error[IED] = 0;
      Non_Zero_Bits_Previous_Scan = 0;
      Non_Zero_Bits_One_Sheet = 0;
    }
    Initial_Error_Detection_FLAG = false;
    while (digitalRead(TachoPin) == LOW) {}
  }
}

//********************************************************//
//*******************************************************//

void Data_Collection()
{
  unsigned long       Time_Delay_Bit_Read_One = 0;
  unsigned long       Time_Delay_Bit_Read_Two = 0;
  bool     clock_flag = true;
  Control_Latch();

  for (i = 0; i < 80; )
  {
    // Serial.println(i);
    if (((micros() - Time_Delay_Bit_Read_One) > Clock_width) && clock_flag == true)
    {
      Time_Delay_Bit_Read_Two = micros();
      clock_flag = false;
      digitalWrite(ClockPin, LOW);
      FLAG = 1;

    }
    if ( ((micros() - Time_Delay_Bit_Read_Two) > Clock_width) && FLAG == 1)
    {
      clock_flag = true;
      Time_Delay_Bit_Read_One = micros();
      One_Set_Data[i] = digitalRead(DataPin);
      digitalWrite(ClockPin, HIGH);

      Bit_Arrangement();

      if (One_Set_Data[i] == 1)
      {
        Sajib = true;

        Non_Zero_Bits_Current_Scan += 1;
      }
      //#ifdef DEBUG
      //      if ((i % 8) == 7)
      //      {
      //        Serial.print("    ");
      //      }
      //#endif
      i++;
      FLAG = 0;
    }
  }
  // Serial.println();

  if (Sajib == true)
  {
    Scan_Counter += 1;
    //Serial.println("scan_counter");
  }

#ifdef DEBUG
  Serial.println();
#endif
  if (i == 80)
  {
    if ((Non_Zero_Bits_Previous_Scan == 0) && (Non_Zero_Bits_Current_Scan != 0))
    {
      Sheet_Start_FLAG = true;
    }
    Bit_Adjustment = (((Non_Zero_Bits_Current_Scan - Non_Zero_Bits_Previous_Scan)) / 2.00);
    Non_Zero_Bits_One_Sheet = Non_Zero_Bits_One_Sheet + Non_Zero_Bits_Current_Scan + Bit_Adjustment;
    Zero_Bits_One_Data_Set = 80 - Non_Zero_Bits_Current_Scan;
    Bit_Adjustment = 0;
    Non_Zero_Bits_Previous_Scan = Non_Zero_Bits_Current_Scan;
    Non_Zero_Bits_Current_Scan = 0;
  }
}


//********************************************************//
//*******************************************************//

void Control_Latch()
{
  digitalWrite(LatchPin, HIGH);
  delayMicroseconds(Latch_Width);      //delay less than 7 will produce a bug
  digitalWrite(LatchPin, LOW);
}


//****************************************************************//
//****************************************************************//


void Bit_Arrangement()
{
  if ((i % 8) == 7)
  {
    int Bit_Position_For_Error_Sensor = i;
    for (int Bits = (i - 7); Bits < (i + 1); Bits++)
    {
      One_Set_Data_For_Error[Bits] = One_Set_Data[Bit_Position_For_Error_Sensor];
      Bit_Position_For_Error_Sensor -= 1;
      //Serial.print(One_Set_Data_For_Error[Bits]);
    }
  }
}


//****************************************************************//
//****************************************************************//

void Buzzer_On_Off()
{
  if (((millis() - Buzzer_Beep_Time) >= 3000) && (Buzzer_ON_FLAG == true))
  {
    Buzzer_ON_FLAG = false;
    digitalWrite(BuzzerPin, LOW);
  }
  if (((millis() - Buzzer_Beep_Time_Off) >= 500) && (Buzzer_ON_FLAG == true) && (Buzzer_Beep_Time_Off != 0))
  {
#ifdef DEBUG
    Serial.println(">>>>>>>>>>Beep off<<<<<<<<<<<<<");
#endif
    digitalWrite(BuzzerPin, LOW);
    Buzzer_Beep_Time_Off = 0;
    Buzzer_Beep_Time_On = millis();
  }
  if (((millis() - Buzzer_Beep_Time_On) >= 500) && (Buzzer_ON_FLAG == true) && (Buzzer_Beep_Time_On != 0))
  {
#ifdef DEBUG
    Serial.println(">>>>>>>>>>Beep On<<<<<<<<<<<<<");
#endif
    digitalWrite(BuzzerPin, HIGH);
    Buzzer_Beep_Time_On = 0;
    Buzzer_Beep_Time_Off = millis();
  }
}
//********************************************************//
//*******************************************************//


//********************************************************//
//*******************************************************//


void Operation()
{
  Data_Collection();
  Zero_Error_Detection_Part_One();

  Unit_Select_Count = Unit_Change_Function();

  if ((Zero_Bits_One_Data_Set == 80) && ( Sheet_Start_FLAG == true))
  {

    Serial.print("Scan_Counter= ");
    Serial.println(Scan_Counter);

    Sheet_Start_FLAG = false;

    Sajib = false;
    Scan_Counter = 0;


    if ((EEPROM.read(508) != 255))
    {
      X_Read_From_EEPROM();
    }

    if ((EEPROM.read(507)) != 255)
    {
      Unit_Select_Count = EEPROM.read(507);
    }
    else
    {
      Unit_Select_Count = 0;
    }
    if ((Unit_Select_Count == 0) || (Unit_Select_Count == 1))
    {
      Area = X  * Non_Zero_Bits_One_Sheet;
    }
    else if (Unit_Select_Count == 2)
    {
      Area = X * Non_Zero_Bits_One_Sheet * 9.2903;
    }
    else if (Unit_Select_Count == 3)
    {
      Area = X * Non_Zero_Bits_One_Sheet * 0.092903;
    }
    if ((Unit_Select_Count == 0) || (Unit_Select_Count == 2) || (Unit_Select_Count == 3))
    {
      Area = Area * 10;
      Area = round(Area);
      Area = Area / 10;
    }
    if (Unit_Select_Count == 2)
    {
      Area = (int)Area;
    }
  }
  else
  {
    Zero_Bits_One_Data_Set = 0;
  }
  if (Area != 0.0)
  {
    Every_Time_After_Getting_New_Area();
  }
  else
  {
    Zero_Bits_One_Data_Set = 0;
  }
  if (digitalRead(Error_Sheet_Pin) == LOW && Sheet_Count != 0)
  {
    Error_Sheet_Pinstate = 1;
    Debounce_Delay = micros();
  }
  if ((micros() - Debounce_Delay) > 50000 && Error_Sheet_Pinstate == 1 && Sheet_Count != 0)
  {
    if (digitalRead(Error_Sheet_Pin) == HIGH)
    {
      Debounce_Delay = micros();
      Error_Sheet_Pinstate = 0;
      if (Previous_Area != 0)
      {
        Error_Sheet_Cancel();
      }
    }
  }
}


//********************************************************//
//*******************************************************//
void Every_Time_After_Getting_New_Area()
{
  //#ifdef DEBUG
  Serial.println();
  Serial.print("Non-Zero bits = ");
  Serial.println(Non_Zero_Bits_One_Sheet);
  Serial.print("Area Measured= ");
  Serial.println(Area, 6);
  //#endif

  if (Unit_Select_Count == 1)
  {
#ifdef DEBUG
    Serial.print("Area before Round_up:  ");
    Serial.println(Area);
#endif
    Area = Area_Round_Up(Area);
    Total_Area = Total_Area_Round_Up(Total_Area);
    Bundle_Area = Area_Round_Up(Bundle_Area);
#ifdef DEBUG
    Serial.println();
    Serial.print("Area After Round_up:  ");
    Serial.println(Area);
#endif
  }

  Serial2.print("t3.txt=");
  Serial2.print("\"");
  Serial2.print("0.0");
  Serial2.print("\"");
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);

  Zero_Error_Detection_Part_Two();
  Total_Area = Area_Sheet_Count_Read_EEPROM();
  if ((Unit_Select_Count == 0) || (Unit_Select_Count == 2) || (Unit_Select_Count == 3))
  {
    Total_Area = Total_Area * 10;
    Total_Area = round(Total_Area);
    Total_Area = Total_Area / 10;
    Bundle_Area = Bundle_Area * 10;
    Bundle_Area = round(Bundle_Area);
    Bundle_Area = Bundle_Area / 10;
  }
#ifdef DEBUG
  Serial.println();
  Serial.print("Previous Area From EEPROM= ");
  Serial.println(Total_Area, 6);
#endif
  Total_Area = Total_Area + Area;
  Sheet_Count += 1;
  if (Bundle_Mode_FLAG == true)
  {
#ifdef DEBUG
    Serial.println("....................");
    Serial.print("Sheet_Running_Bundle= ");
    Serial.println(Sheet_Running_Bundle);
    Serial.print("Sheet_in_Bundle=  ");
    Serial.println(Sheet_in_Bundle);
#endif

    if (Sheet_in_Bundle != 0)
    {
      if (Sheet_Running_Bundle <= Sheet_in_Bundle)
      {
#ifdef DEBUG
        Serial.println("....................");
#endif
        Bundle_Area += Area;
        Sheet_Running_Bundle += 1;
      }
      if (Sheet_Running_Bundle > Sheet_in_Bundle)
      {
#ifdef DEBUG
        Serial.println("....................");
#endif
        Bundle_Area = Area;
        Sheet_Running_Bundle = 1;
      }
      if ((Sheet_Count - Sheet_Count_Bundle_Press) == Sheet_in_Bundle)
      {
        digitalWrite(BuzzerPin, HIGH);
        Buzzer_Beep_Time = millis();
        Buzzer_Beep_Time_Off = millis();
        Buzzer_ON_FLAG = true;
        Sheet_Count_Bundle_Press = Sheet_Count;
      }

      if ((Unit_Select_Count == 0) || (Unit_Select_Count == 2) || (Unit_Select_Count == 3))
      {
        Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.print(Bundle_Area, 1); // This is the value you want to send to that object and atribute mention before.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
      }
      if (Unit_Select_Count == 1)
      {
        Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.print(Bundle_Area, 2); // This is the value you want to send to that object and atribute mention before.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
      }
      Serial2.print("t6.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Sheet_Running_Bundle); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
  }
  if (Sheet_Count == 9999)
  {
    When_Sheet_Count_9999();
  }

  Area_Sheet_Count_Saved_EEPROM(Total_Area);
#ifdef DEBUG
  Serial.println();
  Serial.print("Number of Sheet =  ");
  Serial.println(Sheet_Count);
  Serial.print("Total area = ");
  Serial.print(Total_Area, 6);
  Serial.println();
#endif
  //NEXTION Display showing commands
  if ((Unit_Select_Count == 0) || (Unit_Select_Count == 2) || (Unit_Select_Count == 3))
  {
    Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Area, 1); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Total_Area, 1); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }

  else if (Unit_Select_Count == 1)
  {
    Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Area, 1); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Total_Area, 2); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  Serial2.print("t5.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print(Sheet_Count);  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);

  Previous_Area = Area;
  Non_Zero_Bits_One_Sheet = 0;
  Area = 0.0;
}



void X_Read_From_EEPROM()
{
  X_Read_EEPROM_Fraction_Part = ((EEPROM.read(508)) * 1000000) + ((EEPROM.read(509)) * 10000) + ((EEPROM.read(510)) * 100) + (EEPROM.read(511));
  X = (X_Read_EEPROM_Fraction_Part / 100000000);
  //#ifdef DEBUG
  Serial.println();
  Serial.print("Value of X= ");
  Serial.println(X, 8);
  //#endif
  if ((X == 0.00000000) || (X >= 1))
  {
    X = 0.00104822;
  }
}

void Error_Sheet_Cancel()
{
  Total_Area = Area_Sheet_Count_Read_EEPROM();
#ifdef DEBUG
  Serial.println();
  Serial.print("Total Area Before Subtruction=  ");
  Serial.println(Total_Area);
  Serial.print("Previous_Area=  ");
  Serial.println(Previous_Area);
#endif
  Total_Area = Total_Area - Previous_Area;
  Sheet_Count -= 1;
  if (Sheet_Count <= 0)
  {
    Total_Area = 0.0;
    Sheet_Count = 0;
  }
  if (Bundle_Mode_FLAG == true)
  {
    Bundle_Area = Bundle_Area - Previous_Area;
    Sheet_Running_Bundle -= 1;
    if (Sheet_Running_Bundle <= 0)
    {
      Bundle_Area = 0.0;
      Sheet_Running_Bundle = 0;
    }
    if (Sheet_Count_Bundle_Press == (Sheet_Count + 1))
    {
      Sheet_Count_Bundle_Press = ((Sheet_Count + 1) - Sheet_in_Bundle);
    }
  }
#ifdef DEBUG
  Serial.println();
  Serial.print("Total Area After Subtruction=  ");
  Serial.println(Total_Area);
#endif
  Previous_Area = 0;
  Area_Sheet_Count_Saved_EEPROM(Total_Area);
  //NEXTION Display showing commands
  Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print("0.0"); // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  if ((Unit_Select_Count == 0) || (Unit_Select_Count == 2) || (Unit_Select_Count == 3))
  {
    Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Total_Area, 1); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    if (Bundle_Mode_FLAG == true)
    {
      Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Bundle_Area, 1); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
  }
  if (Unit_Select_Count == 1)
  {
    Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Total_Area, 2); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    if (Bundle_Mode_FLAG == true)
    {
      Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Bundle_Area, 2); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
  }
  Serial2.print("t5.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print(Sheet_Count);  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  if (Bundle_Mode_FLAG == true)
  {
    Serial2.print("t6.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Sheet_Running_Bundle); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
}

//********************************************************//
//*******************************************************//

float Up_Down_Button(float UD)
{
  if (digitalRead(UpPin) == LOW)
  {
    UDstate = 1;
    Debounce_Delay = micros();
  }
  if ((micros() - Debounce_Delay) > 50000 && UDstate == 1)
  {
    if (digitalRead(UpPin) == HIGH)
    {
      Debounce_Delay = micros();
      UDstate = 0;
      UD += 0.10;
    }
  }
  if (digitalRead(DownPin) == LOW)
  {
    UDstate1 = 1;
    Debounce_Delay = micros();
  }
  if ((micros() - Debounce_Delay) > 50000 && UDstate1 == 1)
  {
    if (digitalRead(DownPin) == HIGH)
    {
      Debounce_Delay = micros();
      UDstate1 = 0;
      UD -= 0.10;
    }
  }
  return (UD);
}

//********************************************************//
//*******************************************************//


void CALCULATION(float Area_Input_Calculation, float Non_Zero_Bit_In_Calculation)
{
  X = (Area_Input_Calculation / (Non_Zero_Bit_In_Calculation));
#ifdef DEBUG
  Serial.println();
  Serial.println(X, 8);
#endif

  Make_X_Integer = long(X);
  Fraction_X = (X - Make_X_Integer);
  Make_Fraction_X_Int = (Fraction_X * 100000000);
#ifdef DEBUG
  Serial.print("Fraction part of X=  .");
  Serial.println(Make_Fraction_X_Int);
#endif
  EEPROM.write(508, (Make_Fraction_X_Int / 1000000));
  EEPROM.write(509, ((Make_Fraction_X_Int % 1000000) / 10000));
  EEPROM.write(510, (((Make_Fraction_X_Int % 1000000) % 10000) / 100));
  EEPROM.write(511, (((Make_Fraction_X_Int % 1000000) % 10000) % 100));
  EEPROM.commit();
}


//********************************************************//
//*******************************************************//

bool CALIBRATION(bool Cali_Select)
{
  Data_Collection();
  Zero_Error_Detection_Part_One();
  if ((Zero_Bits_One_Data_Set == 80) && (Sheet_Start_FLAG == true))
  {
    Sheet_Start_FLAG = false;
    Sheet_Passed_In_Calibration += 1;
  }

  if (digitalRead(CalibrationPin) == LOW)
  {
    CalibrationPinstate = 1;
    Debounce_Delay = micros();
  }
  if ((micros() - Debounce_Delay) > 10000 && CalibrationPinstate == 1)
  {
    if (digitalRead(CalibrationPin) == HIGH)
    {
      Debounce_Delay = micros();
      CalibrationPinstate = 0;
      Area_Calculate_In_Calibraion_FLAG = true;
    }
  }

  if (Area_Calculate_In_Calibraion_FLAG == true)
  {
    Serial.print("Total Non_Zero_Bits_One_Sheet = ");
    Serial.println(Non_Zero_Bits_One_Sheet);
    Serial.print("Sheet_Passed_In_Calibration = ");
    Serial.println(Sheet_Passed_In_Calibration);
    Non_Zero_Bits_One_Sheet = Non_Zero_Bits_One_Sheet / Sheet_Passed_In_Calibration;
    Sheet_Passed_In_Calibration = 0;
    if ((EEPROM.read(508) != 255))
    {
      X_Read_From_EEPROM();
    }
    Area_Calculate_In_Calibraion_FLAG = false;
    Area = X * Non_Zero_Bits_One_Sheet;
    Area = Area * 10;
    Area = round(Area);
    Area = Area / 10;

    //#ifdef DEBUG
    Serial.println();
    Serial.print("Area Measured= ");
    Serial.println(Area);
    Serial.print("Non_Zero_Bit = ");
    Serial.println(Non_Zero_Bits_One_Sheet);
    //#endif

    Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print("0.0"); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);

    Zero_Error_Detection_Part_Two();
    Cali_Loop_Select = 1;

    detachInterrupt(TachoPin);

    Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Area, 1); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }

  while (Cali_Loop_Select == 1)
  {
    Serial2.print("p1.pic=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Pre_Value_Plus_Minus = Plus_Minus;
    Plus_Minus = Up_Down_Button(Plus_Minus);
    float Change = Plus_Minus - Pre_Value_Plus_Minus;
    if (Plus_Minus != Pre_Value_Plus_Minus)
    {
      Area += Change;
      //#ifdef DEBUG
      Serial.println("Calibrated Area= ");
      Serial.print(Area);
      Serial.println();
      //#endif
      Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Area, 1); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
    if (digitalRead(CalibrationPin) == LOW)
    {
      CalibrationPinstate = 1;
      Debounce_Delay = micros();
    }
    if ((micros() - Debounce_Delay) > 10000 && CalibrationPinstate == 1)
    {
      if (digitalRead(CalibrationPin) == HIGH)
      {
        Debounce_Delay = micros();
        Serial2.print("t3.pco=65504");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.print("0.0"); // This is the value you want to send to that object and atribute mention before.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
        Cali_Select = 0;
        Plus_Minus = 0;
        CALCULATION(Area, Non_Zero_Bits_One_Sheet);
        Cali_Loop_Select = 0;
        Pre_Value_Plus_Minus = 0;
        Change = 0;
        Area = 0;
        Non_Zero_Bits_One_Sheet = 0;
        CalibrationPinstate = 0;
        Scan_Counter = 0;
      }
    }
  }

#ifdef DEBUG
  Serial.print("bit count: ");
  Serial.print(Non_Zero_Bits_One_Sheet);
  Serial.println();
#endif
  return (Cali_Select);
}


//********************************************************//
//*******************************************************//

void Area_Sheet_Count_Saved_EEPROM(double Total_Area_in_Function)
{
#ifdef DEBUG
  Serial.println();
  Serial.print("Total_Area_in_Function=  ");
  Serial.println(Total_Area_in_Function);
#endif
  long Make_Total_Area_in_Function_Int = 0;
  float Total_Area_in_Function_Fraction = 0.00;
  int Make_Bundle_Area_int = 0;
  float Bundle_Area_Fraction = 0.00;
  bool FLAG_Initial = false;
  int Address_S = 0;
  Make_Total_Area_in_Function_Int = long(Total_Area_in_Function);
  Total_Area_in_Function_Fraction = Total_Area_in_Function - Make_Total_Area_in_Function_Int;
  Make_Bundle_Area_int = int(Bundle_Area);
  Bundle_Area_Fraction = Bundle_Area - Make_Bundle_Area_int;
  for (int mem = 0; mem < 506; mem++)
  {
    if ((EEPROM.read(mem)) == 'E')
    {
      if ((EEPROM.read(mem + 1)) == 'N')
      {
        if ((EEPROM.read(mem + 2)) == 'D')
        {
          if ((EEPROM.read(mem + 3)) == '~')
          {
            Address_S = mem;
            FLAG_Initial = true;
            break;
          }
        }
      }
    }
  }
  if ((Address_S + 15) >= 506)
  {
#ifdef DEBUG
    Serial.println("Memory Full");
#endif
    EEPROM.write(Address_S, 0);
    EEPROM.write((Address_S + 3), 0);
    EEPROM.commit();
    Address_S = 0;
  }
  if (FLAG_Initial == false)
  {
#ifdef DEBUG
    Serial.println("INITIAL");
#endif
    Address_S = 0;
  }
  EEPROM.write((Address_S + 12), 'E');
  EEPROM.write((Address_S + 13), 'N');
  EEPROM.write((Address_S + 14), 'D');
  EEPROM.write((Address_S + 15), '~');
  EEPROM.write(Address_S, (Make_Total_Area_in_Function_Int / 1000000));
  EEPROM.write((Address_S + 1), ((Make_Total_Area_in_Function_Int % 1000000) / 10000));
  EEPROM.write((Address_S + 2), (((Make_Total_Area_in_Function_Int % 1000000) % 10000) / 100));
  EEPROM.write((Address_S + 3), (((Make_Total_Area_in_Function_Int % 1000000) % 10000) % 100));
  EEPROM.write((Address_S + 4), (Total_Area_in_Function_Fraction * 100));
  EEPROM.write((Address_S + 5), (Sheet_Count / 100));
  EEPROM.write((Address_S + 6), (Sheet_Count % 100));
  EEPROM.write((Address_S + 7), (Make_Bundle_Area_int / 100));
  EEPROM.write((Address_S + 8), (Make_Bundle_Area_int % 100));
  EEPROM.write((Address_S + 9), (Bundle_Area_Fraction * 100));
  EEPROM.write((Address_S + 10), (Sheet_Running_Bundle));
  EEPROM.write((Address_S + 11), (Sheet_in_Bundle));
  EEPROM.commit();
#ifdef DEBUG
  Serial.println((Address_S));
  Serial.println((Address_S + 15));
#endif
}

double Area_Sheet_Count_Read_EEPROM()
{
  int Address_A = 0;
  Full = 0;
  for (int SK = 0; SK < 506; SK++)
  {
    if ((EEPROM.read(SK)) == 'E')
    {
      if ((EEPROM.read(SK + 1)) == 'N')
      {
        if ((EEPROM.read(SK + 2)) == 'D')
        {
          if ((EEPROM.read(SK + 3)) == '~')
          {
            Address_A = SK - 12;
            Full = ((EEPROM.read(Address_A)) * 1000000) + ((EEPROM.read(Address_A + 1)) * 10000) + ((EEPROM.read(Address_A + 2)) * 100) + (EEPROM.read(Address_A + 3)) + (float((EEPROM.read(Address_A + 4)) / 100.00));
            Sheet_Count = ((EEPROM.read(Address_A + 5)) * 100) + (EEPROM.read(Address_A + 6));
            Bundle_Area = ((EEPROM.read(Address_A + 7)) * 100) + (EEPROM.read(Address_A + 8)) + (float((EEPROM.read(Address_A + 9)) / 100.00));
            Sheet_Running_Bundle = EEPROM.read(Address_A + 10);
            Sheet_in_Bundle = EEPROM.read(Address_A + 11);
            break;
          }
        }
      }
    }
  }
  return (Full);
}


void Memory_Clean()
{
  int Mem_Cln_Start = 0;
  for (int Mem_Cln = 0; Mem_Cln < 506; Mem_Cln++)
  {
    if (EEPROM.read(Mem_Cln) == 'E')
    {
      if (EEPROM.read(Mem_Cln + 1) == 'N')
      {
        if (EEPROM.read(Mem_Cln + 2) == 'D')
        {
          if (EEPROM.read(Mem_Cln + 3) == '~')
          {
            Mem_Cln_Start = Mem_Cln;
            break;
          }
        }
      }
    }
  }
  EEPROM.write(Mem_Cln_Start, 0);
  EEPROM.write((Mem_Cln_Start + 3), 0);
  EEPROM.commit();
}

void Zero_Error_Detection_Part_One()
{
  bool FLAG_Error = false;
  for (int U = 0; U < 80; U++)
  {
    if (One_Set_Data_For_Error[U] == 1)
    {
      FLAG_Error = true;
      break;
    }
  }
  if (FLAG_Error == true)
  {
    for (int ur = 0; ur < 80; ur++)
    {
      int Ones = 0;
      Ones = (One_Set_Data_For_Error[ur]) | (Previous_Data_Error[ur]);
      if (Ones == 1)
      {
        Was_Once_Non_Zero[ur] = 1;
      }
      Previous_Data_Error[ur] = One_Set_Data_For_Error[ur];
    }
    FLAG_Error = false;
  }
}

void Zero_Error_Detection_Part_Two()
{
  bool Position_Left_FLAG = true;
  bool Position_Right_FLAG = true;
  byte Position_Right = 0;
  byte Position_Left = 0;

  for (int g = 0; g < 80; g++)
  {
    if ((Was_Once_Non_Zero[g] == 1) && (Position_Left_FLAG == true))
    {
      Position_Left = g;
      Position_Left_FLAG = false;
    }
    if ((Was_Once_Non_Zero[(79 - g)] == 1) && (Position_Right_FLAG == true))
    {
      Position_Right = 79 - g;
      Position_Right_FLAG = false;
    }
  }

  String Broken_Zero;
  int SENSOR_NUMBER_ZERO;
  for (int L = Position_Left; L <= Position_Right; L++)
  {
    if (Was_Once_Non_Zero[L] == 0)
    {
      SENSOR_NUMBER_ZERO = L + 1;
      Broken_Zero = (String("") + Broken_Zero + SENSOR_NUMBER_ZERO + ",");
    }
  }
  if (Broken_Zero == 0)
  {
#ifdef DEBUG
    Serial.println("There is no error");
#endif
    Serial2.print("t1.txt=");
    Serial2.print("\"");
    Serial2.print(" ");
    Serial2.print("\"");
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t12.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(" ");
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  else
  {
#ifdef DEBUG
    Serial.print("Error ############################# ==");
    Serial.println(Broken_Zero);
#endif
    Serial2.print("t1.txt=");
    Serial2.print("\"");
    Serial2.print("FAULT:");
    Serial2.print("\"");
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t12.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print(Broken_Zero);
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }

  for (int RT = 0; RT < 80; RT++)
  {
    Previous_Data_Error[RT] = 0;
    Was_Once_Non_Zero[RT] = 0;
  }
}

void ReSet_Button_Function()
{
  Memory_Clean();
  Sheet_Count = 0;
  Total_Area = 0;
  Area = 0;
  if (Bundle_Mode_FLAG == true)
  {
    Sheet_Running_Bundle = 0;
    Bundle_Area = 0;
    Sheet_Count_Bundle_Press = 0;
  }
  Area_Sheet_Count_Saved_EEPROM(Total_Area);

  Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print("0.0"); // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.print("t6.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print("0");  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print("0");  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print("0.0");  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.print("t5.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.print("0");  // This is the value you want to send to that object and atribute mention before.
  Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
  Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
  Serial2.write(0xff);
  Serial2.write(0xff);

  if (digitalRead(TachoPin) == LOW)
  {
    Control_Latch();

    for (i = 0; i < 80; )
    {
      if ((micros() - Time_Delay_Bit_Read) > (2 * Clock_width))
      {
        Time_Delay_Bit_Read = micros();
        digitalWrite(ClockPin, LOW);
        FLAG = 1;
      }
      if ( ((micros() - Time_Delay_Bit_Read) > Clock_width) && FLAG == 1)
      {
        One_Set_Data[i] = digitalRead(DataPin);
        digitalWrite(ClockPin, HIGH);

        Bit_Arrangement();

        if (One_Set_Data[i] == 0)
        {
          zero_RST += 1;
        }
        i++;

        FLAG = 0;
      }
    }

    if (zero_RST == 80)
    {
#ifdef DEBUG
      Serial.println("There is no error");
#endif
      Serial2.print("t1.txt=");
      Serial2.print("\"");
      Serial2.print(" ");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t12.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(" ");
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      zero_RST = 0;
    }
    else
    {
      int SENSOR_NUMBER_ONE;
      String Broken_One;
      for (int z = 0; z < 80; z++)
      {
        if (One_Set_Data_For_Error[z] == 1)
        {
          SENSOR_NUMBER_ONE = z + 1;
          Broken_One = (String("") + Broken_One + SENSOR_NUMBER_ONE + ",");
        }
      }
#ifdef DEBUG
      Serial.println("Error #############################");
#endif
      Serial2.print("t1.txt=");
      Serial2.print("\"");
      Serial2.print("FAULT:");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t12.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Broken_One);
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      zero_RST = 0;
    }
    Calibration_Select = 0;   //only for selecting ReSet Function in Calibration Function
  }
}

byte Unit_Change_Function()
{
  if (digitalRead(Unit_Change_Pin) == LOW)
  {
    Unit_Change_Pinstate = 1;
    Debounce_Delay = micros();
  }
  else if ((micros() - Debounce_Delay) > 50000 && Unit_Change_Pinstate == 1)
  {
    if (digitalRead(Unit_Change_Pin) == HIGH)
    {
      Debounce_Delay = micros();
      Unit_Change_Pinstate = 0;
      Unit_Select_Count += 1;
      if (Unit_Select_Count >= 3)
      {
        Changed_From_Zero = 2;
        Unit_Select_Count = 0;
      }
      else
      {
        Changed_From_Zero = 0;
      }
    }

    if (Unit_Select_Count == 0)
    {
      Serial2.print("t0.txt=");
      Serial2.print("\"");
      Serial2.print("sft");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t9.txt=");
      Serial2.print("\"");
      Serial2.print("sft");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      EEPROM.write(507, 0);
      EEPROM.commit();

      if ((Unit_Select_Count == 0) && (Changed_From_Zero == 2))
      {
        Area = Area * 0.107639;
        Area = Area * 10;
        Area = round(Area);
        Area = Area / 10;
        Bundle_Area = Bundle_Area * 0.107639;
        Bundle_Area = Bundle_Area * 10;
        Bundle_Area = round(Bundle_Area);
        Bundle_Area = Bundle_Area / 10;
        Previous_Area = Previous_Area * 0.107639;
        Previous_Area = Previous_Area * 10;
        Previous_Area = round(Previous_Area);
        Previous_Area = Previous_Area / 10;
        Total_Area = Total_Area * 0.107639;
        Total_Area = Total_Area * 10;
        Total_Area = round(Total_Area);
        Total_Area = Total_Area / 10;
      }
    }
    else if (Unit_Select_Count == 1)
    {
      Serial2.print("t0.txt=");
      Serial2.print("\"");
      Serial2.print("qsf");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t9.txt=");
      Serial2.print("\"");
      Serial2.print("qsf");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      EEPROM.write(507, 1);
      EEPROM.commit();
      Area = Area_Round_Up(Area);
      Bundle_Area = Area_Round_Up(Bundle_Area);
      Previous_Area = Area_Round_Up(Previous_Area);
      Total_Area = Total_Area_Round_Up(Total_Area);
    }
    if (Unit_Select_Count == 2)
    {
      Serial2.print("t0.txt=");
      Serial2.print("\"");
      Serial2.print("sdm");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t9.txt=");
      Serial2.print("\"");
      Serial2.print("sdm");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      EEPROM.write(507, 2);
      Area = Area * 9.2903;
      Area = Area * 10;
      Area = round(Area);
      Area = Area / 10;
      Bundle_Area = Bundle_Area * 9.2903;
      Bundle_Area = Bundle_Area * 10;
      Bundle_Area = round(Bundle_Area);
      Bundle_Area = Bundle_Area / 10;
      Previous_Area = Previous_Area * 9.2903;
      Previous_Area = Previous_Area * 10;
      Previous_Area = round(Previous_Area);
      Previous_Area = Previous_Area / 10;
      Total_Area = Total_Area * 9.2903;
      Total_Area = Total_Area * 10;
      Total_Area = round(Total_Area);
      Total_Area = Total_Area / 10;
    }
    if (Unit_Select_Count == 3)
    {
      Serial2.print("t0.txt=");
      Serial2.print("\"");
      Serial2.print("sm");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t9.txt=");
      Serial2.print("\"");
      Serial2.print("sm");
      Serial2.print("\"");
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      EEPROM.write(507, 3);
      Area = Area / 100;
      Area = Area * 10;
      Area = round(Area);
      Area = Area / 10;
      Bundle_Area = Bundle_Area / 100;
      Bundle_Area = Bundle_Area * 10;
      Bundle_Area = round(Bundle_Area);
      Bundle_Area = Bundle_Area / 10;
      Previous_Area = Previous_Area / 100;
      Previous_Area = Previous_Area * 10;
      Previous_Area = round(Previous_Area);
      Previous_Area = Previous_Area / 10;
      Total_Area = Total_Area / 100;
      Total_Area = Total_Area * 10;
      Total_Area = round(Total_Area);
      Total_Area = Total_Area / 10;
    }
    Area_Sheet_Count_Saved_EEPROM(Total_Area);
    if ((Unit_Select_Count == 0) || (Unit_Select_Count == 2) || (Unit_Select_Count == 3))
    {
      Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Area, 1); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Bundle_Area, 1); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Total_Area, 1); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
    if (Unit_Select_Count == 1)
    {
      Serial2.print("t3.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Area, 2); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Bundle_Area, 2); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t4.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Total_Area, 2); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
  }
  return (Unit_Select_Count);
}
float Area_Round_Up(float Area_in_Function_Round_Up)
{
  long Make_Int_Area_in_Function_Round_Up = 0;
  float Area_Fraction_Part_Round_Up = 0;
  Make_Int_Area_in_Function_Round_Up = long(Area_in_Function_Round_Up);
  Area_Fraction_Part_Round_Up = Area_in_Function_Round_Up - Make_Int_Area_in_Function_Round_Up;
  if ((0.00 <= Area_Fraction_Part_Round_Up) && (Area_Fraction_Part_Round_Up <= 0.13))
  {
    Area_in_Function_Round_Up = Area_in_Function_Round_Up - Area_Fraction_Part_Round_Up;
  }
  else if ((0.14 <= Area_Fraction_Part_Round_Up) && (Area_Fraction_Part_Round_Up <= .37))
  {
    Area_in_Function_Round_Up = (Area_in_Function_Round_Up - Area_Fraction_Part_Round_Up) + 0.25;
  }
  else if ((.38 <= Area_Fraction_Part_Round_Up) && (Area_Fraction_Part_Round_Up <= .62))
  {
    Area_in_Function_Round_Up = (Area_in_Function_Round_Up - Area_Fraction_Part_Round_Up) + 0.50;
  }
  else if ((.63 <= Area_Fraction_Part_Round_Up) && (Area_Fraction_Part_Round_Up <= .86))
  {
    Area_in_Function_Round_Up = (Area_in_Function_Round_Up - Area_Fraction_Part_Round_Up) + 0.75;
  }
  else
  {
    Area_in_Function_Round_Up = (Area_in_Function_Round_Up - Area_Fraction_Part_Round_Up) + 1;
  }
  return (Area_in_Function_Round_Up);
}

double Total_Area_Round_Up(double Total_Area_in_Function_Round_Up)
{
  long Make_Int_Total_Area_in_Function_Round_Up = 0;
  float Total_Area_Fraction_Part_Round_Up = 0;
  Make_Int_Total_Area_in_Function_Round_Up = long(Total_Area_in_Function_Round_Up);
  Total_Area_Fraction_Part_Round_Up = Total_Area_in_Function_Round_Up - Make_Int_Total_Area_in_Function_Round_Up;
  if ((0.00 <= Total_Area_Fraction_Part_Round_Up) && (Total_Area_Fraction_Part_Round_Up <= 0.13))
  {
    Total_Area_in_Function_Round_Up = Total_Area_in_Function_Round_Up - Total_Area_Fraction_Part_Round_Up;
  }
  else if ((0.14 <= Total_Area_Fraction_Part_Round_Up) && (Total_Area_Fraction_Part_Round_Up <= .37))
  {
    Total_Area_in_Function_Round_Up = (Total_Area_in_Function_Round_Up - Total_Area_Fraction_Part_Round_Up) + 0.25;
  }
  else if ((.38 <= Total_Area_Fraction_Part_Round_Up) && (Total_Area_Fraction_Part_Round_Up <= .62))
  {
    Total_Area_in_Function_Round_Up = (Total_Area_in_Function_Round_Up - Total_Area_Fraction_Part_Round_Up) + 0.50;
  }
  else if ((.63 <= Total_Area_Fraction_Part_Round_Up) && (Total_Area_Fraction_Part_Round_Up <= .86))
  {
    Total_Area_in_Function_Round_Up = (Total_Area_in_Function_Round_Up - Total_Area_Fraction_Part_Round_Up) + 0.75;
  }
  else
  {
    Total_Area_in_Function_Round_Up = (Total_Area_in_Function_Round_Up - Total_Area_Fraction_Part_Round_Up) + 1;
  }
  return (Total_Area_in_Function_Round_Up);
}


//************************************************************************//
//***********************************************************************//

void Bundle_Mode_Selection()
{
  bool Work_Done = false;
  if (digitalRead(Bundle_Selection_Pin) == LOW)
  {
    Bundle_Selection_Pinstate = 1;
    Debounce_Delay = micros();
  }

  else if ((micros() - Debounce_Delay) > 10000 && Bundle_Selection_Pinstate == 1)
  {
    if (digitalRead(Debug_Pin) == HIGH)
    {
      Bundle_Selection_Pinstate = 0;
      Debounce_Delay = micros();
      Bundle_Selection_Countrer += 1;
      Work_Done = true;
#ifdef DEBUG
      Serial.println();
      Serial.print("Bundle_Selection_Countrer = ");
      Serial.println(Bundle_Selection_Countrer);
#endif
      if (Bundle_Selection_Countrer >= 3)
      {
        Bundle_Selection_Countrer = 1;
      }
    }
  }
  if ((Bundle_Selection_Countrer == 2) && (Work_Done == true))
  {
    Bundle_Mode_FLAG = false;
    Buzzer_ON_FLAG = false;
    Work_Done = false;
    digitalWrite(BuzzerPin, LOW);
    Bundle_Area = 0;
    Sheet_Running_Bundle = 0;
    Sheet_in_Bundle = 0;
    EEPROM.write(506, 255);
    EEPROM.commit();
    Area_Sheet_Count_Saved_EEPROM(Total_Area);
    Serial2.print("t6.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print("0"); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t7.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print("0"); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print("0.0"); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t2.pco=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t6.pco=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t10.pco=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t7.pco=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t8.pco=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t9.pco=0");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  if ((Bundle_Selection_Countrer == 1) && (Work_Done == true))
  {
    Sheet_Count_Bundle_Press = Sheet_Count;
    Serial2.print("t6.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print("0"); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t8.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.print("0.0"); // This is the value you want to send to that object and atribute mention before.
    Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t2.pco=63488");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t6.pco=63488");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t10.pco=63488");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t7.pco=63488");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t8.pco=63488");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t9.pco=63488");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Bundle_Sheet_Set_Mode();
    Work_Done = false;
    Serial2.print("t2.pco=65504");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t6.pco=65504");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t10.pco=65504");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t7.pco=65504");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t8.pco=65504");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t9.pco=65504");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
    Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  if ((Bundle_Selection_Countrer == 0) && (Work_Done == true))
  {
    Work_Done = false;
  }
}

void Bundle_Sheet_Set_Mode()
{
  bool EXIT = false;
  float Pre_Value_Plus_Minus_Bundle = 0;
  float Plus_Minus_Bundle = 0;
  delay(1000);
  while (1)
  {
    Pre_Value_Plus_Minus_Bundle = Plus_Minus_Bundle;
    Plus_Minus_Bundle = Up_Down_Button(Plus_Minus_Bundle);
    if (Plus_Minus_Bundle != Pre_Value_Plus_Minus_Bundle)
    {
      if (Plus_Minus_Bundle > Pre_Value_Plus_Minus_Bundle)
      {
        Sheet_in_Bundle += 1;
      }
      else
      {
        Sheet_in_Bundle -= 1;
      }
      if (Sheet_in_Bundle < 0)
      {
        Sheet_in_Bundle = 0;
        Pre_Value_Plus_Minus_Bundle = 0;
        Plus_Minus_Bundle = 0;
      }
      if (Sheet_in_Bundle > 99)
      {
        Sheet_in_Bundle = 99;
      }
      Serial2.print("t7.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Sheet_in_Bundle); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
    }
    if (digitalRead(Bundle_Selection_Pin) == LOW)
    {
      Bundle_Selection_Pinstate = 1;
      Debounce_Delay = millis();
    }
    if ((millis() - Debounce_Delay) > 50 && Bundle_Selection_Pinstate == 1)
    {
      if (digitalRead(Bundle_Selection_Pin) == HIGH)
      {
        Debounce_Delay = millis();
        Bundle_Mode_FLAG = true;
        EEPROM.write(506, 1);
        EEPROM.commit();
        if (Sheet_in_Bundle == 0)
        {
          EEPROM.write(506, 0);
          EEPROM.commit();
          Bundle_Mode_FLAG = false;
        }
        Plus_Minus_Bundle = 0;
        Pre_Value_Plus_Minus_Bundle = 0;
        Bundle_Selection_Pinstate = 0;
        Sheet_Running_Bundle = 0;
        Bundle_Area = 0.00;
        Area_Sheet_Count_Saved_EEPROM(Total_Area);

        break;
      }
    }
  }
}

void Tacho_Pulse_Check()
{
  unsigned long Time_Sub = millis();
  unsigned long Time_Dif = 0;
  unsigned long Time_Page_Refresh = millis();
  unsigned long Time_Text_Refresh = 0;
  bool Text_Refresh_FLAG = false;

  while (1)
  {

    if ((digitalRead(TachoPin) == LOW) && (Debug_Press_Counter == 1))
    {
      Time_Dif = millis() - Time_Sub;
      Time_Sub = millis();
      Serial.print("Time_Dif_bet_tacho:");
      Serial.println(Time_Dif);
      Serial.print(",");
      Serial2.print("t1.txt+=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(Time_Dif); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t1.txt+=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(","); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);



      if (millis() - Time_Page_Refresh >= 5000)
      {
        Serial2.print("t1.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.print(" "); // This is the value you want to send to that object and atribute mention before.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
        Time_Page_Refresh = millis();
      }
      Time_Text_Refresh = millis();
      Text_Refresh_FLAG = true;
      while (digitalRead(TachoPin) == LOW) {}
    }
    if (digitalRead(Debug_Pin) == LOW)
    {
      Debug_Pinstate = 1;
      Debounce_Delay = micros();
    }
    else if ((micros() - Debounce_Delay) > 10000 && Debug_Pinstate == 1)
    {
      if (digitalRead(Debug_Pin) == HIGH)
      {
        Debounce_Delay = micros();
        Debug_Pinstate = 0;
        Debug_Press_Counter += 1;
      }
    }
    if (Debug_Press_Counter == 2)
    {
      if (Text_Refresh_FLAG == true)
      {
        Text_Refresh_FLAG = false;
        Serial2.print("t0.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.print("Short Error Sensors"); // This is the value you want to send to that object and atribute mention before.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
        Serial2.print("t1.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.print(" "); // This is the value you want to send to that object and atribute mention before.
        Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
        Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
        Serial2.write(0xff);
        Serial2.write(0xff);
      }
      if ((millis() - Time_Text_Refresh) >= 500)
      {
        Time_Text_Refresh = millis();
        if (digitalRead(TachoPin) == LOW)
        {
          Control_Latch();
          for (i = 0; i < 80; )
          {
            if ((micros() - Time_Delay_Bit_Read) > (2 * Clock_width))
            {
              Time_Delay_Bit_Read = micros();
              digitalWrite(ClockPin, LOW);
              FLAG = 1;
            }
            if ( ((micros() - Time_Delay_Bit_Read) > Clock_width) && FLAG == 1)
            {
              One_Set_Data[i] = digitalRead(DataPin);
              digitalWrite(ClockPin, HIGH);

              Bit_Arrangement();
              if (One_Set_Data[i] == 0)
              {
                zero_RST += 1;
              }
              i++;

              FLAG = 0;
            }
          }

#ifdef DEBUG
          Serial.println();
          for (int SePr = 0; SePr < 80; SePr++)
          {
            Serial.print(One_Set_Data_For_Error[SePr]);
            if (SePr % 8 == 7)
            {
              Serial.print("  ");
            }
          }
          Serial.println();
#endif
          if (zero_RST == 80)
          {
            Serial2.print("t1.txt=");
            Serial2.print("\"");
            Serial2.print("There is no short Sensor");
            Serial2.print("\"");
            Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
            Serial2.write(0xff);
            Serial2.write(0xff);
            zero_RST = 0;
          }
          else
          {
            int SENSOR_NUMBER_ONE;
            String Broken_One;
            for (int z = 0; z < 80; z++)
            {
              if (One_Set_Data_For_Error[z] == 1)
              {
                SENSOR_NUMBER_ONE = z + 1;
                Broken_One = (String("") + Broken_One + SENSOR_NUMBER_ONE + ",");
              }
            }
            Serial2.print("t1.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
            Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
            Serial2.print(Broken_One);
            Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
            Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
            Serial2.write(0xff);
            Serial2.write(0xff);
            zero_RST = 0;
          }
          for (int IED = 0; IED < 80; IED++)
          {
            One_Set_Data[IED] = 0;
            One_Set_Data_For_Error[IED] = 0;
          }
          while (digitalRead(TachoPin) == LOW) {}
        }
      }
    }
    if (Debug_Press_Counter == 3)
    {
      Serial2.print("to.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print("Tacho_Pulse Check"); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("t1.txt=");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.print(" "); // This is the value you want to send to that object and atribute mention before.
      Serial2.print("\"");  // Since we are sending text we need to send double quotes before and after the actual text.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.print("page 1");  // This is sent to the nextion display to set what object name (before the dot) and what atribute (after the dot) are you going to change.
      Serial2.write(0xff);  // We always have to send this three lines after each command sent to the nextion display.
      Serial2.write(0xff);
      Serial2.write(0xff);
      break;
    }
  }
}
void When_Sheet_Count_9999()
{
  Sheet_Count = 0;
  Area = 0;
  Total_Area = 0;
  Previous_Area = 0;
  for (int cv = 0; cv < 506; cv++)
  {
    if (EEPROM.read(cv) == 'E')
    {
      if (EEPROM.read(cv + 1) == 'N')
      {
        if (EEPROM.read(cv + 2) == 'D')
        {
          if (EEPROM.read(cv + 3) == '~')
          {
            EEPROM.write(cv, 0);
            EEPROM.write((cv + 3), 0);
            EEPROM.commit();
          }
        }
      }
    }
  }
}
