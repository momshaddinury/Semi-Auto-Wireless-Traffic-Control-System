//Including Library:
#include <SX1278.h>
#include <SPI.h>

//Lora SX1278:
#define LORA_MODE             10
#define LORA_CHANNEL          CH_6_BW_125
#define LORA_ADDRESS          4
#define LORA_SEND_TO_ADDRESS  2

char my_packet [50];
char testData[50]; //Transmits data to Gateway

//Pin def
#define pushbutton A0
//DEBUG
boolean debug_1  = true;
boolean debug_2  = true;
boolean debug_3  = true;
boolean debug_4  = true;

int value;

void setup() {
  Serial.begin(115200);
  loraSetup();
  pinMode(pushbutton, INPUT);
}

void loop() {
  value = analogRead(pushbutton);
  //Serial.println(value);
  pushButton();
}


void pushButton() {
  if (value > 350 && value < 410 && debug_1 == true) {
    Serial.println("Push button 1 is pressed");

    String("Push button 1 is pressed").toCharArray(testData, 50);
    

    debug_1 = false;
    debug_2 = true;
    debug_3 = true;
    debug_4 = true;
    sendData(testData);
  } else if (value > 230 && value < 280 && debug_2 == true) {
    Serial.println("Push button 2 is pressed");

    String("Push button 2 is pressed").toCharArray(testData, 50);
    

    debug_1 = true;
    debug_2 = false;
    debug_3 = true;
    debug_4 = true;
    sendData(testData);
  } else if (value > 120 && value < 180 && debug_3 == true) {
    Serial.println("Push button 3 is pressed");

    String("Push button 3 is pressed").toCharArray(testData, 50);
    

    debug_1 = true;
    debug_2 = true;
    debug_3 = false;
    debug_4 = true;
    sendData(testData);
  } else if (value > 70 && value < 100 && debug_4 == true) {
    Serial.println("Push button 4 is pressed");

    String("Push button 4 is pressed").toCharArray(testData, 50);
    

    debug_1 = true;
    debug_2 = true;
    debug_3 = true;
    debug_4 = false;
    sendData(testData);
  }
}

void sendData(char message[]) {
  if (!sx1278.sendPacketTimeout(0, message))
  {
    Serial.println(F("Packet sent....."));
  }
  else {
    Serial.println(F("Packet not sent....."));
  }
}

void recieveData() {

  int  z = sx1278.receivePacketTimeoutACK(5000);
  if (z == 0) {
    delay(10);
    Serial.println(F("Package received!"));

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
    Serial.print(F("Message:  "));
    Serial.println(my_packet);
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

