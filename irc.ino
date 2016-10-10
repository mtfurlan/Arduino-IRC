#include <ESP8266WiFi.h>
#include "IRC.h"

//This contains WIFI_SSID and WIFI_PASS defines
#include "pass.h"


// https://github.com/JhonControl/ESP8266_Client_IRC_terminal_Serial

const char* host = "10.4.4.60";

WiFiClient client;

void setup() {

  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //WiFiClient* client;
  //char* host;
  //int port;
  //char* username;
  //char* realname;
  //char* nick;
  ircConfig conf = {
    client,
    "10.4.4.60",
    6667,
    "esp8266NodeMCU1",
    "Mark Furland",
    "Bot",
    "#test"
  };
  IRC ircClient(conf);
}



void loop() {
}

