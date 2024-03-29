#include <DFRobot_DHT11.h>
#include <Servo.h>

DFRobot_DHT11 DHT1;
DFRobot_DHT11 DHT2;
DFRobot_DHT11 DHT3;
DFRobot_DHT11 DHT4;

// Variables 
#define DHT11_PIN1 9 // South-East Corner
#define DHT11_PIN2 10 // North-West Corner
#define DHT11_PIN3 11 // South-West Corner
#define DHT11_PIN4 12 // North-East Corner

Servo myservo;
int pos = 0;
float h1, h2, h3, h4, t1, t2, t3, t4;
float tf1, tf2, tf3, tf4;
float avgHumidity, avgTemperature;

void readAndPrint(DFRobot_DHT11 DHT, int pin, String sensorNumber){
  DHT.read(pin);
  Serial.print("Temperature" + sensorNumber + ": ");
  Serial.print(DHT.temperature);
  Serial.print("°F");
  Serial.print("  Humidity" + sensorNumber + ": ");
  Serial.print(DHT.humidity);
  Serial.println("%");
}

void setup(){
  Serial.begin(9600); // Servo
  myservo.attach(8); // Exhaust Fan Motor -Open/Close
  pinMode (2, OUTPUT); // Internal Fan
  pinMode (3, OUTPUT); // Exhaust Fan
}

int getServoPosition() {
  return pos;
}

void exhaustCover(int pos){
  myservo.write(pos);
}

void controlFan(int pin, int state){
  digitalWrite(pin, state);
}

void loop(){

  controlFan(2, HIGH);

  readAndPrint(DHT1, DHT11_PIN1, "1");
  delay(3000); 
  readAndPrint(DHT2, DHT11_PIN2, "2");
  delay(3000); 
  readAndPrint(DHT3, DHT11_PIN3, "3");
  delay(3000); 
  readAndPrint(DHT4, DHT11_PIN4, "4");
  delay(3000); 

  DHT1.read(DHT11_PIN1);
  h1 = DHT1.humidity;
  t1 = DHT1.temperature;
  delay(3000); 

  DHT2.read(DHT11_PIN2);
  h2 = DHT2.humidity;
  t2 = DHT2.temperature;
  delay(3000);  

  DHT3.read(DHT11_PIN3);
  h3 = DHT3.humidity;
  t3 = DHT3.temperature;
  delay(3000); 

  DHT4.read(DHT11_PIN4);
  h4 = DHT4.humidity;
  t4 = DHT4.temperature;
  delay(3000); 

  avgHumidity = (h1 + h2 + h3 + h4) / 4.0;
  avgTemperature = (t1 + t2 + t3 + t4) / 4.0 * 9.0 / 5.0 + 32.0;

  Serial.println("Average Humidity: " + String(avgHumidity) + "%");
  Serial.println("Average Temperature: " + String(avgTemperature) + "°F");

  if(avgHumidity >= 40){
    int pos = getServoPosition();
    if(pos == 0){
      exhaustCover(pos);
      delay(2000);
      controlFan(3, HIGH);
    } else {
      pos = 170;
      exhaustCover(pos);
      delay(2000);
      controlFan(3, HIGH);
    }
  }
  delay(2000);
}
