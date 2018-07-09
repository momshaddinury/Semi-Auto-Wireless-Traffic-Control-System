#include <Source.h>

void setup() {
  Serial.begin(9600);
  //Display Setup:
  displaySetup();
  // Lora Initialization
  loraSetup();
  //PinMode:
  pinMode(interruptButton, INPUT);
  pinMode(digitalButton, INPUT);
  pinMode(analogButton, INPUT);
  //Interupt:
  attachInterrupt(digitalPinToInterrupt(interruptButton), B1_ISR_F, FALLING);
}

void loop() {
  //After the device is booted it automatically re-boots other device:
  sync();
  //Checks for the Interrupt if it occurs
  InteruptPinAction();
  digitalWrite(interruptButton, HIGH);
  //Checks if the digital button is low:
  digitalButtonAction();
  //Reads and monitors the analog pin:
  AB_value = analogRead(analogButton);
  analogButtonAction();
  //This function checks for data to receive
  recieveData();
  // delay(1000);
}

void sync() {
  if (resetCondition == true) {
    String("S").toCharArray(testData, 50);
    sendData(testData);
    delay(1000);

    sendDataFailSafe();
    if ( T_packet_state == 0 ) {
      resetStop = true;
    }
  }
  if (resetStop == true) {
    resetCondition = false;
    resetStop = false;
  }
}

void B1_ISR_F() {
  B_ISR_F_F = true;
}

void InteruptPinAction() {
  //For Button 1:
  if (B_ISR_F_F) {

    Serial.println("Push button 1 is pressed. ");
    Location = Location_1;

    if (button1State) {

      String("G").toCharArray(testData, 50);
      sendData(testData);
      delay(1000);
      //FailSafe:
      sendDataFailSafe();

      if (T_packet_state == 0) {
        blockStateColor = true;
        Location1.attach(1, Blink_Location_Rect);
      }
      if (T_packet_state == 0) {
        button1State = false;
        B_ISR_F_F = false;
      }
    }
    else if (!button1State) {

      String("R").toCharArray(testData, 50);
      sendData(testData);
      delay(1000);
      //FailSafe:
      sendDataFailSafe();

      if (T_packet_state == 0) {
        blockStateColor = false;
        Location1.attach(1, Blink_Location_Rect);
      }
      if (T_packet_state == 0) {
        button1State = true;
        B_ISR_F_F = false;
      }
    }
  }
}

void digitalButtonAction() {
  if(digitalRead(digitalButton) == LOW) {
    if((long(millis()) - DB_priv_time) >= interval) {
      Serial.println("Push Button 2 is Pressed!");
      Location = Location_2;
      if(button2State) {
        String("G").toCharArray(testData, 50);
        sendData(testData);
        delay(1000);
        //FailSafe:
        sendDataFailSafe();

        if (T_packet_state == 0) {
          blockStateColor = true;
          Location2.attach(1, Blink_Location_Rect);
        }
        if (T_packet_state == 0) {
          button2State = false;
        }
      } else if (!button2State) {

        String("R").toCharArray(testData, 50);
        sendData(testData);
        delay(1000);
        //FailSafe:
        sendDataFailSafe();

        if (T_packet_state == 0) {
          blockStateColor = false;
          Location2.attach(1, Blink_Location_Rect);
        }
        if (T_packet_state == 0) {
          button2State = true;
        }
      }
      DB_priv_time = millis();
    }
  }
}

void analogButtonAction() {
  //For Button 3:
  if( AB_value > 400) {
    if((long(millis()) - AB_priv_time_3) >= interval) {
      Serial.println("Button 3 is Pressed!");
      Location = Location_3;
      if(button3State) {
        String("G").toCharArray(testData, 50);
        sendData(testData);
        delay(1000);
        //FailSafe:
        sendDataFailSafe();

        if (T_packet_state == 0) {
          blockStateColor = true;
          Location3.attach(1, Blink_Location_Rect);
        }
        if (T_packet_state == 0) {
          button3State = false;
        }
      } else if (!button3State) {

        String("R").toCharArray(testData, 50);
        sendData(testData);
        delay(1000);
        //FailSafe:
        sendDataFailSafe();

        if (T_packet_state == 0) {
          blockStateColor = false;
          Location3.attach(1, Blink_Location_Rect);
        }
        if (T_packet_state == 0) {
          button3State = true;
        }
      }
      AB_priv_time_3 = millis();
    }
    //For button 4
  } else if (AB_value > 40 && AB_value <300) {
    if((long(millis()) - AB_priv_time_4) >= interval) {
      Serial.println("Button 4 is Pressed");
      Location = Location_4;
      if(button4State) {
        String("G").toCharArray(testData, 50);
        sendData(testData);
        delay(1000);
        //FailSafe:
        sendDataFailSafe();

        if (T_packet_state == 0) {
          blockStateColor = true;
          Location4.attach(1, Blink_Location_Rect);
        }
        if (T_packet_state == 0) {
          button4State = false;
        }
      } else if (!button4State) {

        String("R").toCharArray(testData, 50);
        sendData(testData);
        delay(1000);
        //FailSafe:
        sendDataFailSafe();

        if (T_packet_state == 0) {
          blockStateColor = false;
          Location4.attach(1, Blink_Location_Rect);
        }
        if (T_packet_state == 0) {
          button4State = true;
        }
      }
      AB_priv_time_4 = millis();
    }
  }
}

int sendData(char message[]) {
  T_packet_state = sx1278.sendPacketTimeoutACK(NodeAddress, message);
  if (T_packet_state == 0)
  {
    Serial.println(F("State = 0 --> Command Executed w no errors!"));
    Serial.println(F("Packet sent....."));

    return T_packet_state;
  }
  else {
    Serial.print(F("Error Code: "));
    Serial.println(T_packet_state);
    Serial.println(F("Packet not sent....."));

    return T_packet_state;
  }
}

int sendDataFailSafe() {
  if ( T_packet_state != 0) {
    sendData(testData);
    delay(1000);
    if ( T_packet_state != 0) {
      loraSetupFT();
      delay(10);
      sendData(testData);
      delay(1000);
    }
  }
}

int recieveData() {
  R_packet_state = sx1278.receivePacketTimeoutACK();
  if (R_packet_state == 0) {
    delay(10);
    Serial.println(F("Package received!"));

    for (unsigned int i = 0; i < sx1278.packet_received.length; i++) {
      my_packet[i] = (char)sx1278.packet_received.data[i];
      yield();
    }
    Serial.print(F("Message:  "));
    Serial.println(my_packet);

    Setting_Block_State_Color();
    return R_packet_state;
  }
}

void Blink_Location_Rect() {
  //This is for the first Location
  if (Location == Location_1) {

    if (blockStateColor) {

      if (locationBlock_1) {

        tft.fillRect(rect1x, rect1y, recwidth, recheight, GREEN);
        tft.setCursor(60, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("GEC");

        locationBlock_1 = false;

      } else if (!locationBlock_1) {
        tft.fillRect(rect1x, rect1y, recwidth, recheight, BLACK);
        tft.setCursor(60, 40);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("GEC");

        locationBlock_1 = true;

      }
    } else if (!blockStateColor) {

      if (locationBlock_1) {

        tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
        tft.setCursor(60, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("GEC");

        locationBlock_1 = false;

      } else if (!locationBlock_1) {

        tft.fillRect(rect1x, rect1y, recwidth, recheight, BLACK);
        tft.setCursor(60, 40);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("GEC");

        locationBlock_1 = true;
      }
    }
  }
  //This is for the second Location
  else if (Location == Location_2) {

    if (blockStateColor) {

      if (locationBlock_2) {

        tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
        tft.setCursor(60, 65);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("BAIZID");

        locationBlock_2 = false;

      } else if (!locationBlock_2) {
        tft.fillRect(rect1x, rect2y, recwidth, recheight, BLACK);
        tft.setCursor(60, 65);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("BAIZID");

        locationBlock_2 = true;

      }
    } else if (!blockStateColor) {

      if (locationBlock_2) {

        tft.fillRect(rect1x, rect2y, recwidth, recheight, RED);
        tft.setCursor(60, 65);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("BAIZID");

        locationBlock_2 = false;

      } else if (!locationBlock_2) {

        tft.fillRect(rect1x, rect2y, recwidth, recheight, BLACK);
        tft.setCursor(60, 65);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("BAIZID");

        locationBlock_2 = true;
      }
    }
  }
  //This is for the third Location
  else if (Location == Location_3) {

    if (blockStateColor) {

      if (locationBlock_3) {

        tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
        tft.setCursor(60, 85);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("MURADPUR");

        locationBlock_3 = false;

      } else if (!locationBlock_3) {
        tft.fillRect(rect1x, rect3y, recwidth, recheight, BLACK);
        tft.setCursor(60, 85);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("MURADPUR");

        locationBlock_3 = true;

      }
    } else if (!blockStateColor) {

      if (locationBlock_3) {

        tft.fillRect(rect1x, rect3y, recwidth, recheight, RED);
        tft.setCursor(60, 85);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("MURADPUR");

        locationBlock_3 = false;

      } else if (!locationBlock_3) {

        tft.fillRect(rect1x, rect3y, recwidth, recheight, BLACK);
        tft.setCursor(60, 85);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("MURADPUR");

        locationBlock_3 = true;
      }
    }
  }
  //This is for the fourth Location
  else if (Location == Location_4) {

    if (blockStateColor) {

      if (locationBlock_4) {

        tft.fillRect(rect1x, rect4y, recwidth, recheight, GREEN);
        tft.setCursor(60, 107);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("PROBARTAK");

        locationBlock_4 = false;

      } else if (!locationBlock_4) {
        tft.fillRect(rect1x, rect4y, recwidth, recheight, BLACK);
        tft.setCursor(60, 107);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("PROBARTAK");

        locationBlock_4 = true;

      }
    } else if (!blockStateColor) {

      if (locationBlock_4) {

        tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);
        tft.setCursor(60, 107);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("PROBARTAK");

        locationBlock_4 = false;

      } else if (!locationBlock_4) {

        tft.fillRect(rect1x, rect4y, recwidth, recheight, BLACK);
        tft.setCursor(60, 107);
        tft.setTextColor(WHITE);
        tft.setTextSize(1);
        tft.print("PROBARTAK");

        locationBlock_4 = true;
      }
    }
  }
}

void Setting_Block_State_Color() {

  if(Location == Location_1 ) {
    if (my_packet[0] == 'K') {
      if (blockStateColor) {
        Location1.detach();
        tft.fillRect(rect1x, rect1y, recwidth, recheight, GREEN);
        tft.setCursor(60, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("GEC");
      } else if (!blockStateColor) {
        Location1.detach();
        tft.fillRect(rect1x, rect1y, recwidth, recheight, RED);
        tft.setCursor(60, 40);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("GEC");
      }
    }
  } 
  else if (Location == Location_2) {
    if (my_packet[0] == 'K') {
      if (blockStateColor) {
        Location2.detach();
        tft.fillRect(rect1x, rect2y, recwidth, recheight, GREEN);
        tft.setCursor(60, 65);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("BAIZID");
      } else if (!blockStateColor) {
        Location2.detach();
        tft.fillRect(rect1x, rect2y, recwidth, recheight, RED);
        tft.setCursor(60, 65);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("BAIZID");
      }
    }
  }
  else if (Location == Location_3) {
    if (my_packet[0] == 'K') {
      if (blockStateColor) {
        Location3.detach();
        tft.fillRect(rect1x, rect3y, recwidth, recheight, GREEN);
        tft.setCursor(60, 85);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("MURADPUR");
      } else if (!blockStateColor) {
        Location3.detach();
        tft.fillRect(rect1x, rect3y, recwidth, recheight, RED);
        tft.setCursor(60, 85);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("MURADPUR");
      }
    }
  }
  else if (Location == Location_4) {
    if (my_packet[0] == 'K') {
      if (blockStateColor) {
        Location4.detach();
        tft.fillRect(rect1x, rect4y, recwidth, recheight, GREEN);
        tft.setCursor(60, 107);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("PROBARTAK");
      } else if (!blockStateColor) {
        Location4.detach();
        tft.fillRect(rect1x, rect4y, recwidth, recheight, RED);
        tft.setCursor(60, 107);
        tft.setTextColor(BLACK);
        tft.setTextSize(1);
        tft.print("PROBARTAK");
      }
    }
  }
}

void loraSetup() {
  Serial.println("");
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

void loraSetupFT () {
  // Power ON the module:
  if (sx1278.ON() == 0) {
    // Serial.println(F("Setting power ON: SUCCESS "));
  } else {
    // Serial.println(F("Setting power ON: ERROR "));
  }

  // Set transmission mode and print the result:
  if (sx1278.setMode(LORA_MODE) == 0) {
    // Serial.println(F("Setting Mode: SUCCESS "));
  } else {
    // Serial.println(F("Setting Mode: ERROR "));
  }

  // Set header:
  if (sx1278.setHeaderON() == 0) {
    // Serial.println(F("Setting Header ON: SUCCESS "));
  } else {
    // Serial.println(F("Setting Header ON: ERROR "));
  }

  // Select frequency channel:
  if (sx1278.setChannel(LORA_CHANNEL) == 0) {
    // Serial.println(F("Setting Channel: SUCCESS "));
  } else {
    // Serial.println(F("Setting Channel: ERROR "));
  }

  // Set CRC:
  if (sx1278.setCRC_ON() == 0) {
    // Serial.println(F("Setting CRC ON: SUCCESS "));
  } else {
    // Serial.println(F("Setting CRC ON: ERROR "));
  }

  // Select output power (Max, High, Intermediate or Low)
  if (sx1278.setPower('M') == 0) {
    // Serial.println(F("Setting Power: SUCCESS "));
  } else {
    // Serial.println(F("Setting Power: ERROR "));
  }

  // Set the node address and print the result
  if (sx1278.setNodeAddress(LORA_ADDRESS) == 0) {
    // Serial.println(F("Setting node address: SUCCESS "));
  } else {
    // Serial.println(F("Setting node address: ERROR "));
  }

  // Print a success
  Serial.println(F("SX1278 RE-CONFIGURED FINISHED"));
  Serial.println();
}

void displaySetup() {

  // Diaplay Initialization and Creating BLock
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3);

  tft.fillScreen(ST7735_CYAN);
  tft.setTextSize(1);
  tft.setTextColor(BLACK);
  tft.setCursor(10, 10);
  tft.println("TRAFFIC CONTROL SYSTEM");
  delay(1000);


  // Black Rect
  //For Location 1:
  tft.drawRect(rect1x - 1 , rect1y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect1y, recwidth, recheight, BLACK);
  //For Location 2:
  tft.drawRect(rect1x - 1 , rect2y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect2y, recwidth, recheight, BLACK);
  //For Location 3:
  tft.drawRect(rect1x - 1 , rect3y - 1, recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect3y, recwidth, recheight, BLACK);
  //For Location 4:
  tft.drawRect(rect1x - 1 , rect4y - 1 , recwidth + 2, recheight, BLACK);
  tft.drawRect(rect1x, rect4y, recwidth, recheight, BLACK);

  // Full Fill RECT
  // Location 1
  tft.fillRect(rect1x, rect1y, recwidth, recheight, YELLOW);
  // Location 2
  tft.fillRect(rect1x, rect2y, recwidth, recheight, YELLOW);
  // Location 3
  tft.fillRect(rect1x, rect3y, recwidth, recheight, YELLOW);
  // Location 4
  tft.fillRect(rect1x, rect4y, recwidth, recheight, YELLOW);

  //Location Names in display (Max 4 location)
  //Location name can be  changed according to location preference.
  //Location -1: GEC
  tft.setCursor(60, 40);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("GEC");

  //Location -2 :Baizid
  tft.setCursor(60, 65);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("BAIZID");

  //Location -3 :Muradpur
  tft.setCursor(60, 85);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("MURADPUR");

  //Location -4 :PROBARTAK
  tft.setCursor(60, 107);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("PROBARTAK");
}