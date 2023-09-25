#include <sstream>
#include <ctime>
#include <iomanip>
#include "client.h"

//处理请求报文
//tcps：控制连接套接字
//cilentAddr：指向客户端控制连接套接字地址
//返回值：0表示错误或者需要结束连接，1正常
int Client::ProcessReq(ReqPacket *req, SOCKADDR_IN *clientAddr) {
    SOCKET dataSocket; //数据连接套接字
    ResPacket res;//回复报文
    InsPacket ins;
    std::map<int, Client>::iterator it;
    Client objective;
    std::ostringstream oss;
    std::cout << "Receive request：" << req->requestId << std::endl;
    //根据命令类型分派执行：
    switch (req->requestId) {
        case GETTIME:
            if (getTime(res.text))res.responseId = OK;
            else {
                res.responseId = ERR;
                strcpy(res.text, "Server eroor: can't get server time.\n");
            }
            if (!SendRes(&res)) return 0;
            break;
        case GETNAME:
            res.responseId = OK;
            oss << serverName << std::endl;
            strcpy(res.text, oss.str().c_str());
            oss.clear();
            if (!SendRes(&res)) return 0;
            break;
        case GETCLIENTLIST:
            res.responseId = OK;
            oss << "Client list:\n";
            for (it = list.begin(); it != list.end(); it++) {
                std::string ipAddress = inet_ntoa(it->second.clientaddr.sin_addr);
                int port = ntohs(it->second.clientaddr.sin_port);
                oss << "ID: " << id;
                oss << " IP Address: " << ipAddress;
                oss << " Port: " << port << "\n";
            }
            strcpy(res.text, oss.str().c_str());
            oss.clear();
            if (!SendRes(&res)) return 0;
            break;
        case SEND:
            if (list.count(req->clientId) > 0) {
                objective = list.at(req->clientId);
                ins.instructionId = MESSAGE;
                std::string ipAddress = inet_ntoa(clientaddr.sin_addr);
                int port = ntohs(clientaddr.sin_port);
                oss << "Sender: " << std::endl;
                oss << "ID: " << id;
                oss << " IP Address: " << ipAddress;
                oss << " Port: " << port << " " << std::endl;
                oss << "Message:" << std::endl;
                oss << req->text << std::endl;
                strcpy(ins.text, oss.str().c_str());
                oss.clear();
                if (!objective.SendIns(&ins)) {
                    res.responseId = ERR;
                    strcpy(res.text, "Error: The target client that sends the message is disconnected.\n");
                    break;
                }
                res.responseId = OK;
                strcpy(res.text, "Send message success.\n");
            } else {
                res.responseId = ERR;
                strcpy(res.text, "Error: The target client for sending the message does not exist.\n");
            }
            SendRes(&res);
            break;
        case QUIT:
            std::cout << "Client quit." << std::endl;
            res.responseId = OK;
            strcpy(res.text, "Bye!\n");
            if (!SendRes(&res)) return 0;
            return 0;
    }
    return 1;
}

//发送回复报文：
int Client::SendRes(ResPacket *res) {
    if (send(tcps, (char *) res, sizeof(ResPacket), 0) == SOCKET_ERROR) {
        std::cout << "Lost the connection to client!" << std::endl;
        return 0;
    }
    return 1;
}

int Client::SendIns(InsPacket *ins) {
    if (send(tcps, (char *) ins, sizeof(ResPacket), 0) == SOCKET_ERROR) {
        std::cout << "Lost the connection to client!" << std::endl;
        return 0;
    }
    return 1;
}

//接收请求报文
//返回值：0表示错误或者连接已经断开，1表示正常
int Client::ReceiveReq(char *req) {
    int nRet;
    int left = sizeof(ReqPacket);

    while (left) {
        nRet = recv(tcps, req, left, 0);
        if (nRet == SOCKET_ERROR) {
            std::cout << "Error occurs when receiving request from client!" << std::endl;
            return 0;
        }
        if (!nRet) {
            std::cout << "Connection is closed by client!" << std::endl;
            return 0;
        }
        left -= nRet;
        req += nRet;
    }
    return 1;
}

int Client::getTime(char *str) {
    std::ostringstream oss;
    std::time_t currentTime = std::time(nullptr);
    std::tm *localTime = std::localtime(&currentTime);
    oss << "Time：";
    if (localTime == nullptr)return 0;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S") << std::endl;
    strcpy(str, oss.str().c_str());
    return 1;
}

