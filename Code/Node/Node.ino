//Library:
#include <Ticker.h>
#include <SX1278.h>

//Child Parameter:
//#define Child_1
//#define Child_2
#define Child_3
//#define Child_4

//
//Lora SX1278:
#define LORA_MODE             4             //mode: mode number to set the required BW, SF and CR of LoRa modem.
#define LORA_CHANNEL          CH_6_BW_125
uint8_t ControllerAddress = 5;              //Parent Address

#ifdef Child_1
#define LORA_ADDRESS          3
#endif
//
#ifdef Child_2
#define LORA_ADDRESS          4
#endif

#ifdef Child_3
#define LORA_ADDRESS          6
#endif

#ifdef Child_4
#define LORA_ADDRESS          7
#endif

//Message var:
char my_packet [50];  //Used to store Incoming message in char array from Parent Node
char testData[50];    //Used to store message which will be sent to Parent Node
String receivedMsg;   //to store the converted my_packet string from char array

//Pin Def:
int ButtonPIN = 2;
int RedLED = 5;
int GreenLED = 4;

//Flag:
boolean T_ISR_F = false;
int T_packet_state;
int R_packet_state;
boolean FunctionBlockingFlag = true;
boolean previousState = true;
boolean blinking;

//Debouncing timer
long debouncing_time = 3000;
volatile unsigned long last_micros;

// timer for sending data:
long ledBlinkingInterval = 350;
unsigned long last_interval = 0;
unsigned long current_millis;
unsigned long lastActivatedTime;
unsigned long presentActivatedTime;

//#define DEBUG

void setup() {
  //Serial communication begin:
#ifdef DEBUG
  Serial.begin(9600);
#endif
  //Lora init:
  loraSetup();
  //Pin config:
  pinConfiguration();
  //Interrupt:
  attachInterrupt(digitalPinToInterrupt(ButtonPIN), Trigger_ISR, FALLING);
#ifdef DEBUG
  Serial.print("Lora Address :");
  Serial.println(LORA_ADDRESS);
#endif
  //turnOff Led:
  ledOff();
  lastActivatedTime = millis();
  presentActivatedTime = millis();
}

void loop() {

  unsigned long startTime = millis();
  recieveData();
  unsigned long endtime = millis();
   #ifdef DEBUG
    Serial.print("Receive Data Processing Time : ");
    Serial.println(endtime - startTime);
   #endif

  Process();


  if (!FunctionBlockingFlag) {
    childTask();
  }
}

void childTask() {
#ifdef DEBUG
  Serial.println("*****Child Task****");
#endif
  //Sends ACK after changing LED
#ifdef Child_1
  if ( (receivedMsg.equals("GL1") || receivedMsg.equals("RL1")) /*&& T_ISR_F*/ && !FunctionBlockingFlag) {
    String("KL1").toCharArray(testData, 50);
    sendData(testData);
  }
#endif

#ifdef Child_2
  if ( (receivedMsg.equals("GL2") || receivedMsg.equals("RL2")) /*&& T_ISR_F*/ && !FunctionBlockingFlag) {
    String("KL2").toCharArray(testData, 50);
    sendData(testData);
  }
#endif

#ifdef Child_3
  if ( (receivedMsg.equals("GL3") || receivedMsg.equals("RL3")) /*&& T_ISR_F*/ && !FunctionBlockingFlag) {
    String("KL3").toCharArray(testData, 50);
    sendData(testData);
  }
#endif

#ifdef Child_4
  if ( (receivedMsg.equals("GL4") || receivedMsg.equals("RL4")) /*&& T_ISR_F*/ && !FunctionBlockingFlag) {
    String("KL4").toCharArray(testData, 50);
    sendData(testData);
  }
#endif
}

void Process() {                            //Controls LED per received message
  //For Child 1
  presentActivatedTime = millis();


  if ( presentActivatedTime - lastActivatedTime <= ledBlinkingInterval ) {
    blinking = previousState;
  }
  else {
    previousState = !previousState;
    lastActivatedTime = presentActivatedTime;

  }


  //For Child 21
#ifdef Child_1
  if (receivedMsg.equals("GL1") && blinking) {
    digitalWrite(RedLED, LOW); //To turn off RED LED
    digitalWrite(GreenLED, HIGH); //To turn on Green LED
#ifdef DEBUG
    Serial.println("Green Is On");
#endif
  }
  else if (receivedMsg.equals("GL1") && !blinking) {
    digitalWrite(RedLED, LOW); //To turn off RED LED
    digitalWrite(GreenLED, LOW); //To turn Off Green LED
#ifdef DEBUG
    Serial.println("Green Is OFF");
#endif
  }

  if (receivedMsg.equals("RL1") && blinking) {
    digitalWrite(RedLED, HIGH); //Turns on RED LED
    digitalWrite(GreenLED, LOW); //Turns off Green LED
#ifdef DEBUG
    Serial.println("RED Is On");
#endif
  }
  else if ( receivedMsg.equals("RL1") && !blinking) {
    digitalWrite(RedLED, LOW); //Turns on RED LED
    digitalWrite(GreenLED, LOW); //Turns off Green LED
#ifdef DEBUG
    Serial.println("RED Is OFF");
#endif

  }
#endif

  //For Child 2
#ifdef Child_2
  if (receivedMsg.equals("GL2") && blinking) {
    digitalWrite(RedLED, LOW); //To turn off RED LED
    digitalWrite(GreenLED, HIGH); //To turn on Green LED
#ifdef DEBUG
    Serial.println("Green Is On");
#endif
  }
  else if (receivedMsg.equals("GL2") && !blinking) {
    digitalWrite(RedLED, LOW); //To turn off RED LED
    digitalWrite(GreenLED, LOW); //To turn Off Green LED
#ifdef DEBUG
    Serial.println("Green Is OFF");
#endif
  }

  if (receivedMsg.equals("RL2") && blinking) {
    digitalWrite(RedLED, HIGH); //Turns on RED LED
    digitalWrite(GreenLED, LOW); //Turns off Green LED
#ifdef DEBUG
    Serial.println("RED Is On");
#endif
  }
  else if ( receivedMsg.equals("RL2") && !blinking) {
    digitalWrite(RedLED, LOW); //Turns on RED LED
    digitalWrite(GreenLED, LOW); //Turns off Green LED
#ifdef DEBUG
    Serial.println("RED Is OFF");
#endif

  }
#endif

  //For Child 3
#ifdef Child_3
  if (receivedMsg.equals("GL3") && blinking) {
    digitalWrite(RedLED, LOW); //To turn off RED LED
    digitalWrite(GreenLED, HIGH); //To turn on Green LED
#ifdef DEBUG
    Serial.println("Green Is On");
#endif
  }
  else if (receivedMsg.equals("GL3") && !blinking) {
    digitalWrite(RedLED, LOW); //To turn off RED LED
    digitalWrite(GreenLED, LOW); //To turn Off Green LED
#ifdef DEBUG
    Serial.println("Green Is OFF");
#endif
  }

  if (receivedMsg.equals("RL3") && blinking) {
    digitalWrite(RedLED, HIGH); //Turns on RED LED
    digitalWrite(GreenLED, LOW); //Turns off Green LED
#ifdef DEBUG
    Serial.println("RED Is On");
#endif
  }
  else if ( receivedMsg.equals("RL3") && !blinking) {
    digitalWrite(RedLED, LOW); //Turns on RED LED
    digitalWrite(GreenLED, LOW); //Turns off Green LED
#ifdef DEBUG
    Serial.println("RED Is OFF");
#endif

  }
#endif


  //For Child 4
#ifdef Child_4
  if (receivedMsg.equals("GL4") && blinking) {
    digitalWrite(RedLED, LOW); //To turn off RED LED
    digitalWrite(GreenLED, HIGH); //To turn on Green LED
#ifdef DEBUG
    Serial.println("Green Is On");
#endif
  }
  else if (receivedMsg.equals("GL4") && !blinking) {
    digitalWrite(RedLED, LOW); //To turn off RED LED
    digitalWrite(GreenLED, LOW); //To turn Off Green LED
#ifdef DEBUG
    Serial.println("Green Is OFF");
#endif
  }

  if (receivedMsg.equals("RL4") && blinking) {
    digitalWrite(RedLED, HIGH); //Turns on RED LED
    digitalWrite(GreenLED, LOW); //Turns off Green LED
#ifdef DEBUG
    Serial.println("RED Is On");
#endif
  }
  else if ( receivedMsg.equals("RL4") && !blinking) {
    digitalWrite(RedLED, LOW); //Turns on RED LED
    digitalWrite(GreenLED, LOW); //Turns off Green LED
#ifdef DEBUG
    Serial.println("RED Is OFF");
#endif

  }
#endif


  //This one should be broadcasted
  if (receivedMsg.equals("S")) {
    ESP.restart();
  }
}
void Trigger_ISR() {                        //ISR Trigger Function
  if ((long)(micros() - last_micros) >= debouncing_time * 1000) {
    T_ISR_F = true;
    last_micros = micros();
  } else {
#ifdef DEBUG
    Serial.println("Bounce");
#endif
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

void sendData(char message[]) {             //Global Send Data Function
  //delay(1000);
  T_packet_state = sx1278.sendPacketTimeoutACKRetries(ControllerAddress, message);
  Serial.print("Send Packet State:");
  Serial.println(T_packet_state);
  if (T_packet_state == 0) {
#ifdef DEBUG
    //Serial.println(F("State = 0 --> Command Executed w no errors!"));
    Serial.println(F("Confirmation Packet sent....."));
#endif
    T_ISR_F = false;
    FunctionBlockingFlag = true;
  }
  else if (T_packet_state == 5 || T_packet_state == 6 || T_packet_state == 7) {
    Serial.print("Conflict --> TRUE\t");
    FunctionBlockingFlag = true;
  }
}
void recieveData() {

  //Global Receive Data Function
  R_packet_state = sx1278.receivePacketTimeoutACK();
  if (R_packet_state == 0) {
#ifdef DEBUG
    Serial.println(F("Package received!"));
#endif

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
#ifdef DEBUG
    Serial.print(F("Message:  "));
    Serial.println(my_packet);
#endif
    receivedMsg = String(my_packet); //Converts CharArray to String
    //Process();
    FunctionBlockingFlag = false;
    //blinking = true;
  }
}

void loraSetup() {

  //Sets Lora Modes & returns 'true' if successful Else 'false'
  Serial.println("");
  // Power ON the module:
  (sx1278.ON() == 0) ? Serial.println(F("Setting power ON: SUCCESS ")) : Serial.println(F("Setting power ON: ERROR "));
  // Set transmission mode and print the result:
  (sx1278.setMode(LORA_MODE) == 0) ? Serial.println(F("Setting Mode: SUCCESS ")) : Serial.println(F("Setting power ON: ERROR "));
  // Set header:
  (sx1278.setHeaderON() == 0) ? Serial.println(F("Setting Header ON: SUCCESS ")) : Serial.println(F("Setting Header ON: ERROR "));
  // Select frequency channel:
  (sx1278.setChannel(LORA_CHANNEL) == 0) ? Serial.println(F("Setting Channel: SUCCESS ")) : Serial.println(F("Setting Channel: ERROR "));
  // Set CRC:
  (sx1278.setCRC_ON() == 0) ? Serial.println(F("Setting CRC ON: SUCCESS ")) : Serial.println(F("Setting CRC ON: ERROR "));
  // Select output power (Max, High, Intermediate or Low)
  (sx1278.setPower('M') == 0) ? Serial.println(F("Setting Power: SUCCESS ")) : Serial.println(F("Setting Power: ERROR "));
  // Set the node address and print the result
  (sx1278.setNodeAddress(LORA_ADDRESS) == 0) ? Serial.println(F("Setting node address: SUCCESS ")) : Serial.println(F("Setting node address: ERROR "));
  // Print a success
  Serial.println(F("SX1278 configured finished"));
  Serial.println();
}


void pinConfiguration() {                   //All  PIN config
  pinMode(RedLED, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(GreenLED, OUTPUT);
  digitalWrite(GreenLED, LOW);
  // pinMode(GreenLED, mode);
  digitalWrite(0, LOW);
  pinMode(ButtonPIN, INPUT);
}
void ledOff() {                             //To keep LED turned off during boot
  digitalWrite(RedLED, LOW);
  digitalWrite(GreenLED, LOW);
}
