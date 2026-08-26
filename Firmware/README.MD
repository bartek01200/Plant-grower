# ESP32 Plant Watering System 
The firmware controls the automated plant watering system using an ESP32.

It monitors soil moisture, temperature, humidity and ambient light, automatically controls a 5 V water pump, displays information on a 16×2 LCD and uploads system data to ThingSpeak.

## System operation

The ESP32 continuously reads:
- Soil moisture
- Temperature
- Humidity
- Ambient light

The soilmoisture reading is used to control the water pump.

Two thresholds are used:
Soil >2100 → Pump ON
Soil <1900 → Pump OFF
# Calculations

# Pump flow: 
200L/hour 
200,000 / 3600 = 55.56 ml/s

Pump runs for 5 sec 
5 x 55.56ml/s = 277.8 mL

This gets added to:

totalWaterDeliveredMl

dailyWaterDeliveredMl

# Water efficiency:
manualWasteMl = 2000.0

Water Efficiency = 100 × (1 - Automatic Water Used/Manual Water Reference)

Automatic system = 800 mL

Manual reference = 2000 mL

Efficiency = 100 × (1 - 800 / 2000) = 60%

# Daily average light:
During the first 12 hours:

lightTotal = lightTotal+every light reading

lightsamples = number of readings taken

Daily average light = lightTotal/ lightSamples

# Monthly average light:
monthlyLightSum = monthly completed days average 

average monthly light = monthlylightsum / monthlydays  //to see average

# Average daily water use:
monthlyWaterSum = daily water consumption
Average daily water = monthlyWatersum /monthlydays

# Monthly water efficiency:
Manual Reference = 2000 mL × Number of Days

Water efficiency = 100 × (1 - Monthly automatic water/(2000 × number of days))

EXAMPLE:
10 days

Manual reference =2000 × 10 = 20,000 mL

Automatic system used = 8,000 mL

Efficiency = 100 × (1 - 8000 / 20000) = 60%

# Light efficiency:
Light efficiency = (Avg monthly light / 2000)×100 //limited to 100%

# Combined efficiency:
Combined Efficiency = (0.6 × Light efficiency) + (0.4 × Water efficiency) //sunlight has more weight by 0.2 then water

# Growth estimate:
Combined >= 70%
=1.5 inches

Combined <= 40%
=1.0 inch

Combined < 40%
=0.5 inches

