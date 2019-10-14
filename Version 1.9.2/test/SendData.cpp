// Sends data to Luftdaten.info
// ---------------------------------
//
//Uses functions from airrohr-firmware.ino
//Sven Theusz 25.02.2019

#include <string.h>
//#include <html-content.h>
#include <Wifi.h>

#define SOFTWARE_VERSION "NRZ-2018-123B" //CHANGE!!!!!!!!!!!!!!!
#define esp_chipid "" //CHANGE!

const char data_first_part[] = "{\"software_version\": \"{v}\", \"sensordatavalues\":[";

void sendLuftdaten(const String& data, const int pin, const char* host, const int httpPort, const char* url, const bool verify, const char* replace_str) {
	String data_4_dusti = data_first_part;
	data_4_dusti.replace("{v}", SOFTWARE_VERSION);
	data_4_dusti += data;
	data_4_dusti.remove(data_4_dusti.length() - 1);
	data_4_dusti.replace(replace_str, "");
	data_4_dusti += "]}";
	if (data != "") {
		sendData(data_4_dusti, pin, host, httpPort, url, verify, "", "application/json");
	}
}







/*****************************************************************
 * send data to rest api                                         *
 *****************************************************************/
void sendData(const String& data, const int pin, const char* host, const int httpPort, const char* url, const bool verify, const char* basic_auth_string, const String& contentType) {
//#include "ca-root.h"

	String request_head = F("POST ");
	request_head += String(url);
	request_head += F(" HTTP/1.1\r\n");
	request_head += F("Host: ");
	request_head += String(host) + "\r\n";
	request_head += F("Content-Type: ");
	request_head += contentType + "\r\n";
	if (strlen(basic_auth_string) != 0) {
		request_head += F("Authorization: Basic ");
		request_head += String(basic_auth_string) + "\r\n";
	}
	request_head += F("X-PIN: ");
	request_head += String(pin) + "\r\n";
	request_head += F("X-Sensor: esp8266-"); //CHANGE!!!!
	request_head += esp_chipid + "\r\n";
	request_head += F("Content-Length: ");
	request_head += String(data.length(), DEC) + "\r\n";
	request_head += F("Connection: close\r\n\r\n");

	const auto doConnect = [ = ](WiFiClient * client) -> bool {
		client->setNoDelay(true);
		client->setTimeout(20000);

		if (!client->connect(host, httpPort)) {
			//debug_out(F("connection failed"), DEBUG_ERROR, 1);
			return false;
		}
		return true;
	};

	const auto doRequest = [ = ](WiFiClient * client) {

		// send request to the server
		client->print(request_head);

		client->println(data);

		// wait for response
		int retries = 20;
		while (client->connected() && !client->available()) {
			delay(100);
			if (!--retries)
				break;
		}

		// Read reply from server and print them
		while(client->available()) {
			char c = client->read();
			
		}
		client->stop();
		
	};

	// Use WiFiClient class to create TCP connections // Eventuell rausnehmen?
	/*
	if (httpPort == 443) {
		WiFiClientSecure client_s;
		if (doConnect(&client_s)) {
			doRequest(&client_s);
		}

/*		WiFiClientSecure client_s;
		if (doConnect(&client_s)) {
			if (verify) {
				if (client_s.setCACert_P(dst_root_ca_x3_bin_crt, dst_root_ca_x3_bin_crt_len)) {
					if (client_s.verifyCertChain(host)) {
						debug_out(F("Server cert verified"), DEBUG_MIN_INFO, 1);
						doRequest(&client_s);
					} else {
						debug_out(F("ERROR: cert verification failed!"), DEBUG_ERROR, 1);
					}
				} else {
					debug_out(F("Failed to load root CA cert!"), DEBUG_ERROR, 1);
				}
			} else {
				doRequest(&client_s);
			}
		}
*/
/*		BearSSL::WiFiClientSecure client_s;
		if (verify) {
			BearSSLX509List cert(dst_root_ca_x3);
			client_s.setTrustAnchors(&cert);
		} else {
			client_s.setInsecure();
		}
		if (doConnect(&client_s)) {
			doRequest(&client_s);
		}
*/
/*
	} else {
		WiFiClient client;
		if (doConnect(&client)) {
			doRequest(&client);
		}
	}
	*/

	WiFiClient client;
		if (doConnect(&client)) {
			doRequest(&client);
		}

	 // nodemcu is alive
	yield();
}