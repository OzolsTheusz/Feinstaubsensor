#include <PubSubClient.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <SoftwareSerial.h>
#include <sub.h>


// Topics to Subscribe and to Publish
//ErsetzungsPunkt für MQTT
#define MQTT_Fein10   "42nibbles/MQTT/FEINSTAUB/Fein10"
#define MQTT_Fein25   "42nibbles/MQTT/FEINSTAUB/Fein25"
#define MQTT_FeinTEMP "42nibbles/MQTT/FEINSTAUB/FeinTemp"
#define MQTT_SLEEP    "42nibbles/MQTT/SLEEP"
 const char* ssid =         "hw1_gast"; //CHANGE!!!!!!!!!!!!!!!!
 const char* password =     "KeineAhnung"; //CHANGE!!!!!!!!!!!!
 const char* mqtt_server =  "dz-pi.hw1.fb4.fh"; //CHANGE!!!!!!!!!!!!!!!!!!
 //ErsetzungsPunkt für MQTT

/*
  Variables
*/
unsigned char dataRevice[9];
char msg[50];
long lastMsg    = 0;
int value       = 0;
//unsigned int sleep = 1;

WiFiClient espClient;
PubSubClient client(espClient);
StoreData store;

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
      //sleep = 1;
    } else {
      //sleep = 2;
    }
  }
}

void sub::setup_wifiSub() {
  delay(10);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println(ssid);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
  }
}

void sub::reconnect() {
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

void sub::setup_ota()
{  
  ArduinoOTA.setPort(1883);

  Serial.println("in ota");

  
  
  ArduinoOTA.setHostname("ESP32");

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
 
void sub::setupSub() 
{
  Serial.println("subsetup");

  //sensor.begin(9600);
  Serial.begin(9600);

  Serial.println();
  Serial.setTimeout(25);

  setup_wifiSub();
  setup_ota();
  
  Serial.println("");
  Serial.print("MAC: ");
  Serial.print(WiFi.macAddress());
  Serial.print("/topcis");
  Serial.println();
      
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

//  Serial.println("\tRun = 0, \r\n\tConfig = 1!");
  delay(1000);
  lastMsg = millis();

  //dht.setup(DHTPIN, DHTesp::DHTTYPE); // Connect DHT sensor to GPIO defined
}


void sub::loopSub() 
{
  // ArduinoOTA.handle();
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  long now = millis();
  // if ((now - lastMsg) > 5000)
  // {
  lastMsg = now;
  delay(250);

  client.publish(MQTT_FeinTEMP, msg, true);
  snprintf(msg, 75, "%d",(store.getData())[5]);
  Serial1.println("MQTT Erfolgreich");

  client.publish(MQTT_Fein25, msg, true);
  snprintf(msg, 75, "%d",(store.getData())[6]);

  client.publish(MQTT_Fein10, msg, true);
  snprintf(msg, 75, "%d",(store.getData())[0]);
}




