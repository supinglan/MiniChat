#include <sstream>
#include "client.h"
//处理请求报文
//tcps：控制连接套接字
//cilentAddr：指向客户端控制连接套接字地址
//返回值：0表示错误或者需要结束连接，1正常
int Client::ProcessReq( ReqPacket* req, SOCKADDR_IN *clientAddr)
{
    SOCKET dataSocket; //数据连接套接字
    ResPacket res;//回复报文
    InsPacket ins;
    std::map<int, Client>::iterator it;
    Client objective;
    std::ostringstream oss;
    //根据命令类型分派执行：
    switch(req->requestId)
    {
        case GETTIME:
            if(getTime(res.text))res.responseId = OK;
            else{
                res.responseId = ERR;
                strcpy(res.text, "Server eroor: can't get server time.\n");
            }
            if(!SendRes(&res)) return 0;
            break;
        case GETNAME:
            res.responseId = OK;
            strcpy(res.text,serverName);
            if(!SendRes( &res)) return 0;
            break;
        case GETCLIENTLIST:
            res.responseId = OK;
            oss<< "Client list:\n";
            for (it = list.begin(); it != list.end();it++) {
                std::string ipAddress = inet_ntoa(it->second.clientaddr.sin_addr);
                int port = ntohs(it->second.clientaddr.sin_port);
                oss << "ID: " << id;
                oss << " IP Address: " << ipAddress;
                oss << " Port: " << port<<"\n";
            }
            strcpy(res.text,oss.str().c_str());
            oss.clear();
            if(!SendRes(&res)) return 0;
            break;
        case SEND:
            if (list.count(id) > 0) {
                objective =list.at(id);
                ins.instructionId = MESSAGE;
                std::string ipAddress = inet_ntoa(clientaddr.sin_addr);
                int port = ntohs(clientaddr.sin_port);
                oss << "ID: " << id;
                oss << " IP Address: " << ipAddress;
                oss << " Port: " << port<<"\n";
                oss<<req->text;
                strcpy(res.text,oss.str().c_str());
                oss.clear();
                if(!objective.SendIns(&ins)){
                    res.responseId = ERR;
                    strcpy(res.text, "Error: The target client that sends the message is disconnected.\n");
                    break;
                }
                res.responseId = OK;
                strcpy(res.text, "Success\n");
            } else {
                res.responseId = ERR;
                strcpy(res.text, "Error: The target client for sending the message does not exist.\n");
            }
            SendRes( &res);
            break;
        case QUIT:
            printf("Client quit.\n");
            res.responseId = OK;
            strcpy(res.text, "Bye!\n");
            SendRes(&res);
    }
    return 1;
}
//发送回复报文：
int Client::SendRes( ResPacket* res)
{
    if(send(tcps, (char *)res, sizeof(ResPacket), 0) == SOCKET_ERROR)
    {
        printf("Lost the connection to client!\n");
        return 0;
    }
    return 1;
}
int Client::SendIns( InsPacket* ins)
{
    if(send(tcps, (char *)ins, sizeof(ResPacket), 0) == SOCKET_ERROR)
    {
        printf("Lost the connection to client!\n");
        return 0;
    }
    return 1;
}
//接收请求报文
//返回值：0表示错误或者连接已经断开，1表示正常
int Client::ReceiveReq( char *req) {
    int nRet;
    int left = sizeof(ReqPacket);

    while (left) {
        nRet = recv(tcps, req, left, 0);
        if (nRet == SOCKET_ERROR) {
            printf("Error occurs when receiving command from client!\n");
            return 0;
        }
        if (!nRet) {
            printf("Connection is closed by client!\n");
            return 0;
        }
        left -= nRet;
        req += nRet;
    }
    return 1;
}
int Client::getTime(char * str){
    SYSTEMTIME st ;
    int ret;
    GetSystemTime(&st);
    ret = sprintf(str,"Current Date and Time: %04d-%02d-%02d %02d:%02d:%02d\n",
                  st.wYear, st.wMonth, st.wDay,st.wHour, st.wMinute, st.wSecond);
    return ret;
}

