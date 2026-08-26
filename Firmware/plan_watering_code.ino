#include "DHT.h" //DHT22 library for it to monitor humidity
#include "LiquidCrystal.h"  //lCD screen library 
#include "WiFi.h"  //Wifi library connectivity
#include "ThingSpeak.h" //Thimgs speak connectivity with the code

#define DHTPIN 13 // DHT22 signal pin
#define DHTTYPE DHT22 // DHT type
#define SOIL_PIN 34// Soil moisture sensor (analog input)
#define RELAY_PIN 12 // Relay control pin
#define BUTTON_PIN 4//Button pin 
#define LDR_PIN 35//LDR pin 


// LCD Pins: LiquidCrystal(rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(5, 18, 15, 2, 19, 21);

DHT dht(DHTPIN, DHTTYPE);   
const char* WIFI_SSID = "wifi";     //This code is needed in order for the code to connect to the wifi to then connect to thingSpeak server
const char* WIFI_PASSWORD = "password";

unsigned long THINGSPEAK_CHANNEL_ID = 3079658;
const char* THINGSPEAK_API_KEY = "api";    // this is a key to the ThingSpeak server to allow the code to write data to the graphs

WiFiClient client;
unsigned long lastSendTime = 0;
const long updateInterval = 60000;  //The intervel which the code sends to ThingSpeak to update graph=

int screenMode = 0; //Screen is intially at 0
const int lightThreshold = 1500; //The lightThreshold to determine good sunlight the more light the lower resistance which gives a lower number giving us insight into the efficiency 

unsigned long dayStartMs   = 0;
const unsigned long HALF_DAY = 12UL * 60UL * 60UL * 1000UL; // 12 hours in ms  In order for me to calculate i had to use unasigned long this is 32bits which allows for a broader range of numbers as 16bit isnt big enough / regular numbers as its in milliseconds this will give a big number
const unsigned long FULL_DAY = 24UL * 60UL * 60UL * 1000UL; // 24 hours in ms

float lightTotal   = 0;    // sum of LDR readings
unsigned long lightSamples = 0;
bool dayComplete   = false;
String bloomStatus = "Good bloom";  // holds current message

const float PUMP_FLOW_ML_PER_SEC = 55.56; // pump rate per second in  mL using Ml instead of Literes due to the fact that its 200l/h so if we did it per second it would be a very small number being harder to deal with
unsigned long pumpOnStart   = 0;
const int SOIL_DRY_THRESHOLD = 2100;
const int SOIL_WET_THRESHOLD = 1900;
bool pumpRunning = false;
const unsigned long MAX_PUMP_TIME = 15000;
float totalWaterDeliveredMl = 0.0;
const float manualWasteMl   = 2000.0; // assume 2L wasted manually (on average using data)

unsigned long monthStartMs = 0; // these variables are placed in the global for effieceny as it wont need to be read over again in the loop 
float monthlyLightSum = 0;
float monthlyWaterSum = 0;
unsigned long monthlyDays = 0;
bool monthComplete = false;
float expectedGrowthInches = 0.0;

float dailyAverageLight = 0;
float dailyWaterDeliveredMl = 0;

const unsigned long MONTH_DURATION = 30UL * 24UL * 60UL * 60UL * 1000UL; //this is the monthly time so that we can give updates on plant growth

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.begin(16, 2); // 16 columns, 2 rows

  pinMode(RELAY_PIN, OUTPUT);   //setting relay as an output to determine pump being on and off
  digitalWrite(RELAY_PIN, LOW);  // Start with pump OFF

  pinMode(BUTTON_PIN, INPUT_PULLUP);//this is the button and assigning it the fact that it will be a physical input from the user

  Serial.print("Connecting to WiFi...");  //this tests network connection if it doesnt connect it wil give a . meanining the password or ssid of the network is incorrect
  lcd.clear();  //if it connects we_connected will be true as it has reached an ip for it to use to communicate with ThingSpeak
  lcd.print("Connecting WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  
  }
  ThingSpeak.begin(client);
  Serial.println("\nConnected to WiFi!");
  lcd.clear();
  lcd.print("WiFi Connected");
  delay(2000);

  lcd.print("System Starting...");
  delay(2000);
  lcd.clear();
  dayStartMs = millis(); //this will be used to know the periods such as if it has been 12 hours by reading the board time of use of code this is in milliseconds 
  monthStartMs = millis();
  }
void loop(){
  float temperature = dht.readTemperature();//these variables will the read the numbers recieved from their respective components this is in a dht as they both are readings form the same component the DHT22
  float humidity = dht.readHumidity();
  int soilValue = analogRead(SOIL_PIN);
  
  // Average LDR reading over 10 samples
  int lightValue = 0;
  for (int i = 0; i < 10; i++) {
    lightValue += analogRead(LDR_PIN);
    delay(5);
  }
  lightValue /= 10;
  
  // Handle button press for screen switching
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // debounce
    while (digitalRead(BUTTON_PIN) == LOW); // wait for release
    screenMode = (screenMode + 1) % 4; // Changed to 4 screens
  }
  
  // Daily light analysis for bloom prediction
  unsigned long now = millis();
  unsigned long elapsed = now - dayStartMs;
  
  if (elapsed < HALF_DAY) {
    // First 12 hours - accumulate light data
    lightTotal += lightValue;
    lightSamples++;
  } else if (!dayComplete) {
    // At 12-hour mark - calculate daily average
    dailyAverageLight = (lightSamples > 0) ? lightTotal / lightSamples : 0;
    if (dailyAverageLight < lightThreshold) {
      bloomStatus = "Good bloom";
    } else {
      bloomStatus = "Poor bloom";
    }
    dayComplete = true;
    } else if (elapsed >= FULL_DAY) {

      monthlyLightSum +=dailyAverageLight;
      monthlyWaterSum +=dailyWaterDeliveredMl;
      monthlyDays++;
      dayStartMs = now;
      lightTotal =0;
      lightSamples =0;
      dailyAverageLight= 0;
      dailyWaterDeliveredMl= 0;

      dayComplete = false;
      bloomStatus = "Analyzing...";
  }
  unsigned long monthElapsed = now - monthStartMs;
  if (monthElapsed >= MONTH_DURATION && !monthComplete) {
    float avgMonthlyLight = (monthlyDays > 0) ? monthlyLightSum / monthlyDays : 0;
    float avgDailyWater = (monthlyDays > 0) ? monthlyWaterSum / monthlyDays : 0;
    float lightEfficiency = max(0.0, min(100.0,
    (1.0 - (avgMonthlyLight / 2000.0)) * 100.0));

   float waterEfficiency = 0;

  if (monthlyDays > 0) {
    waterEfficiency =
    max(0.0, 100.0 * (1.0 -
    (monthlyWaterSum / (manualWasteMl * monthlyDays))));
  }
    float combinedEfficiency =
      (lightEfficiency * 0.6) +
      (waterEfficiency * 0.4);
    if (combinedEfficiency >= 70) {
      expectedGrowthInches = 1.5;
    }
    else if (combinedEfficiency >= 40) {
      expectedGrowthInches = 1.0;
    }
    else {
      expectedGrowthInches = 0.5;
    }

    monthComplete = true;
  }

  // Turn pump ON when soil is dry
  if (!pumpRunning && soilValue > SOIL_DRY_THRESHOLD) {

    digitalWrite(RELAY_PIN, HIGH);
    pumpRunning = true;
    pumpOnStart = millis();
    Serial.println("Pump ON - Watering started");
  }
  else if (pumpRunning && soilValue < SOIL_WET_THRESHOLD) {
    digitalWrite(RELAY_PIN, LOW);
    unsigned long runTimeMs = millis() - pumpOnStart;
    float waterUsed = (runTimeMs / 1000.0) * PUMP_FLOW_ML_PER_SEC;

    totalWaterDeliveredMl += waterUsed;
    dailyWaterDeliveredMl += waterUsed;
    pumpRunning = false;
    pumpOnStart = 0;
    Serial.print("Pump OFF - Water used: ");
    Serial.print(waterUsed);
    Serial.println(" ml");
  }                                                         
  if (pumpRunning && millis() - pumpOnStart >= MAX_PUMP_TIME) {//safety timeout stops pump after 15 seconds
    digitalWrite(RELAY_PIN, LOW);

    unsigned long runTimeMs = millis() - pumpOnStart;
    float waterUsed =(runTimeMs / 1000.0) * PUMP_FLOW_ML_PER_SEC;
    totalWaterDeliveredMl += waterUsed;
    dailyWaterDeliveredMl += waterUsed;

    pumpRunning = false;
    pumpOnStart = 0;
    Serial.println("Pump stopped - safety timeout");
  }
  float efficiency = 0;  //calculate water efficiency
  if (manualWasteMl > 0) {
    efficiency =
      max(0.0, 100.0 *
      (1.0 - (totalWaterDeliveredMl / manualWasteMl)));
  }
  //display information based on screen mode
  lcd.clear();
  switch (screenMode) {
    case 0: //environmental data
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(temperature, 1);
      lcd.print("C H:");
      lcd.print(humidity, 0);
      lcd.print("%");
      
      lcd.setCursor(0, 1);
      lcd.print("Soil:");
      lcd.print(soilValue);
      lcd.print(" L:");
      lcd.print(lightValue);
      break;
      
    case 1: // Bloom prediction
      lcd.setCursor(0, 0);
      lcd.print("Light Analysis");
      
      lcd.setCursor(0, 1);
      lcd.print(bloomStatus);
      break;
    case 2: // Water efficiency
      lcd.setCursor(0, 0);
      lcd.print("Used:");
      lcd.print(totalWaterDeliveredMl, 0);
      lcd.print("ml");
      
      lcd.setCursor(0, 1);
      lcd.print("Efficiency:");
      lcd.print(efficiency, 1);
      lcd.print("%");
      break;
      
    case 3: // Monthly growth prediction
      lcd.setCursor(0, 0);
      lcd.print("Monthly Growth");
      lcd.setCursor(0, 1);

      if (monthComplete) {
        lcd.print("Expected:");
        lcd.print(expectedGrowthInches, 1);
        lcd.print("in");
      } else {
        lcd.print("Analyzing...");
      }

    break;
  }
  // Upload to ThingSpeak every minute
  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= updateInterval) {
    lastSendTime = currentTime;
    
    // Check for valid sensor readings
    if (!isnan(temperature) && !isnan(humidity)) {
      ThingSpeak.setField(1, temperature);
      ThingSpeak.setField(2, humidity);
      //ThingSpeak.setField(3, soilValue); Not needed but kept if wanting to see outcomes
      ThingSpeak.setField(4, efficiency);
      ThingSpeak.setField(5, lightValue);
      ThingSpeak.setField(6, totalWaterDeliveredMl);
      ThingSpeak.setField(7, expectedGrowthInches);
      int result = ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
    
      if (result == 200) {
        Serial.println("Data successfully sent to ThingSpeak");
      } else {
        Serial.print("ThingSpeak error: ");
        Serial.println(result);
      }
      
      // Debug output
      Serial.print("Temp: "); Serial.print(temperature);
      Serial.print("°C|Humidity: "); Serial.print(humidity);
      Serial.print("% |Soil: "); Serial.print(soilValue);
      Serial.print("|Light: "); Serial.print(lightValue);
      Serial.print("|Water used: "); Serial.print(totalWaterDeliveredMl);
      Serial.println("ml");
    } else {
      Serial.println("Skipping ThingSpeak update - invalid sensor data");
    }
  }
  delay(2000); 
}// 2-second loop delay