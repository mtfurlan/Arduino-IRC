#include "Arduino.h"
#include "IRC.h"

void IRC::init(ircConfig conf){
  _client = conf.client;
  _conf = conf;
}
void IRC::onMsg(ircMsg* (*callback)(ircMsg* msg)){
  _msgHandler = callback;
}
void sendMsg(ircMsg* msg){
}

void IRC::begin(){
  while(true){
    DEBUG_PRINTLN("IRC connecting ...");
    if (_client.connect(_conf.host, _conf.port)) {
      DEBUG_PRINTLN("connected");
      _connectionRegistration();
    }else{
      // if you didn't get a connection to the server:
      DEBUG_PRINTLN("IRC connection failed");
      delay(2000);
    }
  }
}



void IRC::_connectionRegistration(){
  char buf[42];
  delay(1000);
  sprintf(buf, "USER %s 0 * :%s\r\n", _conf.username, _conf.realname);
  _client.print(buf);
  delay(500);
  sprintf(buf, "NICK %s\r\n", _conf.nick);
  _client.print(buf);
  delay(500);
  sprintf(buf, "JOIN %s\r\n", _conf.chan);
  _client.print(buf);
  delay(500);
  handle_irc_connection();
}


void IRC::handle_irc_connection() {
  char buf[900];//TODO: resize
  ircMsg currMsg;//Never gets deallocated
  char c;
  while(true) {
    if (!_client.connected()) {
      return;
    }
    if(_client.available()) {
      c = _client.read();
    }else{
      continue;
    }

    if(c == ':') {
      memset(&currMsg, 0, sizeof(currMsg));//Is this necessary?

      read_until(' ', currMsg.from);
      read_until(' ', currMsg.type);
      read_until(' ', currMsg.to);
      sprintf(buf, "%s %s %s\n", currMsg.type, currMsg.from, currMsg.to);
      DEBUG_PRINT(buf);

      /**
       * Known Bug list
       *
       * Crashes on QUIT messages
       * We use magic numbers for read_until
       **/

      if(strcmp(currMsg.type, "PRIVMSG") == 0) {
        ignore_until(':');
        read_until('\r', currMsg.msg);
        DEBUG_PRINTLN("Got message");
        if(currMsg.to[0] == '#'){
          currMsg.pm = false;
        }else{
          currMsg.pm = true;
        }
        ircMsg* newMsg = _msgHandler(&currMsg);
        sprintf(buf, "PRIVMSG %s :%s\r\n", newMsg->to, newMsg->msg);
        DEBUG_PRINT("Sending: ");
        DEBUG_PRINT(buf);
        _client.print(buf);
        free(newMsg);
      }else if(strcmp(currMsg.type, "433") == 0){
        //nick in use, append _
        char* temp = _conf.nick;
        _conf.nick = (char*)malloc(strlen(_conf.nick)+1);
        _conf.nick[0] = '\0';
        strcat(_conf.nick,temp);
        strcat(_conf.nick,"_");
        return _connectionRegistration();
      }else{//Some other error, probably
        ignore_until('\r');
      }
    }else if (c == 'P') {
      // could be a PING request by the server.
      char buf[5];
      memset(buf, 0, sizeof(buf));
      buf[0] = c;
      for(int i = 1; i < 4; i++) {
        c = _client.read();
        buf[i] = c;
      }
      ignore_until('\r');
      if(strcmp(buf, "PING") == 0) {
        _client.print("PONG\r\n");
        DEBUG_PRINTLN("PING->PONG");
      }
    }
    //Not : or P, probably don't care.
  } // end while
}

int IRC::read_until(char abort_c, char* buffer) {
  int bytes_read = 0;
  memset(buffer, 0, 32);//TODO: NOT HARDCODE SIZES
  for(int i = 0; i < 31; i++) {
    if (_client.available()) {
      char c = _client.read();
      bytes_read++;
      if(c == abort_c) {
        return bytes_read;
      }
      else if (c == '\n') {
        return bytes_read;
      }
      buffer[i] = c;
    }
  }
  ignore_until(abort_c);
  return bytes_read;
}

// reads characters from the connection until
// it hits the given character.
void IRC::ignore_until(char c) {
  while(true){
    if (_client.available()) {
      char curr_c = _client.read();
      if (curr_c == c) {
        return;
      }
    }
  }
}
