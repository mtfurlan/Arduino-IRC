#ifndef IRC_h
#define IRC_h

#include "Arduino.h"
#include <ESP8266WiFi.h>

#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINTLN(x)  Serial.println (x)
 #define DEBUG_PRINT(x)  Serial.print (x)
#else
 #define DEBUG_PRINTLN(x)
 #define DEBUG_PRINT(x)
#endif

typedef struct IRC_Config {
  WiFiClient client;
  char* host;
  int port;
  char* username;
  char* realname;
  char* nick;
  char* chan;
} ircConfig;

typedef struct IRC_Message {
  char to[200];
  char from[32];
  char type[32];
  char msg[510];
  bool pm;
} ircMsg;

class IRC{
  public:
    void init(ircConfig conf);
    void begin();
    //Returns a mallocd ircMsg, which is handled after the message is sent
    void onMsg(ircMsg* (*callback)(ircMsg* msg));
    //void loopHandler(ircMsg* (*callback)());
  private:
    WiFiClient _client;
    ircConfig _conf;
    ircMsg* (*_msgHandler)(ircMsg* msg);
    //ircMsg* (*_loopHandler)();


    void _connectionRegistration();

    void handle_irc_connection();
    int read_until(char abort_c, char buffer[]);
    void ignore_until(char c);
};

#endif
