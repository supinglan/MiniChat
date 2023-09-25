#ifndef LAB5_CLIENT_H
#define LAB5_CLIENT_H

#include <winsock2.h>
#include "define.h"
#include<String>
#include <map>
#include <iostream>

class Client {
public:
    int id;
    SOCKET tcps;
    sockaddr_in clientaddr;
    char* serverName;
    int ProcessReq(ReqPacket* req, SOCKADDR_IN *clientAddr);
    int SendRes(ResPacket* res);
    int SendIns(InsPacket* ins);
    int ReceiveReq( char *req);
    static int getTime(char * str);
};
extern std::map<int,Client> list;
extern char *serverName;

#endif //LAB5_CLIENT_H
