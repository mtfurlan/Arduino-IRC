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

class IRC{
  public:
    IRC(ircConfig conf);
    void onMsg(void* callback);
    void sendMsg(char* msg, char* channel);
  private:
    WiFiClient _client;
    ircConfig _conf;

    void handle_irc_connection();
    void print_nick(char buffer[]);
    int read_until(char abort_c, char buffer[]);
    void ignore_until(char c);
    void print_until(char c);
    void print_until_endline();
};

#endif
