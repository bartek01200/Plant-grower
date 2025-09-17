NAME:

Plant watering system      

OVERVIEW:

The enitre system is built around the ESP32 microcontroller, which cooridnates 
all sensors and outputs.

A capactivie soil moisture sensor determines whether the plant needs watering 
by continuously measuring the moisture level in the soil.

A DHT22 sensor provides temperature and humidity readings, giving the user insight
into the surroundings enviromental conditions that can influence the plants water needs.

A 16x2 LCD screen displays real time temperature,humidity and soil mositre data.

A variable resisor or potentiometer lets the user adujust the LCD backlight brightness for 
comfortable viewing.

When soil is dry the ESP32 activates a relay module which switches power to a 5v water pump.
The pump draws water from a reservoir and delivers it through a silicone tube directly to the plants
ensuring automated and effiecent irrigation.

FEATURES:

-Soil moisture sensing with ESP32/Arduino
-LCD display for real time values
-Relay controlled water pump
-Temperature and humiduty logging 

HARDWARE:

-Microcontroller: ESP32
-Sensors: DHT22, capacitive soil moisture sensor
-Actuators 5V relay module + 5V water pump
-Power: 5V regulated supply
-Display: 16x2 LCD with variable resistor

SOFTWARE:

Arduino IDE 1.x
LIbraries : DHT.h, LiquidCrystal.h


