# Project Name - Lora based Taffic Control System

## Project Members:
* Momshad Dinury
* Abdullah Zowad Khan
* Towqir Ahmed Shaem

## Shortcuts
* [**Controller Algorithm Overview**](#controller-algorithm-overview)
* [**Node Algorithm Overview**](#node-algorithm-overview)

## Controller Details

[**Controller.ino**](https://gitlab.com/Momshad/Lora-Traffic-Control-System/blob/Controller-Adjustments-Timers-Displays/Controller/Controller.ino)

### Interrupt Handlers

```cpp
void ISR_DB_1();
void ISR_DB_2();
void ISR_DB_3();
```

Display timers attached to:
```cpp
void Setting_Block_State_Color();
void showTime();
```

## Version 1 Configuration

* Display - TFT
* MCU - esp8266
* Communication - RF (LoRa Module)

## Version 2 Configuration

* Display - 4 Nokia 5110 Transflective Display
* MCU - ESP32
* Communication - Unchanged

### Controller Algorithm Overview
![Controller Algorithm](https://gitlab.com/Momshad/Lora-Traffic-Control-System/blob/Controller-Adjustments-Timers-Displays/Data%20Collection/Algorithm/Traffic%20Control.jpg)

### Node Algorithm Overview
![Node Algorithm](https://gitlab.com/Momshad/Lora-Traffic-Control-System/blob/Controller-Adjustments-Timers-Displays/Data%20Collection/Algorithm/Traffic%20Control%20Node.jpg)    
