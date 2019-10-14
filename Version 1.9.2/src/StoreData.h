// SDS011 dust sensor PM2.5 and PM10
// ---------------------------------
//
//Control Functions for SDS011
//Sven Theusz 13.11.2018

#include <SoftwareSerial.h>

class StoreData{
	public:
        int setData(int* data);
        int* getData();
};