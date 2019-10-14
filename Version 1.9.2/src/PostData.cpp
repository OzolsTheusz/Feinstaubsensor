// Sends data to Luftdaten.info using HTTP Post
// ---------------------------------
//
//
//Sven Theusz 05.03.2019

#include <WiFi.h>
#include <HTTPClient.h>
#include <PostData.h>
#include <StoreData.h>
#include <string>
#include "ArduinoJson.h"


#define SOFTWARE_VERSION "1.0" 
#define esp_chipid "esp32-1201974832" //ÄNDERN!!! EIGENE ID MUSS AUSGELESEN WERDEN

void PostData::postDataa(char sensortype) //Post data to luftdaten.info server
{
   if (WiFi.status() == WL_CONNECTED)
   {
      HTTPClient http;
      http.begin("http://api-rrd.madavi.de/data.php"); //Specify destination for HTTP request
      http.addHeader("Content-Type", "application/json"); //Specify contenttype = json

      StoreData dataStorage;
      int *data = dataStorage.getData(); //Get latest data from storage

      
      
         http.addHeader("X-Pin", "1"); //set type SDS011
      
      
         //http.addHeader("X-Pin", "3"); //set type BMP085
      
      
         http.addHeader("X-Pin", "7"); //set type DHT22
      
      
      
      http.addHeader("X-Sensor", ": esp32-1201974832"); //Specify serialnumber
      
      String jsonPayload = "DUMMY PAYLOAD"; //Initalize payload with dummystring
      
      
      //Set payload with data for luftdaten.info
      jsonPayload = String("{\"software_version\": \"") + SOFTWARE_VERSION + "\", \"sensordatavalues\":[{\"value_type\":\"SDS_P1\",\"value\":\"" + data[5] + "\"},{\"value_type\":\"SDS_P2\",\"value\":\"" + data[6] + "\"},{\"value_type\":\"DHT22_temperature\",\"value\":\"" + data[0] + "\"},{\"value_type\":\"humidity\",\"value\":\"" + data[1] + "\"}]}";
      
/* 
      if(sensortype == 'A'){
         jsonPayload = String("{\"software_version\": \"") + SOFTWARE_VERSION + "\", \"sensordatavalues\":[{\"value_type\":\"P1\",\"value\":\"" + data[5] + "\"},{\"value_type\":\"P2\",\"value\":\"" + data[6] + "\"}]}";
      }
      if(sensortype == 'B'){
         jsonPayload = String("{\"software_version\": \"") + SOFTWARE_VERSION + "\", \"sensordatavalues\":[{\"value_type\":\"temperature\",\"value\":\"" + data[2] + "\"},{\"value_type\":\"pressure\",\"value\":\"" + data[3] + "\"}]})";
      }
      if(sensortype == 'C'){
         jsonPayload = String("{\"software_version\": \"") + SOFTWARE_VERSION + "\", \"sensordatavalues\":[{\"value_type\":\"temperature\",\"value\":\"" + data[0] + "\"},{\"value_type\":\"humidity\",\"value\":\"" + data[1] + "\"}]})";
      }
      */
      
      int httpResponseCode = http.POST(jsonPayload); //Start http-post with payload
      

      if (httpResponseCode > 0)
      {
         String response = http.getString(); //Get the response to the request
         Serial.print("SENT JSON POST: ");
         Serial.println(httpResponseCode); //Print http-returncode to console
         Serial.println(response);
      }
      else
      {
         Serial.print("Error on sending POST: ");
         Serial.println(httpResponseCode); //If http-post sent bad returncode print to console 
      }
      http.end(); //Free resources
      
   }
   else
   {
      Serial.println("Error in WiFi connection");
   }

   //delay(5000); //EVTL WIEDER REINPACKEN FALLS PROBLEME BEIM SENDEN
}

