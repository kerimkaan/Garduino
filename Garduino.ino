/*
 * Garduino | 0.2
 * Plant Monitoring & Management System on Arduino
 *
 * DHT temp & humidity detecting: Sensor(Digital 2), integers(h,t,hic), Wire a 10k Ohm resistor to VCC
 * Servo motor controlling: Motor(Digital 9), integers(motor,pos)
 * IR Controlling (Optional,Not Include): Receiver(Digital 13), integers(RECV_PIN)
 * Water level (Distance) detecting: Sensor(Digital 3 & 2), VCC to Arduino 5v GND to Arduino GND
 * Soil humidity detecting: Sensor(A0), VCC to 5V
 *
 *
 * Before use please read README.md file.
 * Kullanmadan önce lütfen BENİOKU.md dosyasını okuyunuz.
 *
 * Repository: github.com/kerimkaan/Garduino
 */

// Libraries
#include <DHT.h> // DHT library
#include <DHT_U.h> // DHT 11 & 22 detecting library
#include <Servo.h> // Servo library

// Servo motor
Servo motor;
int pos = 0;

// DHT Sensor
#define DHTPIN 2 // DHT to Arduino Digital 2 / DHT sensörü 2. pine bağlı
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Distance sensor (HC-SR04) for water level detecting (HC-SR04) Pins
#define trigPin 3 // Digital 3
#define echoPin 2 // Digital 2

// LEDs
int led1 = 7; // LED1(Mavi, Blue) 7. pine bağlı
int led2 = 12; // LED2(Sarı, Yellow) 12. pine bağlı
int led3 = 11;  // LED3(Kırmızı, Red) 11. pine bağlı

// Soil humidity sensor
int soilsensor = 0; // Soil humidity sensor integer, Arduino A0
int nem; // Humidity integer


void setup() {
 Serial.begin(9600);
 motor.attach(9); // Servo, Digital 9
 Serial.println("Garduino 0.2 - Plant Monitoring & Management System");
 dht.begin();
  pinMode(led1, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
 // Soil humidity detecting
  nem= analogRead(soilsensor);
  nem= map(nem,0,1023,0,100);

  // Enviroment temperature & humidity detecting, monitoring
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
  Serial.print("Toprak Nemi / Soil Humidity: ");
  Serial.println(nem);

  // Water level detecting & monitoring
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print("Su seviyesi / Water level: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 5){
    Serial.println("Su seviyesi çok düşük / Water level is too low");
    Serial.print("Su seviyesi çok düşük / Water level is too low");
    digitalWrite(led1,HIGH);
    digitalWrite(led2,HIGH);
    digitalWrite(led3,HIGH);
    Serial.println("-------------------------------------------------------------------");
  }
  else{
    if (nem  >= 0 && nem  < 20){
     Serial.println("Toprak nem seviyesi / Soil humidity level: TOO HIGH");
     digitalWrite(led3,HIGH);
     delay(250);
     digitalWrite(led3,LOW);
     delay(100);
     digitalWrite(led3,HIGH);
     delay(250);
     digitalWrite(led3,LOW);
    }
    else if (nem  >= 20 && nem  < 50)
    {
     Serial.println("Toprak nem seviyesi / Soil humidity level: HIGH");
     digitalWrite(led2,HIGH);
     delay(500);
     digitalWrite(led2,LOW);
    }
    else if (nem  >= 50 && nem  < 75)
    {
     Serial.println("Toprak nem seviyesi / Soil humidity level: NORMAL");
     digitalWrite(led1,LOW);
     digitalWrite(led2,LOW);
     digitalWrite(led3,LOW);
    }
    else if (nem  >= 75 && nem  < 100)
    {
     digitalWrite(led1,HIGH);
     Serial.println("Toprak nem seviyesi / Soil humidity level: LOW");
     Serial.println("Sulama yapiliyor... / Watering...");
     for (pos = 0; pos <= 90; pos += 5) {
       // in steps of 5 degrees

       motor.write(pos);
          delay(150);

     }
     digitalWrite(led1,LOW);
    }
    Serial.println("-------------------------------------------------------------------");
  }


delay(1000);
}
