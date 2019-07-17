#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <WiFi.h>
#include <HardwareSerial.h>

/*
#ifndef SENSOR_H
#define SENSOR_H
*/

class sensor{
    public:
    int sleep(boolean mode);
    int setPassive(boolean mode);
    int requestData();
    int setPeriod(int time);
    int getFirmware();
    void bufferClear();
    void sdsReadOnly();
    void sdsdebug();
    void DhtRead();
    void BmpRead();
    void setupSensor();
    void sensorData();
    void sensorloop();
    int getPm10();
    int getPm25();
    int getTemp();
};

//#endif