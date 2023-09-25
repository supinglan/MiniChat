#include "define.h"
#include <stdio.h>
#include <winsock2.h>

#include "client.h"

//全局函数声明：
int Init(SOCKET *listenSock);
char* serverName = "server-1";
std::map<int,Client> list;
//线程函数，参数包括相应控制连接的套接字：
DWORD WINAPI ThreadFunc( LPVOID lpParam )
{
    SOCKET tcps;
    sockaddr_in clientaddr;
    Client * client = (Client*)lpParam;
    tcps = client->tcps;
    clientaddr = client->clientaddr;

    printf("socket id is %u.\n", tcps);

    //发送回复报文给客户端，内含命令使用说明：
    printf("Serve client %s:%d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
    ResPacket rspns= {OK,
                      "Welcome to Mini Chat Server (Windows Version)!\n"
    };
    ((Client*)lpParam)->SendRes(&rspns);

    //循环获取客户端命令报文并进行处理
    for(;;)
    {
        ReqPacket req;
        if(!client->ReceiveReq((char *) &req))
            break;
        if(!client->ProcessReq(&req, &clientaddr))
            break;
    }

    //线程结束前关闭控制连接套接字：
    list.erase(client->id);
    closesocket(tcps);
    delete lpParam;
    return 0;
}

int main(int argc, char* argv[])
{
    //服务器侦听套接字
    SOCKET listenSock;
    Client *client = NULL;
    int i =0;

    if(!Init(&listenSock))
        return 1;

    printf("Server listening on %d port...\n", SERVER_PORT);

    //循环接受客户端连接请求，并生成线程去处理
    for(;;)
    {
        client = new Client;
        if(client == NULL)
        {
            printf(" malloc space failed!\n");
            continue;
        }

        int len = sizeof(Client);
        //等待接受客户端控制连接请求
        client->tcps = accept(listenSock, (SOCKADDR*)&client->clientaddr, &len);
        client->id = i;
        list.insert(std::pair(i,*client));
        i++;

        //创建一个线程来处理相应客户端的请求：
        DWORD dwThreadId, dwThrdParam = 1;
        HANDLE hThread;

        hThread = CreateThread(
                NULL,                        // no security attributes
                0,                           // use default stack size
                ThreadFunc,                  // client function
                client,					// argument to client function
                0,                           // use default creation flags
                &dwThreadId);                // returns the client identifier

        // Check the return value for success.

        if (hThread == NULL)
        {
            printf("CreateThread failed.\n");
            closesocket(client->tcps);
            delete client;
        }
    }
    return 0;
}



//服务器初始化，创建一个侦听套接字：
int Init(SOCKET *listenSock)
{
    //startup->socket->bind->listen
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    SOCKET tcps_listen;

    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup( wVersionRequested, &wsaData );
    if (err!=0)
    {
        printf("Winsock初始化时发生错误!\n");
        return 0;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE( wsaData.wVersion ) != 2 )
    {
        WSACleanup();
        printf("无效Winsock版本!\n");
        return 0;
    }

    tcps_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(tcps_listen == INVALID_SOCKET)
    {
        WSACleanup();
        printf("创建Socket失败!\n");
        return 0;
    }
    SOCKADDR_IN tcpaddr;
    tcpaddr.sin_family=AF_INET;
    tcpaddr.sin_port=htons(SERVER_PORT);
    tcpaddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    err=bind(tcps_listen,(SOCKADDR*)&tcpaddr,sizeof(tcpaddr));
    if (err!=0)
    {
        err = WSAGetLastError();
        WSACleanup();
        printf("Socket绑定时发生错误!\n");
        return 0;
    }
    err=listen(tcps_listen, 3);
    if (err!=0)
    {
        WSACleanup();
        printf("Socket监听时发生错误!\n");
        return 0;
    }

    *listenSock = tcps_listen;
    return 1;
}




