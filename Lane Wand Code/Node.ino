/*
  Project Name : Lora Traffic Control System
  Developers : Towqir Ahmed Shaem , Momshad Dinury
  Email : towqirahmedshaem@gmail.com, md.dinury@gmail.com
  Company : Stellar Technology, Bangladesh

*/

#include <SX1278.h>
#include <SPI.h>
#include <Ticker.h>

//LED:
#define LED 4
boolean LEDstate = true;
Ticker flipper;

//Lora SX1278:
#define LORA_MODE             1
#define LORA_CHANNEL          CH_6_BW_125
#define LORA_CHANNEL_2        CH_7_BW_125
#define LORA_ADDRESS          3
#define LORA_SEND_TO_ADDRESS  5

uint8_t ControllerAddress = 5;

boolean FreChCondition = true;


char my_packet[50];
char testData[50];//Transmits data to Gateway
int state;

//Pindef:
#define tb 2 //tb = trasnmit button
boolean tirgger = false;

void setup() {

  Serial.begin(115200);
  delay(10);
  Serial.println("Initialized..");
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);

  pinMode(LED, OUTPUT);
  pinMode(tb, INPUT);
  loraSetup();
  attachInterrupt(digitalPinToInterrupt(tb), ISR, FALLING);
}

void loop() {

  recieveData();

  if(tirgger){
      tirgger = false;
      Process();
  }


}

void Process() {
      delay(200);
      String("K").toCharArray(testData, 50);
      sendData(testData);

      if(state != 0) {
        Serial.println("Trying again..");
        sendData(testData);
      }

      if (state == 0){
        flipper.detach();
        digitalWrite(LED, HIGH);
        FreChCondition = true;
        loraFreqChChange();
      }
}

void ISR(){
  tirgger = true;
}

void Blinking() {

  if(LEDstate){
    digitalWrite(LED, HIGH);
    LEDstate = false;
  }else if(!LEDstate){
    digitalWrite(LED, LOW);
    LEDstate = true;
  }

}

int sendData(char message[]) {
  
  state = sx1278.sendPacketTimeoutACK(ControllerAddress, message);
  if (state == 0)
  {
    Serial.println(F("State = 0 --> Command Executed w no errors!"));
    Serial.println(F("Packet sent....."));

    return state;
  }
  else {
    Serial.println(state);
    Serial.println(F("Packet not sent....."));

    return state;
  }
}


void recieveData() {

  int  z = sx1278.receivePacketTimeoutACK();
  if (z == 0) {
    delay(10);
    Serial.println(F("Package received!"));

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
    Serial.print(F("Message:  "));
    setLedState();

    Serial.println(my_packet);

    FreChCondition = false;
    loraFreqChChange();
  }
}

void setLedState() {
  if (my_packet[0] == 'G') {
    digitalWrite(LED, HIGH);
    flipper.attach(0.5, Blinking);
  } else if (my_packet[0] == 'R') {
    digitalWrite(LED, LOW);
  }
}

void loraSetup() {
  // Power ON the module:
  if (sx1278.ON() == 0) {
    Serial.println(F("Setting power ON: SUCCESS "));
  } else {
    Serial.println(F("Setting power ON: ERROR "));
  }

  // Set transmission mode and print the result:
  if (sx1278.setMode(LORA_MODE) == 0) {
    Serial.println(F("Setting Mode: SUCCESS "));
  } else {
    Serial.println(F("Setting Mode: ERROR "));
  }

  // Set header:
  if (sx1278.setHeaderON() == 0) {
    Serial.println(F("Setting Header ON: SUCCESS "));
  } else {
    Serial.println(F("Setting Header ON: ERROR "));
  }

  // Select frequency channel:
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
    Serial.println(F("Setting Channel: SUCCESS "));
  } else {
    Serial.println(F("Setting Channel: ERROR "));
  }

  // Set CRC:
  if (sx1278.setCRC_ON() == 0) {
    Serial.println(F("Setting CRC ON: SUCCESS "));
  } else {
    Serial.println(F("Setting CRC ON: ERROR "));
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
    Serial.println(F("Setting Power: SUCCESS "));
  } else {
    Serial.println(F("Setting Power: ERROR "));
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
    Serial.println(F("Setting node address: SUCCESS "));
  } else {
    Serial.println(F("Setting node address: ERROR "));
  }

  // Print a success
  Serial.println(F("SX1278 configured finished"));
  Serial.println();


}

void loraFreqChChange () {

  if( FreChCondition ) {

    // Select frequency channel 6:
    if (sx1278.setChannel(LORA_CHANNEL) == 0) {
      Serial.println(F("Setting Channel: SUCCESS "));
    } else {
      Serial.println(F("Setting Channel: ERROR "));
    }

  } else if (!FreChCondition ) {

    // Select frequency channel 7:
    if (sx1278.setChannel(LORA_CHANNEL_2) == 0) {
      Serial.println(F("Setting Channel: SUCCESS "));
    } else {
      Serial.println(F("Setting Channel: ERROR "));
    }

  }

}




