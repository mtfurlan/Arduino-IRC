#include "Arduino.h"
#include "IRC.h"

void IRC::init(ircConfig conf){
  _client = conf.client;
  _conf = conf;
}
void IRC::msgHandler(void (*callback)(ircMsg* msg), addressMode msgsToHandle){
  _msgHandler = callback;
  _msgsToHandle = msgsToHandle;
}
void IRC::loopHandler(void (*callback)()){
  _loopHandler = callback;
}
void IRC::sendMsg(ircMsg* newMsg){
  char buf[900];//TODO: resize
  sprintf(buf, "PRIVMSG %s :%s\r\n", newMsg->to, newMsg->msg);
  DEBUG_PRINT("Sending: ");
  DEBUG_PRINT(buf);
  _client.print(buf);
  free(newMsg);
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
  delay(800);
  sprintf(buf, "JOIN %s\r\n", _conf.chan);
  _client.print(buf);
  handle_irc_connection();
}


void IRC::handle_irc_connection() {
  char buf[900];//TODO: resize
  ircMsg curMsg;//Never gets deallocated
  char c;
  while(true) {
    if(_loopHandler != NULL){
      _loopHandler();
    }
    if (!_client.connected()) {
      return;
    }
    if(_client.available()) {
      c = _client.read();
    }else{
      continue;
    }

    if(c == ':') {
      memset(&curMsg, 0, sizeof(curMsg));//Necessary

      read_until(' ', curMsg.from);
      read_until(' ', curMsg.type);
      read_until(' ', curMsg.to);
      sprintf(buf, "%s %s %s\n", curMsg.type, curMsg.from, curMsg.to);
      DEBUG_PRINT(buf);

      /**
       * Known Bug list
       *
       * We use magic numbers for read_until
       * Buf scattered about
       **/

      if(strcmp(curMsg.type, "PRIVMSG") == 0) {
        if(_msgHandler != NULL){
          ignore_until(':');
          read_until('\r', curMsg.msg);
          DEBUG_PRINTLN("Got message");
          if(_msgsToHandle == ADDRESSED){
            //continue if not addressed
            //if the first strlen(nick) chars are the same, and the character after that is :
            if(strncmp(curMsg.msg, _conf.nick, strlen(_conf.nick)) != 0 || curMsg.msg[strlen(_conf.nick)] != ':'){
              DEBUG_PRINTLN("address mode failure");
              continue;
            }else{
              DEBUG_PRINTLN("address mode success");
              //Strip addressing from msg
              //Copy curMsg.msg + nick length + 2 to the start, and then set curMsg.msg length- (nick length + 2) to null.
              memmove(curMsg.msg, curMsg.msg + strlen(_conf.nick)+2, strlen(curMsg.msg)-(strlen(_conf.nick)+2));
              curMsg.msg[strlen(curMsg.msg)-(strlen(_conf.nick)+2)] = '\0';
            }
          }
          if(curMsg.to[0] == '#'){
            curMsg.pm = false;
          }else{
            curMsg.pm = true;
          }
          //Split nick from from
          strncpy(curMsg.nick,curMsg.from, strchr(curMsg.from,'!')-curMsg.from);

          _msgHandler(&curMsg);
        }
      }else if(strcmp(curMsg.type, "433") == 0){
        //nick in use, append _
        char* temp = _conf.nick;
        _conf.nick = (char*)malloc(strlen(_conf.nick)+1);
        _conf.nick[0] = '\0';
        strcat(_conf.nick,temp);
        strcat(_conf.nick,"_");
        strcat(_conf.nick,"\0");
        //TODO: sent "QUIT"
        return;//Go back to connection loop
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
