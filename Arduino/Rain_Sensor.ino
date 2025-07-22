#include "DHT.h" 

#define ERROR_CODE -1.f
// ===================
// Arduino Uno Pin Set
// ===================
#define DHTPIN 7       
#define DHTTYPE DHT11  
DHT dht(DHTPIN, DHTTYPE);
const int ANALOG_PIN = A0; // A0
const int DIGITAL_PIN = 2; // D2


// ===================
// Rain Sensor Setup
// ===================
void setup() {
  Serial.begin(115200);  // baud rate 
  pinMode(DIGITAL_PIN, INPUT);
  dht.begin();
}

// =============
// Program loop
// =============
void loop() {
  // 1. Read Sensing Data
  int analogValue = analogRead(ANALOG_PIN);
  int digitalValue = digitalRead(DIGITAL_PIN);
  float humidity = dht.readHumidity();   
  float temperature = dht.readTemperature(); 

  // error handling code, error code : -1
  if (isnan(humidity) || isnan(temperature)) {
    humidity = ERROR_CODE; 
    temperature = ERROR_CODE;
  }

  // 2. Send to Datas by Serial Port
  Serial.print(analogValue);
  Serial.print(",");
  Serial.print(digitalValue);
  Serial.print(",");
  Serial.print(humidity); 
  Serial.print(",");
  Serial.println(temperature); // '\n'
  delay(500); 
}