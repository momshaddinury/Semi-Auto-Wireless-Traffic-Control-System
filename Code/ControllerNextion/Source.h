#ifndef Source_h
#define Source_h



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
