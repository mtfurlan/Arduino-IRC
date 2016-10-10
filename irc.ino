#include <ESP8266WiFi.h>
#include "IRC.h"

//This contains WIFI_SSID and WIFI_PASS defines
#include "pass.h"


// https://github.com/JhonControl/ESP8266_Client_IRC_terminal_Serial

const char* host = "10.4.4.60";

WiFiClient client;
IRC ircClient;

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
  ircClient.init(conf);
  ircClient.onMsg(msgHandler);
  ircClient.begin();
}

char buf[901];

//This mallocs a thing, will be freed when sent
void msgHandler(ircMsg* msg){
  sprintf(buf, "%s: <%s> %s\n", msg->to, msg->from, msg->msg);
  Serial.print(buf);
  ircMsg* newMsg = (ircMsg*)malloc(sizeof(ircMsg));

  Serial.println(strlen(msg->to));

  if(msg->pm){
    strcpy(newMsg->to, msg->nick);
  }else{
    strcpy(newMsg->to, msg->to);
  }
  strcpy(newMsg->msg, "PONG");

  ircClient.sendMsg(newMsg);
}



//Not used
void loop() {}

