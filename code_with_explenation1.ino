#include "DHT.h"  //DHT22 library for it to monitor humidity
#include "LiquidCrystal.h"  //lCD screen library 
#include "WiFi.h"
#include "ThingSpeak.h"

// ===== Pin Definitions =====
#define DHTPIN 13            // DHT11 signal pin
#define DHTTYPE DHT22       // DHT type
#define SOIL_PIN 34         // Soil moisture sensor (analog input)
#define RELAY_PIN 12        // Relay control pin

// LCD Pins: LiquidCrystal(rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(5, 18, 15, 2, 19, 21);

DHT dht(DHTPIN, DHTTYPE);
const char* WIFI_SSID = "SHELL 082F26";
const char* WIFI_PASSWORD = "fz4Ey6bRN3ThzFJx";
unsigned long THINGSPEAK_CHANNEL_ID = 3079658;
const char* THINGSPEAK_API_KEY = "ALBSWTF6P4UCJG56";

WiFiClient client;
unsigned long lastSendTime = 0;
const long updateInterval = 60000;

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.begin(16, 2);          // 16 columns, 2 rows

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Start with pump OFF



  Serial.print("Connecting to WiFi...");
  lcd.clear();
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

  }
void loop()
 // Initialize ThingSpeak
  {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilValue = analogRead(SOIL_PIN);


  unsigned long currentTime = millis();
  if (currentTime - lastSendTime >= updateInterval) {
    lastSendTime = currentTime;

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Skipping ThingSpeak update due to bad sensor data.");
    } else {
      ThingSpeak.setField(1, temperature);
      ThingSpeak.setField(2, humidity);
      ThingSpeak.setField(3, soilValue);
      ThingSpeak.writeFields(THINGSPEAK_CHANNEL_ID, THINGSPEAK_API_KEY);
      
      Serial.print("Data sent: ");
      Serial.print("Temp: "); Serial.print(temperature);
      Serial.print(" C | Humidity: "); Serial.print(humidity);
      Serial.print(" F | Soil: "); Serial.println(soilValue);
    }
  }

 

  // Display on Serial Monitor
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" C | Humidity: "); Serial.print(humidity);
  Serial.print(" % | Soil: "); Serial.println(soilValue);

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(temperature);
  lcd.print("C H:"); lcd.print(humidity);
  lcd.print("F");

  lcd.setCursor(0, 1);
  lcd.print("Soil: "); lcd.print(soilValue);

  // Control Relay
  if (soilValue > 2000) {
    digitalWrite(RELAY_PIN, HIGH);  // Dry → Pump ON
    Serial.println("Pump: ON");
    lcd.print(" DRY");
  } else {
    digitalWrite(RELAY_PIN, LOW);   // Wet → Pump OFF
    Serial.println("Pump: OFF");
    lcd.print(" WET");
  }

  delay(2000);
}
