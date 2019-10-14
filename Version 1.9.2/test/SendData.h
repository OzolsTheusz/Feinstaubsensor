// Senddata.h 
// ---------------------------------
//
//Sending data to Luftdaten.info
//Sven Theusz 25.01.2019

#include <SoftwareSerial.h>

class SendData {
	public:
		
        void sendLuftdaten(const String& data, const int pin, const char* host, const int httpPort, const char* url, const bool verify, const char* replace_str);
        void sendData(const String& data, const int pin, const char* host, const int httpPort, const char* url, const bool verify, const char* basic_auth_string, const String& contentType)

		
		
};