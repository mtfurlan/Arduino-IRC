#include <ESP8266WiFi.h>
#include "IRC.h"

//This contains WIFI_SSID and WIFI_PASS defines
#include "pass.h"


int val = 0;
int oldVal = 0;
volatile int inPin = D1;
int outPin = D0;

WiFiClient client;
IRC ircClient;

void setup() {

  pinMode(inPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(inPin), pinInterrupt, CHANGE);

  pinMode(outPin, OUTPUT);
  digitalWrite(outPin, LOW);

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
    "irc.sorcery.net",
    6667,
    "esp8266NodeMCU1",
    "Mark Furland",
    "ScuzzTestBot",
    "#scuzzBotTest"
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

  if(strcmp(msg->msg, "toggle") == 0){
    digitalWrite(outPin, !digitalRead(outPin));

    ircMsg* newMsg = (ircMsg*)malloc(sizeof(ircMsg));

    Serial.println(strlen(msg->to));

    if(msg->pm){
      strcpy(newMsg->to, msg->nick);
    }else{
      strcpy(newMsg->to, msg->to);
    }
    strcpy(newMsg->msg, "Toggle");

    ircClient.sendMsg(newMsg);
    return;
  }
  if(strcmp(msg->msg, "ping") == 0){
    ircMsg* newMsg = (ircMsg*)malloc(sizeof(ircMsg));

    Serial.println(strlen(msg->to));

    if(msg->pm){
      strcpy(newMsg->to, msg->nick);
    }else{
      strcpy(newMsg->to, msg->to);
    }
    strcpy(newMsg->msg, "PONG");

    ircClient.sendMsg(newMsg);
    return;
  }
}

void pinInterrupt(){
  val = digitalRead(inPin);
}

//Actual loop
void loopHandler(){
  if(val != oldVal){
      Serial.println("valChange");
    //Change
    if(!val){
      ircMsg* newMsg = (ircMsg*)malloc(sizeof(ircMsg));
      strcpy(newMsg->to, "#scuzzBotTest");
      sprintf(newMsg->msg, "Pin went %d", val);
      ircClient.sendMsg(newMsg);
    }
    oldVal = val;
  }
}

//Not used
void loop() {}

