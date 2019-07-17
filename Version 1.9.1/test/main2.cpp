#include <Wire.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP085.h>
#include <DHT.h>
#include <WiFi.h>
#include <HardwareSerial.h>

#define DHTPIN 14
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE); //Config DHT
Adafruit_BMP085 bmp;      //Config  BMP
char sds_buffer[11];
byte sds_buffer_pos_w = 0;

HardwareSerial Serial1(1);
HardwareSerial Serial2(2);
//Netzwerk+++++++++++++++++++++++++++++
const char *ssid = "AlleDieMaikelHeissenSIndFetttttt";
const char *password = "19944986";
//Netzwerk+++++++++++++++++++++++++++++

//Var für Werte
unsigned int pm25;
unsigned int pm10;
unsigned int devId;
int dht_Temp;
unsigned int dht_Press;
unsigned int dht_Alt;
int bmp_Temp;
unsigned int bmp_Humidi;
byte checksum;
int anz;

/* Von Sven  (SDS)*/
int sleep(boolean mode)
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

int setPassive(boolean mode)
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

int requestData()
{
    byte request[] = {0xAA, 0xB4, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x10, 0xAB};
    for (byte i = 0; i < sizeof(request); i++)
        (Serial2.write(request[i]));

    return 0;
}

int setPeriod(int time)
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

int getFirmware()
{

    //get firmware version
    byte period[] = {0xAA, 0xB4, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x93, 0x79, 0x13, 0xAB};

    for (byte i = 0; i < sizeof(period); i++)
        (Serial2.write(period[i]));

    return 0;
}

void bufferClear()
{
    for (sds_buffer_pos_w = 0; sds_buffer_pos_w < 11; sds_buffer_pos_w++)
        sds_buffer[sds_buffer_pos_w] = 0x00;
    sds_buffer_pos_w = 0;
}
void sdsReadOnly()
{
    int wertAnz = 0;
    while (wertAnz < 10)
    {
        while (Serial2.available() && wertAnz < 10)
        {
            sds_buffer[wertAnz] = Serial2.read();
            wertAnz++;
        }
    }
    pm10 = ((sds_buffer[4] + (sds_buffer[5] << 8))) / 10;
    pm25 = ((sds_buffer[2] + (sds_buffer[3] << 8))) / 10;
    devId = (sds_buffer[6] + (sds_buffer[7] << 8));
    
    if((pm10+pm25)>200){
        Serial.print("[SDS011] Fehler!!!!");
        sdsReadOnly();
    }
}

void DhtRead()
{
    float h = dht.readHumidity();    //use the functions which are supplied by library.
    float t = dht.readTemperature(); // Read temperature as Celsius (the default)
    if (isnan(h) || isnan(t))
    { // Check if any reads failed and exit early (to try again).
        Serial.println("Failed to read from DHT sensor!");
    }
    Serial.printf("DHT Feuchtigkeit: %2.2f %c, Temp: %2.2f \n", h, 37, t);
    pinMode(5, PULLDOWN);
}
void BmpRead()
{
    
    Serial.printf("BMP Temp: %2.2f, Druck: %i, Höhe(Altitude): %2.2f \n", bmp.readTemperature(), bmp.readPressure(), bmp.readAltitude());
}
void setup_wifi(void)
{
    delay(10);

    WiFi.mode(WIFI_MODE_MAX);
    WiFi.begin(ssid, password);
    Serial.printf("Try to connect to: %s", ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    WiFi.mode(WIFI_MODE_STA);
}

void setup()
{
    //Test
    Serial1.begin(9600, SERIAL_8N1, 4, 2); //Baud rate, parity mode, RX, TX
    Serial2.begin(9600, SERIAL_8N1, 16, 17);

    bmp.begin();
    dht.begin();
    Serial.begin(9600);
    Serial2.begin(9600);

    setPassive(false);
    // setup_wifi();
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    DhtRead();
    delay(500);
    BmpRead();
    delay(500);
    sdsReadOnly();
    Serial.printf("pm10: %i, pm25: %i \n", pm10, pm25);
}
