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
- Potentiometer 10kΩ
- Breadboard
- wires
- 120Ω resistor for LDR sensor
- External 5V supply
- Silicone tubing and water reservoir (water in a container)
- 1 additonal wire for pump power
- Push Button

## High-Current Relay & Pump Wiring
The ESP32 GPIO pins operate at 3.3V and can only safely output up to 12mA, whereas the 5V water pump requires significantly higher current to operate. Connecting the pump directly to the ESP32 would burn out the microcontroller.

To solve this, the pump's power circuit is physically isolated using the relay module:

Wire Preparation: The positive (red) and negative (black) leads from the 5V power supply and pump were cut and stripped.

Relay Wiring: 
The black ground wire provides a common ground, while the red power feed is broken across the relays COM (Common) and NO (Normally Open) terminals.

Operation: 
The relay acts as an electronically controlled inline switch. When the ESP32 sends a trigger signal to the relay, internal electromagnets snap the switch closed across COM and NO, completing the 5V power loop directly from the external power supply to the pump.


### LDR and Voltage Divider Resistor
The ESP32 can only read voltage, but the LDR changes its electrical resistance when light hits it. Pairing the LDR with a resistor turns those resistance changes into a varying voltage signal (0V to 3.3V) that the ESP32 can actually measure.

### Soil moisture sensor
The  soil moisture sensor is connected to an ESP32 analogue input.

The sensor provides a voltage that changes depending on the moisture level of the soil. The ESP32 reads this using its ADC and uses the result to determine when watering is required.

ESP32 -> soil moisture(if low moisture) -> Relay -> 5V water pump -> water reservoir -> plant
## Push button
Use of the push button allowd me to switch screens on lCD screen making it for functional

### Potentiometer
Used to change contrast of LCD screen help user see better

## Why use a bread board:
The system was assembled on a breadboard to allow the sensors, relay, LCD and ESP32 connections to be easily tested and modified during development.

Using a breadboard made it possible to:
- Prototype the circuit quickly
- Test individual sensors
- Debug wiring
- Modify component connections
- Verify the control system before creating a more permanent implementation
- Keep costs low compared with designing and manufacturing a custom PCB
