#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include <sensor.h>
#include <sub.h>
#include <PostData.h>
#include "freertos/timers.h"

// Konfigurationen
/*->Modus
mode 1 -> Normaler Betrieb
mode 2 -> kein Netzwerkverkehr
mode 3 -> MQTT verwenden aber kein Luftdata
mode 4 -> Luftdata benutzen aber kein MQTT*/
#define Mode 3

//Netzwerk+++++++++++++++++++++++++++++
const char *ssid1 = "hw1_gast";  //CHANGE!!!!!!!!!!!!!!!
const char *password1 = "KeineAhnung"; //CHANGE!!!!!!!!!!!!
//Deepsleep++++++++++++++++++++++++++++
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  122

//In sub.cpp Wlan und MQTT ergänzen
// Ende Konfiguration

sensor sensor;
sub sub;
WiFiClass wifi;
PostData pd;
TimerHandle_t tmr;
static int network = 0;
static int loopcount=0;
RTC_DATA_ATTR int bootCount = 0;
boolean luftdata;
boolean mqtt;
//27 to s of sensordatavalues  ----  58 to e of value_type ---- 75 is first value
char *myArray = "{\"software_version\": 1.0, \"sensordatavalues\":[{\"value_type\":\"P1\",\"value\":\"X\"},{\"value_type\":\"P2\",\"value\":\"53.32\"}]}"; 

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = ESP_SLEEP_WAKEUP_TIMER;

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup_wifi(void) // WLANverbindung aufbauen
{
    int count;
    wifi.mode(WIFI_MODE_MAX);
    wifi.begin(ssid1, password1);
    Serial.printf("Try to connect to: %s", ssid1);
    while (wifi.status() != WL_CONNECTED && count <1000)
    {
        Serial.print(".");
        count++;
        delay(100);
    }
    network= (int)wifi.isConnected();
    Serial.println(wifi.isConnected());
    Serial.println(wifi.isConnected());
    wifi.mode(WIFI_MODE_STA);
}

void setup() //Init Routine
{
    delay(1000);
    Serial.begin(9600);

    int id=1;
    int interval = 100;
    ++bootCount;
    Serial.println("Boot number: " + String(bootCount));

    sensor.setupSensor();

    int chipid=ESP.getEfuseMac();//Bordnummer ausgeben
    Serial.println(chipid);
    Serial.println("Der Modus ist");
    Serial.println(Mode);
    switch(Mode){//Konfigurieren des Systems nach Modus
        case 1:setup_wifi();luftdata=true;mqtt=true; Serial.println("Mode 1"); 
            break;
        case 2: luftdata=false;mqtt=false;Serial.println("Mode 2"); 
            break;
        case 3: setup_wifi();Serial.println("Mode 3"); 
                sub.setup_ota();
                sub.setupSub(); 
                network=true;
                mqtt=true;
                break;
        case 4: setup_wifi();Serial.println("Mode 4"); 
                network=true;
            break;
        default: Serial.println("No mode set - ERROR");
    }
    sensor.sleep(false);
    delay(500);//Warten bis der Sds011 startklar ist
print_wakeup_reason();
}

void loop()
{
    loopcount++;
    sensor.sensorloop();
    if(network&&mqtt){
        Serial.println("loopmqtt");
        sensor.sensorData();
        sub.loopSub();
    }
    if(luftdata&&network&&loopcount==19){
        pd.postDataa('A');
    }
    delay(1000);
    if (loopcount>20){
      Serial.println("Going to sleep now");
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      sensor.sleep(true);//schlafen legen 
      Serial.flush();
    esp_deep_sleep_start();
    }
}
