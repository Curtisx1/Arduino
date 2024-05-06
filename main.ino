#include <DFRobot_DHT11.h>
#include <Servo.h>
#include "thingProperties.h"

DFRobot_DHT11 DHT1;
DFRobot_DHT11 DHT2;
DFRobot_DHT11 DHT3;
DFRobot_DHT11 DHT4;

// Variables 
#define DHT11_PIN1 9 // South-East Corner
#define DHT11_PIN2 10 // North-West Corner
#define DHT11_PIN3 11 // South-West Corner
#define DHT11_PIN4 12 // North-East Corner
/*
String soilStatus;
float avgHum;
float hum1;
float hum2;
float hum3;
float hum4;
float moistureValue = 0;
float avgTemp;
float temp1;
float temp2;
float temp3;
float temp4;
bool fanStatus;
 */
const int AirValue = 756;
const int WaterValue = 0;
int intervals = (AirValue - WaterValue)/3;


const int COVER_CLOSED_POS = 165;
const int COVER_OPEN_POS = 5;
const int FAN_DELAY_MS = 2000;
const int FAN_SPEED_HIGH = HIGH;
const int FAN_SPEED_OFF = LOW;  // Assuming LOW turns the fan off
const int FAN_PIN = 3;
const int HUMIDITY_THRESHOLD = 68;
const int TEMPERATURE_THRESHOLD = 88;
const int DHT11_DELAY_MS = 3000;
Servo myservo;
int pos = 0;

void readAndPrint(DFRobot_DHT11 DHT, int pin, String sensorNumber){
  DHT.read(pin);
  float fahrenheit = (DHT.temperature * 9.0 / 5.0) + 32;
  Serial.print("Temperature" + sensorNumber + ": ");
  Serial.print(fahrenheit);
  Serial.print("°F");
  Serial.print("  Humidity" + sensorNumber + ": ");
  Serial.print(DHT.humidity);
  Serial.println("%");
  ArduinoCloud.update();
}

void setup(){
  // Defined in thingProperties.h
  initProperties();

  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection, false);
  
  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
 */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  Serial1.begin(9600); // Servo
  Serial2.begin(9600); // Soil Moisture Guage
  myservo.attach(8); // Exhaust Fan Motor -Open/Close
  pinMode (2, OUTPUT); // Internal Fan
  pinMode (3, OUTPUT); // Exhaust Fan
}

int getServoPosition() {
  return pos;
}

void exhaustCover(int pos){
  myservo.write(pos);
  ArduinoCloud.update();
}

void controlFan(int pin, int state){
  digitalWrite(pin, state);
  ArduinoCloud.update();
}

// Function to check and print soil moisture level
void checkSoilMoisture() {
  moistureValue = analogRead(A0);  // Read moisture level from sensor A0
  Serial.print("Moisture Value: ");
  Serial.println(moistureValue);
  ArduinoCloud.update();
  Serial.println("Check Moisture Function ran");

  if (moistureValue > WaterValue && moistureValue < (WaterValue + intervals)) {
    Serial.println("Very Wet");
    soilStatus = "Very Wet";
    ArduinoCloud.update();
  } else if (moistureValue > (WaterValue + intervals) && moistureValue < (AirValue - intervals)) {
    Serial.println("Wet");
    soilStatus = "Wet";
    ArduinoCloud.update();
  } else if (moistureValue < AirValue && moistureValue > (AirValue - intervals)) {
    Serial.println("Dry");
    soilStatus = "Dry";
    ArduinoCloud.update();
  }
}

void loop(){
  ArduinoCloud.update();
  controlFan(2, HIGH); // Turns the internal fan on
  
  checkSoilMoisture();  // Call the soil moisture check function
  
  delay(DHT11_DELAY_MS); 
  
  readAndPrint(DHT1, DHT11_PIN1, "1");
  delay(DHT11_DELAY_MS); 
  readAndPrint(DHT2, DHT11_PIN2, "2");
  delay(DHT11_DELAY_MS); 
  readAndPrint(DHT3, DHT11_PIN3, "3");
  delay(DHT11_DELAY_MS); 
  readAndPrint(DHT4, DHT11_PIN4, "4");
  delay(DHT11_DELAY_MS); 

  DHT1.read(DHT11_PIN1);
  hum1 = DHT1.humidity;
  temp1 = DHT1.temperature;
  ArduinoCloud.update();
  delay(DHT11_DELAY_MS); 

  DHT2.read(DHT11_PIN2);
  hum2 = DHT2.humidity;
  temp2 = DHT2.temperature;
  ArduinoCloud.update();
  delay(DHT11_DELAY_MS);  

  DHT3.read(DHT11_PIN3);
  hum3 = DHT3.humidity;
  temp3 = DHT3.temperature;
  ArduinoCloud.update();
  delay(DHT11_DELAY_MS); 

  DHT4.read(DHT11_PIN4);
  hum4 = DHT4.humidity;
  temp4 = DHT4.temperature;
  ArduinoCloud.update();
  delay(DHT11_DELAY_MS);
  
  avgHum = (hum1 + hum2 + hum3 + hum4) / 4.0;
  avgTemp = (temp1 + temp2 + temp3 + temp4) / 4.0 * 9.0 / 5.0 + 32.0;
  ArduinoCloud.update();
  Serial.println("Average Humidity: " + String(avgHum) + "%");
  Serial.println("Average Temperature: " + String(avgTemp) + "°F");

  ArduinoCloud.update();
  
  if (avgHum >= HUMIDITY_THRESHOLD || avgTemp > TEMPERATURE_THRESHOLD) {
    if (!fanStatus) { // Check if the fan is off, then turn it on and open the exhaust
      pos = getServoPosition();
      if (pos == COVER_OPEN_POS) {
        exhaustCover(COVER_OPEN_POS);
        pos = 170;
        ArduinoCloud.update();
      }
      delay(FAN_DELAY_MS);
      controlFan(FAN_PIN, FAN_SPEED_HIGH);
      fanStatus = true;
      ArduinoCloud.update();
    }
  } else {
    if (fanStatus) { // Check if the fan is on, then turn it off and close the exhaust
      controlFan(FAN_PIN, FAN_SPEED_OFF);
      fanStatus = false;
      delay(5000);
      exhaustCover(COVER_CLOSED_POS);
      pos = 0;
      ArduinoCloud.update();
    }
  }
  delay(FAN_DELAY_MS); // General delay to prevent the loop from running too quickly
  ArduinoCloud.update();
}