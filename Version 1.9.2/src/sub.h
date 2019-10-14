#include <PubSubClient.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <StoreData.h>

class sub {
    public:
    //void callback(char* topic, byte* payload, unsigned int length);
    void reconnect();
    void setup_ota();
    void setupSub();
    void MQTTSend(void);
    void loopSub();
    void setup_wifiSub();
};