# eratani-device
This repository is part of capstone project final deliverables for Bangkit! 2021 program.
## Introduction
Era tani project is a smart farming system that utilize real-time monitoring system, automation, data analytics, and machine learning prediction. Currently, era tani project only focusing on mushroom cultivation smart farming systme. This repository contains information about how to make the device for real-time monitoring system and automation.
## Materials
To build this device, we're going to use ESP8266 Development board, Nodemcu V3. But there are also some option to choose like Arduino Nano 33 BLE Sense, ESP32 Development board, Arduino Mega, and many more. beside microcontroller unit there also some material that we need to build the project. Here are the list of needed materials:
* DHT11/DHT22 Temperature and Humidity Sensor
* 5K Ohm Resistor
* 2x16 LCD Display with I2C Module
* 4-Channel Relay Module<br/>
 
For the automation part, we also going to need a final control element. In this project because we're going only to use 2 final control elements which are 12V Peltier, and 5V Humidifier. All of the materials can be found on Indonesia's online marketplace, Tokopedia.  
The wiring will be shown in the folder.
## Dependencies
The arduino sketch will have dependencies on some library depending on what board that is used. In this project ESP8266 is used so here is the list of library that we used in this project  
* Firebase ESP8266 Library
* DHT Master Library
* Adafruit Unified Sensor Library<br/>

For firebase library choose accordingly and please see the library documentation since some firebase microcontroller library is still in development and experimental.
