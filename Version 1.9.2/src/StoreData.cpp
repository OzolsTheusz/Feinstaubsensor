#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string>
#include <EEPROM.h>
#include <StoreData.h>
#include <Preferences.h>

#define EEPROM_SIZE 64 //Storage Size

static int data[7];
int dataSize = 7;
Preferences prefs;

int StoreData::setData(int *data) //Write data in storage
{
    Serial.println("Schreiben: ");
    prefs.begin("nvs", false);

    prefs.putInt("DHTTEMP", data[0]);
    prefs.putInt("DHTHUMI", data[1]);
    prefs.putInt("BMPTEMP", data[2]);
    prefs.putInt("BMPPRESS", data[3]);
    prefs.putInt("BMPALT", data[4]);
    prefs.putInt("PM10", data[5]);
    prefs.putInt("PM25", data[6]);

    for (int i = 0; i < 7; i++)
        {
         
            Serial.printf(" %i",data[i]); //Print stored data to console
        }
        Serial.println();
    //delay(1000); //EVTL WIEDER REINNEHMEN FALLS PROBLEME
    return 1;
}

int *StoreData::getData() //Get stored data from storage
{
    Serial.println("Lesen: ");
    data[0] = prefs.getInt("DHTTEMP",0);
    data[1] = prefs.getInt("DHTHUMI",1);
    data[2] = prefs.getInt("BMPTEMP",2);
    data[3] = prefs.getInt("BMPPRESS",3);
    data[4] = prefs.getInt("BMPALT",4);
    data[5] = prefs.getInt("PM10",5);
    data[6] = prefs.getInt("PM25",6);
    for (int i = 0; i < 7; i++)
        {
         
            Serial.printf("%i ",data[i]); //Print read data to console
        }
        Serial.println();
    //delay(1000); //EVTL WIEDER REINNEHMEN FALLS PROBLEME 
    return data;
}
