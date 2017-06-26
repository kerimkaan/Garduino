/*
 * Garduino | 0.1
 * Plant Monitoring & Management System on Ardunio.
 * 
 * Water level detecting: Sensor(A0=Analog), led1(7), integers(watersensor,WaterSensorValue)
 * DHT temp & humidity detecting: Sensor(Digital 2), integers(h,t,hic)
 * Servo motor controlling: Motor(Digital 9), integers(motor,pos)
 */
#include <DHT.h>
#include <DHT_U.h>
#include <Servo.h>

Servo motor;
int pos = 0;
#define DHTPIN 2 // DHT sensörü 2. pine bağlı
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
int led1 = 7; // LED1(Mavi) 7. pine bağlı
int led2 = 12; // LED2(Sarı) 12. pine bağlı
int led3 = 11;  // LED3(Kırmızı) 11. pine bağlı
int watersensor = 0; // Water sensor integer A0


void setup() {
 Serial.begin(9600);
 motor.attach(9);
 Serial.println("Garduino 0.1 - Plant Monitoring & Management System");
 dht.begin();
  pinMode(led1, OUTPUT);
}

void loop() {
 int WaterSensorValue = analogRead(watersensor);
  float h = dht.readHumidity(); // Read humidity (%) - Nem oranı
  float t = dht.readTemperature(); // Read temperature as Celsius - Celsius cinsinden sıcaklık değeri
  float hic = dht.computeHeatIndex(t, h, false); // Compute heat index in Celsius - Isı indeksi (Hissedilen sıcaklık)
  Serial.println("-------------------------------------------------------------------");
  Serial.print("Ortam Nemi / Env. Humidity: ");
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print("Ortam Sicakligi / Env. Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  Serial.print("Isi Indeksi(Hissedilen Sicaklik) / Heat Index: ");
  Serial.print(hic);
  Serial.println(" *C ");
  
 
 if (WaterSensorValue >= 500){
  Serial.println("Toprak su seviyesi / Soil Water level: TOO HIGH");
  digitalWrite(led3,HIGH);
  delay(250);
  digitalWrite(led3,LOW);
  delay(100);
  digitalWrite(led3,HIGH);
  delay(250);
  digitalWrite(led3,LOW);
 }
 else if (WaterSensorValue >= 400 && WaterSensorValue < 500)
 {
  Serial.println("Toprak su seviyesi / Soil Water level: HIGH");
  digitalWrite(led2,HIGH);
  delay(500);
  digitalWrite(led2,LOW);
 }
 else if (WaterSensorValue >= 250 && WaterSensorValue < 400)
 {
  Serial.println("Toprak su seviyesi / Soil Water level: NORMAL");
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
 }
 else if (WaterSensorValue >= 0 && WaterSensorValue < 250)
 {
  digitalWrite(led1,HIGH);
  Serial.println("Toprak su seviyesi / Soil Water level: LOW");
  
  for (pos = 0; pos <= 90; pos += 5) {
    // in steps of 1 degree
    
    motor.write(pos);              
       delay(150);
       
  }
  digitalWrite(led1,LOW);
 }
 Serial.println("-------------------------------------------------------------------");
delay(1500);
}
