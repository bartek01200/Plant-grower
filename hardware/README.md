# Hardware
The plant watering system was developed as a breadboard based prototype using an ESP32 as the main controller.

The hardware combines environmental sensing, automatic pump control, local LCD feedback and Wi-Fi connectivity for remote monitoring.

## Main Components
- ESP32 development board
- soil moisture sensor
- DHT22 temperature and humidity sensor
- LDR light sensor
- 16×2 LCD
- Relay module
- 5V water pump
- Potentiometer
- Breadboard
- wires
- External 5V supply
- Silicone tubing and water reservoir (water in a container)


### Soil moisture sensor
The  soil moisture sensor is connected to an ESP32 analogue input.

The sensor provides a voltage that changes depending on the moisture level of the soil. The ESP32 reads this using its ADC and uses the result to determine when watering is required.

ESP32 -> soil moisture(if low moisture) -> Relay -> 5V water pump -> water reservoir -> plant

## Why use a bread board:
The system was assembled on a breadboard to allow the sensors, relay, LCD and ESP32 connections to be easily tested and modified during development.

Using a breadboard made it possible to:
- Prototype the circuit quickly
- Test individual sensors
- Debug wiring
- Modify component connections
- Verify the control system before creating a more permanent implementation
- Keep costs low compared with designing and manufacturing a custom PCB
