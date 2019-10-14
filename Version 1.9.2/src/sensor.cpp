#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <WiFi.h>
#include <HardwareSerial.h>
#include <sensor.h>
#include <Storedata.h>

#define DHTPIN 14 //Pin to which DHT22 datapin is connected
#define DHTTYPE DHT22 //Decide between DHT11 and DHT22

DHT dht(DHTPIN, DHTTYPE); //Configure DHT
Adafruit_BMP085 bmp;      //Configure BMP
StoreData storage;

char sds_buffer[11];
byte sds_buffer_pos_w = 0;

//HardwareSerial Serial1(1); //FINAL RAUSNEHMEN
//HardwareSerial Serial2(2);    

int pm25;
int pm10;
unsigned int devId;
int dht_Temp;
unsigned int bmp_Press;
unsigned int bmp_Alt;
int bmp_Temp;
unsigned int dht_Humidi;
byte checksum;
int anz;
int data[7];

int sensor::sleep(boolean mode) //Send signal for sleep or work to SDS011
{
    //mode == true -> go to sleep
    //mode == false -> work

    byte sleep[] = {0xAA, 0xB4, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x13, 0xAB};
    byte work[] = {0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x14, 0xAB};

    if (mode)
    {
        Serial.println("a");
        for (byte i = 0; i < sizeof(sleep); i++)
            (Serial2.write(sleep[i]));
        return 0;
    }
    if (!mode)
    {
        for (byte i = 0; i < sizeof(work); i++)
            (Serial2.write(work[i]));
        return 0;
    }
    return 1;
}

int sensor::setPassive(boolean mode) //set SDS011 to active or passive mode
{
    //mode == true -> Data only when requested
    //mode == false -> Data gets sended automatically

    byte passive[] = {0xAA, 0xB4, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x10, 0xAB};
    byte active[] = {0xAA, 0xB4, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x0F, 0xAB};

    if (mode)
    {
        for (byte i = 0; i < sizeof(passive); i++)
            (Serial2.write(passive[i]));
        return 0;
    }
    if (!mode)
    {
        for (byte i = 0; i < sizeof(active); i++)
            (Serial2.write(active[i]));
        return 0;
    }
    return 1;
}

int sensor::requestData() //send datarequest to SDS011 (only in passive mode)
{
    byte request[] = {0xAA, 0xB4, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x10, 0xAB};
    for (byte i = 0; i < sizeof(request); i++)
        (Serial2.write(request[i]));

    return 0;
}

int sensor::setPeriod(int time) //Set working period of SDS011
{
    digitalWrite(LED_BUILTIN, LOW);
    Serial.printf("\nWarte  %i Zeiteinheiten", time);
    //work 30 seconds and sleep time*60-30 seconds (5th byte)
    //ACTUAL: SET PERIOD TO 30S

    int n = time;
    if (n > 30)
    {
        n = 30;
    }
    
    byte period[] = {0xAA, 0xB4, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x15, 0xAB};

    for (byte i = 0; i < sizeof(period); i++)
        (Serial2.write(period[i]));

    return 0;
}

int sensor::getFirmware() //Get firmwareversion of SDS011
{

    //get firmware version
    byte period[] = {0xAA, 0xB4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x13, 0xAB};

    for (byte i = 0; i < sizeof(period); i++)
        (Serial2.write(period[i]));

    return 0;
}
//Test
int sensor::getPm10(){
    return pm10;
}
int sensor::getPm25(){
    return pm25;
}
int sensor::getTemp(){
    return bmp_Temp;
}

void sensor::bufferClear() //Clear databuffer of SDS011
{
    for (sds_buffer_pos_w = 0; sds_buffer_pos_w < 11; sds_buffer_pos_w++)
        sds_buffer[sds_buffer_pos_w] = 0x00;
    sds_buffer_pos_w = 0;
}
void sensor::sdsReadOnly() //Read data from SDS011
{
    pm25 = 0;
    pm10 = 0;
    devId = 0;
    checksum = 0;
    int temp = 0;
    digitalWrite(LED_BUILTIN, LOW);

    if (Serial2.available())
    {
        sds_buffer_pos_w = 0;
        while ((temp = Serial2.read()) != 0xAA)
        {
            //Wait and listen for 0xAA
        }
        sds_buffer[sds_buffer_pos_w] = 0xAA;

        while (((temp = Serial2.read()) != 0xAB) && (temp != - 1)) //Write data in buffer while message hasn't ended
        {
            sds_buffer_pos_w++;
            sds_buffer[sds_buffer_pos_w] = temp;
        }
        sds_buffer_pos_w++;
        sds_buffer[sds_buffer_pos_w] = 0xAB;

        for(int i =0;i>=sds_buffer_pos_w;i++){
            Serial.printf("%i: %c \n",i,sds_buffer[i]);
        }
        delay(100);
        
        //Convert raw data from sds011 to pm2.5 and pm10 
        
        pm10 = ((sds_buffer[5]*256) + (sds_buffer[4])/10);
        pm25 = ((sds_buffer[3]*256) + (sds_buffer[2])/10);
        devId = (sds_buffer[6] + sds_buffer[7]);
        //Print read data to console
        Serial.printf("pm2.5:%i, pm10:%i \n",pm25,pm10);

        if(!(pm10>0&&pm10<90&&pm25>0&&pm25<90)){
            Serial.println("Werte falsch");
            delay(100);
            sdsReadOnly();
        }
    }
}

void sensor::DhtRead() //Read data from DHT
{
    dht_Humidi = dht.readHumidity();    //use the functions which are supplied by library.
    dht_Temp = dht.readTemperature(); // Read temperature as Celsius (default)
    if (isnan(dht_Humidi) || isnan(dht_Temp))
    { // Check if any reads failed and exit early (to try again).
        Serial.println("Failed to read from DHT sensor!");
    }
    //Print read data to console
    Serial.printf("DHT Feuchtigkeit: %2.2f, Temp: %2.2f \n", dht_Humidi,dht_Temp);
}

void sensor::BmpRead() //Read data from BMP085
{
    bmp_Temp=bmp.readTemperature();
    bmp_Press=bmp.readPressure();
    bmp_Alt=bmp.readAltitude();
    //Print read data to console
    Serial.printf("BMP Temp: %2.2f, Druck: %i, Höhe(Altitude): %2.2f \n", bmp.readTemperature(), bmp.readPressure(), bmp.readAltitude());
}

void sensor::setupSensor(){ //Initialize Sensors
    //HardwareSerial Serial1(1);
    //Serial1.begin(9600, SERIAL_8N1, 4, 2); //Baud rate, parity mode, RX, TX
    Serial2.begin(9600); //Setup badrate for SDS011 to 9600
    bmp.begin();
    dht.begin();
    //Set active Mode for SDS011
    sleep(false);
    setPassive(false);
}
void sensor::sensorData(){
   // (dht_Temp,dht_Humidi,bmp_Temp,bmp_Press,bmp_Alt,pm10,pm25)

   //DHT ist fehleranfällig
   if (dht_Temp==0){
       dht_Temp=bmp_Temp;
   }
    if((int)pm25&(int)pm10){
        int data[7]={dht_Temp,dht_Humidi,bmp_Temp,bmp_Press,bmp_Alt,pm10,pm25};
        storage.setData(data);

        storage.getData(); //Test if storing was successfull (getData prints to console)
    }
}

void sensor::sensorloop(){ //Read data of all sensors in loop
    BmpRead(); 
    DhtRead();
    sdsReadOnly(); 	
    //sensorData();
}