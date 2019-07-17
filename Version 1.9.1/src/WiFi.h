
#ifndef WiFi_h
#define WiFi_h

#include <stdint.h>

#include "Print.h"
#include "IPAddress.h"
#include "IPv6Address.h"

#include "WiFiType.h"
#include "WiFiSTA.h"
#include "WiFiAP.h"
#include "WiFiScan.h"
#include "WiFiGeneric.h"

#include "WiFiClient.h"
#include "WiFiServer.h"
#include "WiFiUdp.h"

class WiFiClass : public WiFiGenericClass, public WiFiSTAClass, public WiFiScanClass, public WiFiAPClass
{
public:
    using WiFiGenericClass::channel;

    using WiFiSTAClass::SSID;
    using WiFiSTAClass::RSSI;
    using WiFiSTAClass::BSSID;
    using WiFiSTAClass::BSSIDstr;

    using WiFiScanClass::SSID;
    using WiFiScanClass::encryptionType;
    using WiFiScanClass::RSSI;
    using WiFiScanClass::BSSID;
    using WiFiScanClass::BSSIDstr;
    using WiFiScanClass::channel;

public:
    void printDiag(Print& dest);
    friend class WiFiClient;
    friend class WiFiServer;
    friend class WiFiUDP;
};

extern WiFiClass WiFi;

#endif