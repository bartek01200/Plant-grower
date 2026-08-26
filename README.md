# ESP32 Automated Plant Watering System
An ESP32-based automated irrigation system that monitors soil moisture,
temperature and humidity, and activates a 5V water pump when the soil
becomes too dry, providing efficency as opposed to standard watering.

The system also displays live sensor values on a 16×2 LCD and logs data
remotely using ThingSpeak.The project uses a breadboard for all connectivity between components.

## Hardware
- ESP32
- Soil moisture sensor
- DHT22 temperature and humidity sensor
- 16×2 LCD
- Relay module
- 5V water pump
- Potentiometer
- 5V power supply
- Breadboard

The soil moisture sensor provides the main watering input to the ESP32.
When the measured moisture falls below the programmed threshold, the ESP32
activates the relay, switching power to the water pump.

The DHT22 provides temperature and humidity measurements, while the LCD
displays the current sensor readings locally.

## Software
The firmware was developed using the Arduino C++ for the ESP32.

Main libraries:
- DHT.h
- LiquidCrystal.h
- WiFi.h
- ThingSpeak.h

The firmware handles:
- soil moisture acquisition
- temperature and humidity sensing
- automatic pump control
- LCD updates
- Wi-Fi connectivity
- ThingSpeak data logging

