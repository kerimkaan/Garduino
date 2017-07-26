/*
 * Garduino | 0.4
 * Plant Monitoring & Management System on Arduino
 *
 * DHT temp & humidity detecting: Sensor(Digital 2), integers(h,t,hic), Wire a 10k Ohm resistor to VCC
 * IR Controlling (Optional,Not Include): Receiver(Digital 13), integers(RECV_PIN)
 * Water level (Distance) detecting: Sensor(Digital 3 & 2), VCC to Arduino 5v GND to Arduino GND
 * Soil humidity detecting: Sensor(A0), VCC to 5V
 * ESP8266: TX,RX, VCC to 3.3V, normal using & Thingspeak API integration
 *
 *
 * Before use please read README.md file.
 * Kullanmadan önce lütfen BENİOKU.md dosyasını okuyunuz.
 *
 * Repository: github.com/kerimkaan/Garduino
 */

// Libraries
#include <DHT.h>    // DHT library
#include <DHT_U.h>  // DHT 11 & 22 detecting library
#include <Servo.h>  // Servo library
// Wi-Fi network identification
#define ssid "SSID"            // Your Wi-Fi SSID
#define pass "Pass"            // Your Wi-Fi password
#define IP "184.106.153.149"   // thingspeak.com IP address

// DHT Sensor
#define DHTPIN 2  // DHT to Arduino Digital 2 / DHT sensörü 2. pine bağlı
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Distance sensor (HC-SR04) for water level detecting (HC-SR04) Pins
#define trigPin 3 // Digital 3
#define echoPin 2 // Digital 2

// LEDs
const int led1 = 5; // LED1(Mavi, Blue) 7. pine bağlı
const int led2 = 6; // LED2(Sarı, Yellow) 12. pine bağlı
const int led3 = 7;  // LED3(Kırmızı, Red) 11. pine bağlı
const int pump = 15; // Water pump to Digital 13

// Soil humidity sensor
int soilsensor = 0; // Soil humidity sensor integer, Arduino A0
int nem; // Humidity integer

const int INTERNAL;


void setup() {
 Serial.begin(115200);
 Serial.println("Garduino 0.4 - Plant Monitoring & Management System");
 dht.begin();

 // Join the Wi-Fi network with ESP8266 Module
 Serial.println("AT"); //ESP modülümüz ile bağlantı kurulup kurulmadığını kontrol ediyoruz.
 delay(3000); //ESP ile iletişim için 3 saniye bekliyoruz.
 analogReference(INTERNAL);
 if(Serial.find("OK")){         //esp modülü ile bağlantıyı kurabilmişsek modül "AT" komutuna "OK" komutu ile geri dönüş yapıyor.
    Serial.println("AT+CWMODE=1"); //esp modülümüzün WiFi modunu STA şekline getiriyoruz. Bu mod ile modülümüz başka ağlara bağlanabilecek.
    delay(2000);
    String baglantiKomutu=String("AT+CWJAP=\"")+ssid+"\",\""+pass+"\"";
   Serial.println(baglantiKomutu);
   // LEDs & sensor
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(pump, OUTPUT);
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
  Serial.print("Ortam Nemi / Ambient Humidity: ");
  Serial.print(h);
  Serial.println(" %\t");
  Serial.print("Ortam Sicakligi / Ambient Temperature: ");
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
     Serial.println("Toprak nem seviyesi / Soil humidity level: LOW");
     Serial.print("Lutfen sulama yapiniz. / Please watering.");
     digitalWrite(led1,HIGH);
     /* NOT TESTED WATER PUMP
     analogWrite(pump, 255); // Water pump is running maximum level
     delay(2000);           //  Watering 2 seconds
     analogWrite(pump, 0);  // Water pump stopped
     */
     digitalWrite(led1,LOW);
    }
    Serial.println("-------------------------------------------------------------------");
  }
  // Functions of collect & send data to Thingspeak
    sicaklik_yolla(t);
    nem_yolla(h);
    toprak_yolla(nem);
    mesafe_yolla(distance);
}

/*
*  Collect and send data to Thingspeak API
*  sicaklik_yolla = yollanacakkomut1 = t = Temperature data
*  nem_yolla = yollanacakkomut2 = h = Ambient humidity data
*  toprak_yolla = yollanacakkomut3 = nem = Soil humidity data
*  mesafe_yolla = nacakkomut4 = distance = Distance sensor data
*
*  Thingspeak API: "GET /update?key=YOUR_API_KEY&fieldYOUR_FIELD="
*/

void sicaklik_yolla(float t){
 Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna bağlanmak için bu kodu kullanıyoruz.
                                                                   //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz.
                                                                   //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor
 delay(250);
  if(Serial.find("Error")){     //sunucuya bağlanamazsak ESP modülü bize "Error" komutu ile dönüyor.
   Serial.println("AT+CIPSTART Error");
    return;
  }
String yollanacakkomut1 = "GET /update?key=R0XZLJNI69MSWQ9Z&field1=";   // Thingspeak API Integration
        yollanacakkomut1 += (float(t));                                      // Burada ise sıcaklığımızı float değişkenine atayarak yollanacakkomut değişkenine ekliyoruz.
        yollanacakkomut1 += "\r\n\r\n";                                             // ESP modülümüz ile seri iletişim kurarken yazdığımız komutların modüle iletilebilmesi için Enter komutu yani
         delay(2000);                                                                                // /r/n komutu kullanmamız gerekiyor.

        Serial.print("AT+CIPSEND=");                    //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
        Serial.println(yollanacakkomut1.length()+2);       //yollanacakkomut değişkeninin kaç karakterden oluştuğunu .length() ile bulup yazırıyoruz.

        delay(1000);

        if(Serial.find(">")){                           //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
                                                        // arduino da ">" işaretini gördüğü anda sıcaklık verisini esp modülü ile thingspeak sunucusuna yolluyor.
        Serial.print(yollanacakkomut1);
        Serial.print("\r\n\r\n");
        }
        else{
        Serial.println("AT+CIPCLOSE");
        }
        }

void nem_yolla(float h){
           Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna bağlanmak için bu kodu kullanıyoruz.
                                                                             //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz.
                                                                             //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor
           delay(250);
            if(Serial.find("Error")){     //sunucuya bağlanamazsak ESP modülü bize "Error" komutu ile dönüyor.
             Serial.println("AT+CIPSTART Error");
              return;
            }
String yollanacakkomut2 = "GET /update?key=R0XZLJNI69MSWQ9Z&field2=";   // Thingspeak API Integration
                yollanacakkomut2 += (float(h));                                      // Burada ise sıcaklığımızı float değişkenine atayarak yollanacakkomut değişkenine ekliyoruz.
                yollanacakkomut2 += "\r\n\r\n";                                             // ESP modülümüz ile seri iletişim kurarken yazdığımız komutların modüle iletilebilmesi için Enter komutu yani
                 delay(2000);                                                                                // /r/n komutu kullanmamız gerekiyor.

                Serial.print("AT+CIPSEND=");                    //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
                Serial.println(yollanacakkomut2.length()+2);       //yollanacakkomut değişkeninin kaç karakterden oluştuğunu .length() ile bulup yazırıyoruz.

                delay(1000);

                if(Serial.find(">")){                           //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
                                                                // arduino da ">" işaretini gördüğü anda sıcaklık verisini esp modülü ile thingspeak sunucusuna yolluyor.
                Serial.print(yollanacakkomut2);
                Serial.print("\r\n\r\n");
                }
                else{
                Serial.println("AT+CIPCLOSE");
                }
                }

void toprak_yolla(int nem){
                     Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna bağlanmak için bu kodu kullanıyoruz.
                                                                                       //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz.
                                                                                       //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor
                     delay(250);
                      if(Serial.find("Error")){     //sunucuya bağlanamazsak ESP modülü bize "Error" komutu ile dönüyor.
                       Serial.println("AT+CIPSTART Error");
                        return;
                      }
String yollanacakkomut3 = "GET /update?key=R0XZLJNI69MSWQ9Z&field3=";   // Thingspeak API Integration
                        yollanacakkomut3 += (float(nem));                                      // Burada ise sıcaklığımızı float değişkenine atayarak yollanacakkomut değişkenine ekliyoruz.
                        yollanacakkomut3 += "\r\n\r\n";                                             // ESP modülümüz ile seri iletişim kurarken yazdığımız komutların modüle iletilebilmesi için Enter komutu yani
                         delay(2000);                                                                                // /r/n komutu kullanmamız gerekiyor.

                        Serial.print("AT+CIPSEND=");                    //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
                        Serial.println(yollanacakkomut3.length()+2);       //yollanacakkomut değişkeninin kaç karakterden oluştuğunu .length() ile bulup yazırıyoruz.

                        delay(1000);

                        if(Serial.find(">")){                           //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
                                                                        // arduino da ">" işaretini gördüğü anda sıcaklık verisini esp modülü ile thingspeak sunucusuna yolluyor.
                        Serial.print(yollanacakkomut3);
                        Serial.print("\r\n\r\n");
                        }
                        else{
                        Serial.println("AT+CIPCLOSE");
                        }
                        }

void mesafe_yolla(long distance){
                               Serial.println(String("AT+CIPSTART=\"TCP\",\"") + IP + "\",80");  //thingspeak sunucusuna bağlanmak için bu kodu kullanıyoruz.
                                                                                                 //AT+CIPSTART komutu ile sunucuya bağlanmak için sunucudan izin istiyoruz.
                                                                                                 //TCP burada yapacağımız bağlantı çeşidini gösteriyor. 80 ise bağlanacağımız portu gösteriyor
                               delay(250);
                                if(Serial.find("Error")){     //sunucuya bağlanamazsak ESP modülü bize "Error" komutu ile dönüyor.
                                 Serial.println("AT+CIPSTART Error");
                                  return;
                                }
String yollanacakkomut4 = "GET /update?key=R0XZLJNI69MSWQ9Z&field4=";   // Thingspeak API Integration
                                                yollanacakkomut4 += (long(distance));                                      // Burada ise sıcaklığımızı float değişkenine atayarak yollanacakkomut değişkenine ekliyoruz.
                                                yollanacakkomut4 += "\r\n\r\n";                                             // ESP modülümüz ile seri iletişim kurarken yazdığımız komutların modüle iletilebilmesi için Enter komutu yani
                                                 delay(2000);                                                                                // /r/n komutu kullanmamız gerekiyor.

                                                Serial.print("AT+CIPSEND=");                    //veri yollayacağımız zaman bu komutu kullanıyoruz. Bu komut ile önce kaç tane karakter yollayacağımızı söylememiz gerekiyor.
                                                Serial.println(yollanacakkomut4.length()+2);       //yollanacakkomut değişkeninin kaç karakterden oluştuğunu .length() ile bulup yazırıyoruz.

                                                delay(1000);

                                                if(Serial.find(">")){                           //eğer sunucu ile iletişim sağlayıp komut uzunluğunu gönderebilmişsek ESP modülü bize ">" işareti ile geri dönüyor.
                                                                                                // arduino da ">" işaretini gördüğü anda sıcaklık verisini esp modülü ile thingspeak sunucusuna yolluyor.
                                                Serial.print(yollanacakkomut4);
                                                Serial.print("\r\n\r\n");
                                                }
                                                else{
                                                Serial.println("AT+CIPCLOSE");
                                                }
                                                }
