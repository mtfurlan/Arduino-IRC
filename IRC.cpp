#include "Arduino.h"
#include "IRC.h"

IRC::IRC(ircConfig conf){
  _client = conf.client;
  _conf = conf;

  char buf[42];

  DEBUG_PRINTLN("IRC connecting ...");
  if (_client.connect(_conf.host, _conf.port)) {
    DEBUG_PRINTLN("connected");
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
  else {
    // if you didn't get a connection to the server:
    DEBUG_PRINTLN("IRC connection failed");
    delay(2000);
  }
}
void IRC::onMsg(void* callback){
}
void IRC::sendMsg(char* msg, char* channel){
}

#define IRC_BUFSIZE  32
char from[IRC_BUFSIZE];
char type[IRC_BUFSIZE];
char to[IRC_BUFSIZE];

void IRC::handle_irc_connection() {
  char c;
  // if there are incoming bytes available
  // from the server, read them and print them:
  while(true) {
    if (!_client.connected()) {
      return;
    }
    if(_client.available()) {
      c = _client.read();
    }
    else {
      continue;
    }

    if(c == ':') {
      memset(from, 0, sizeof(from));
      memset(type, 0, sizeof(type));
      memset(to, 0, sizeof(to));

      read_until(' ', from);
      read_until(' ', type);
      read_until(' ', to);

      if(strcmp(type, "PRIVMSG") == 0) {
        print_nick(from);
        ignore_until(':');
        print_until('\r');
      }else{

        ignore_until('\r');
      }
    }
    // could be a PING request by the server.
    else if (c == 'P') {
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
        DEBUG_PRINT("PING->PONG");
      }
    }
  } // end while

}

void IRC::print_nick(char buffer[]) {
  DEBUG_PRINT("<");
  for(int i = 0; i < IRC_BUFSIZE - 1; i++) {
    if(buffer[i] == '!') {
      break;
    }
    DEBUG_PRINT(buffer[i]);
  }
  DEBUG_PRINT(">");
}

int IRC::read_until(char abort_c, char buffer[]) {
  int bytes_read = 0;
  memset(buffer, 0, sizeof(buffer));
  for(int i = 0; i < IRC_BUFSIZE - 1; i++) {
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

// reads characters from the connection until
// it hits the given character.
void IRC::print_until(char c) {
  while(true){
    if (_client.available()) {
      char curr_c = _client.read();
      if (curr_c == c) {
        DEBUG_PRINT("");
        return;
      }
      DEBUG_PRINT(curr_c);
    }
  }
}



// reads characters from the connection until
// it hits the given character.
void IRC::print_until_endline() {
  while(true){
    if (_client.available()) {
      char curr_c = _client.read();
      if (curr_c == '\r') {
        curr_c = _client.read();
        if (curr_c == '\n') { return; }
      }
      DEBUG_PRINT(curr_c);
    }
  }
}
