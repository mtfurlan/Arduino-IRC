#include <ESP8266WiFi.h>
#include "IRC.h"

//This contains WIFI_SSID and WIFI_PASS defines
#include "pass.h"


int val = 0;
int oldVal = 0;
int inPin = D0;
int outPin = D1;

const char* host = "10.4.4.60";

WiFiClient client;
IRC ircClient;

void setup() {
  pinMode(inPin, INPUT_PULLUP);
  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, digitalRead(inPin));

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
  ircClient.msgHandler(msgHandler, ALL);
  ircClient.loopHandler(loopHandler);
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

//Actual loop
void loopHandler(){
  val = digitalRead(inPin);
  if(val != oldVal){
    //Change
    if(val){
      ircMsg* newMsg = (ircMsg*)malloc(sizeof(ircMsg));
      strcpy(newMsg->to, "#test");
      strcpy(newMsg->msg, "Pin went low");
      ircClient.sendMsg(newMsg);
    }
    oldVal = val;
  }
}

//Not used
void loop() {}

