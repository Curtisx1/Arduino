#include<Servo.h>
#include <DFRobot_DHT11.h>
DFRobot_DHT11 DHT11;

Servo servo;
// External temp Sensor -> Connected to PIN 1
int sensorPin = 1;
// Humitity Sensor -> Connecting to PIN 6
int DHT11PIN = 7;

void setup() {
  // Fan Servo-Connected to PIN 9
  servo.attach(9);
  // Locking servo-Connected to PIN
  servo.attach();
  // Rear hatch servo-Connected to PIN
  servo.attach();
  // Delay of 1 Baud
  Serial.begin(9600);
}

void loop() {
  // Reading the temperature. Prints in C and F.
  // Prints voltage reading. Set to 3.3v when powered by the Arduino
  int reading = analogRead(sensorPin);
  float voltage = reading * 3.3;
  voltage /= 1024.0;
  Serial.print(voltage); Serial.println(" volts");
  float temperatureC = (voltage - 0.5) * 100 ;
  Serial.print(temperatureC); Serial.println(" degrees C");
  // Converting C to F
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  Serial.print(temperatureF); Serial.println(" degrees F");
  // DHT11 Humidity Sensor read out
  delay(2000);
  DHT11.read(DHT11PIN);
  // Printing output of the sensor
  Serial.print("Humidity (%): ");
  Serial.println((float)DHT11.humidity, 2);
  //Exhaust Fan servo 1 -> PIN 9
  delay(2000);
  // Opens the vent
  servo.attach(9);
  servo.write(0);
  // Add in logic for temp and humidity to turn the servo back on, thus closing the vent.
  delay(2000);
  // Closes the vent
  servo.attach(9);
  servo.write(165);
  delay(2000);
  // Locking servo 2 -> PIN
  // Rear hatch servo 3 -> PIN
}