#include "DHT.h"
#include "LiquidCrystal.h"

// ===== Pin Definitions =====
#define DHTPIN 13            // DHT11 signal pin
#define DHTTYPE DHT22       // DHT type
#define SOIL_PIN 34         // Soil moisture sensor (analog input)
#define RELAY_PIN 12        // Relay control pin

// LCD Pins: LiquidCrystal(rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(5, 18, 15, 2, 19, 21);

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.begin(16, 2);          // 16 columns, 2 rows

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Start with pump OFF

  lcd.print("System Starting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();


  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT read failed");
    return; 
}

  int soilValue = analogRead(SOIL_PIN);

  // Display on Serial Monitor
  Serial.print("Temp: "); Serial.print(temperature);
  Serial.print(" C | Humidity: "); Serial.print(humidity);
  Serial.print(" % | Soil: "); Serial.println(soilValue);

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("T:"); lcd.print(temperature);
  lcd.print("C H:"); lcd.print(humidity);
  lcd.print("%");

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
