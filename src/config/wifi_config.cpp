#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>

#include "epd_driver/epd.h"
//web 
#include "web/Web_Scripts.h" // JavaScript code
#include "web/Web_CSS.h"     // Cascading Style Sheets
#include "web/Web_HTML.h"    // HTML page of the tool

#include "config/wifi_config.h"

/* Server and IP address ------------------------------------------------------*/
WebServer server(80);
IPAddress myIP;

void handleNotFound() {
    // digitalWrite(led, 1);
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
    // digitalWrite(led, 0);
    Serial.println(message);
  }
  

void my_wifi_init() {
    Serial.println("start to config network.");
    Serial.printf("Connecting to %s\n", WIFI_SSID);

    WiFi.mode(WIFI_MODE_STA);

    // Applying SSID and password
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // Waiting the connection to a router
    int try_count = 0;
    int wait = WiFi.waitForConnectResult(10000);
    Serial.println(wait);
    if (wait != WL_CONNECTED) {
      Serial.println("wifi connected failed.");
      return;
    }

    myIP = WiFi.localIP();
    Serial.print("Local IP: ");
    Serial.println(myIP);

    // Connection is complete
    Serial.println("");

    Serial.println("WiFi connected");
    server.on("/", Web_SendHTML);
    server.on("/index.css", Web_SendCSS);
	  server.on("/Web_SendJS_A.js", Web_SendJS_A);

    server.on("/EPD", Srvr__postProc);
    server.on("/LOADA", Srvr__postProc);
    server.on("/LOADB", Srvr__postProc);
    server.on("/SHOW", Srvr__postProc);

    server.onNotFound(handleNotFound);

    // Start the server
    server.begin();
    Serial.println("Server started");

}

String get_local_ip() {
  return WiFi.localIP().toString();
}

String get_host_name() {
  return WiFi.getHostname();
}