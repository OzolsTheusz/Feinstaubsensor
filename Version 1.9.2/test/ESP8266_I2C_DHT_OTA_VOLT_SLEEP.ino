#include <ESP8266WiFi.h>

#include <PubSubClient.h>
#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "DHTesp.h"

#ifdef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP8266 ONLY!)
#error Select ESP8266 board.
#endif

#include <NDIRZ16.h>
#include <SoftwareSerial.h>

// Topics to Subscribe and to Publish

#define MQTT_INTOPIC  "42nibbles/MQTT/SENSOR/TEMPERATUR"
#define MQTT_WINKEL   "42nibbles/MQTT/SENSOR/WINKEL"
#define MQTT_TEMP     "42nibbles/MQTT/SENSOR/AUSSEN/TEMP"
#define MQTT_TEMP2    "42nibbles/MQTT/SENSOR/AUSSEN/TEMP2"
#define MQTT_HUM      "42nibbles/MQTT/SENSOR/AUSSEN/HUM"
#define MQTT_VOLT     "42nibbles/MQTT/SENSOR/AUSSEN/VOLT"
#define MQTT_STATUS   "42nibbles/MQTT/SENSOR/AUSSEN/STATUS"
#define MQTT_WILL     "42nibbles/MQTT/SENSOR/WILL"
#define MQTT_OTA      "42nibbles/MQTT/SENSOR/AUSSEN/OTA"
#define MQTT_CO2      "42nibbles/MQTT/SENSOR/AUSSEN/CO2"
#define MQTT_SLEEP    "42nibbles/MQTT/SLEEP"

//  definition of DHT-sensor

#define DHTTYPE   DHT11               // DHT 11
#define DHTPIN    13                	// what digital pin we're connected to
SoftwareSerial    s_serial(14, 12);   // TX, RX for the CO2-Sensor

#define sensor s_serial

const unsigned char cmd_get_sensor[] =
{
    0xff, 0x01, 0x86, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x79
};

unsigned char dataRevice[9];
int temperature;
int CO2PPM;
DHTesp dht;




// Update these with values suitable for your network.
/* 
  Credentials Wulfen Dimker Allee
  Achtung!!! für Campuswoche anpassen
*/
//  const char* ssid = "FRITZ!Box 7490";

/* 
  Credentials FH-Dortmund FB4 Informatik Hardware 1 Labor
*/
 const char* ssid =         "hw1_gast";
 const char* password =     "KeineAhnung";
 const char* mqtt_server =  "dz-pi.hw1.fb4.fh";

/*
  Variables
*/
char msg[50];
long lastMsg    = 0;
int value       = 0;
bool sleep      = true;

/*
  Create Instances
*/
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

ADC_MODE(ADC_VCC);          // internal VCC to ESP

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (uint8_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  if ( (String)topic == (String) MQTT_SLEEP) {
  // Switch on the LED if an 1 was received as first character
    Serial.println("Sleep! ");
    if ((char)payload[0] == '1') {
      sleep = true;
    } else {
      sleep = false;
    }
  }
}

bool dataRecieve(void)
{
    byte data[9];
    uint8_t i=0;
    //transmit command data
    for(i=0; i<sizeof(cmd_get_sensor); i++)
    {
        sensor.write(cmd_get_sensor[i]);
    }
    delay(10);
    //begin reveiceing data
    if(sensor.available())
    {
        while(sensor.available())
        {
            for(i=0; i<9; i++)
            {
                data[i] = sensor.read();
                Serial.print(data[i]);
                Serial.print(" ");
            }
            Serial.println("");
        }
    }

    for(uint8_t j=0; j<9; j++)
    {
        Serial.print(data[j]);
        Serial.print(" ");
    }
    Serial.println("");

    if((i != 9) || (1 + (0xFF ^ (byte)(data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7]))) != data[8])
    {
      CO2PPM = (int)data[2] * 256 + (int)data[3];
      temperature = (int)data[4] - 40;
      return false;
    }

    CO2PPM = (int)data[2] * 256 + (int)data[3];
    temperature = (int)data[4] - 40;
    Serial.print("CO2-Konzentration: ");
    Serial.print(CO2PPM);
    Serial.println(" ppm");
    return true;
}


void setup_wifi(void) {
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Try to connect to: ");
  Serial.println(ssid);
 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266_Sensor1";
    if (client.connect(clientId.c_str())) {
      Serial.print(clientId);
      Serial.println(" connected");
      // client.subscribe(MQTT_OTA);
      client.subscribe(MQTT_SLEEP);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup_ota()
{  // Port defaults to 8266
  ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");
  ArduinoOTA.setHostname("ESP8266-Aussen");

  // No authentication by default
  ArduinoOTA.setPassword((const char *)"12345");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
   Serial.println("OTA Ready");
 }
 
void setup() 
{

  sensor.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.setTimeout(25);

  setup_wifi();
  setup_ota();
  
  Serial.println("");
  Serial.print("MAC: ");
  Serial.print(WiFi.macAddress());
  Serial.print("/topcis");
  Serial.println();

  // Wire.begin(4,5); // start I2C, define PINs

  // while (!tempsensor.begin()) {
  //   Serial.println("Couldn't find MCP9808!");
  //   delay(500);
  // }
      
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

//  Serial.println("\tRun = 0, \r\n\tConfig = 1!");
  delay(1000);
  lastMsg = millis();

  dht.setup(DHTPIN, DHTesp::DHTTYPE); // Connect DHT sensor to GPIO defined
}


void loop() 
{
  
  // ArduinoOTA.handle();
  delay(dht.getMinimumSamplingPeriod());
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  // if ((now - lastMsg) > 5000) 
  // {
    lastMsg = now;
    delay(250);
    // float celsius = tempsensor.readTempC();    // MCP9808 Temp-Sensor
    float humidity =    dht.getHumidity(); 
    float temperature = dht.getTemperature();

    if(dataRecieve())
    {
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.print("  CO2: ");
        Serial.print(CO2PPM);
        Serial.println("");
        snprintf (msg, 75, "%d", CO2PPM);
        client.publish(MQTT_CO2, msg, true);
    }

    int espVcc = ESP.getVcc();
    client.publish(MQTT_STATUS,"ALIVE", true);
    Serial.print("Voltage = ");
    Serial.print(espVcc);
    Serial.println(" mV"); 
    snprintf (msg, 75, "%d", espVcc);
    client.publish(MQTT_VOLT, msg, true);
    Serial.print("Feuchte = ");
    Serial.println(humidity, 1);
    snprintf (msg, 75, "%d", (int)(humidity + 0.5));
    client.publish(MQTT_HUM, msg, true);
    Serial.print("Temperature 1 = ");
    Serial.println(temperature, 1);
    // Serial.print("Temperature 2 = ");
    // Serial.println(celsius, 1);

    // snprintf (msg, 75, "%d", (int) (temperature + 0.5));
    // client.publish(MQTT_TEMP, msg, true);
    snprintf (msg, 75, "%d", (int)(temperature + 0.5));
    client.publish(MQTT_TEMP2, msg, true);
      
    if (sleep)
    {
      Serial.println("Deepsleep for 120 seconds");
      client.publish(MQTT_STATUS,"SLEEP", true);
      delay(100);
      ESP.deepSleep(120e6);
    }
  // } 
}



