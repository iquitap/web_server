#include "mythread.h"
#include "clientthread.h"

extern MyThread *server;
extern int startsig;
extern int stopsig;
extern int nomove;

void MyThread::run()
{
    qDebug() << "Mythread run: ";
    qDebug() << "this: " << this->thread();
    qDebug() << "current: " << currentThread();

    WSADATA wsaData;
    //SOCKET sListen,sAccept;                     //服务器监听套接字，连接套接字
    int serverport = this->port;                //服务器端口号
    struct sockaddr_in ser,cli;                 //服务器地址，客户端地址
    int iLen = sizeof(struct sockaddr_in);

    qDebug() << "serverport: " << serverport;

    myprint("-----------------------\n");
    myprint("Server waiting\n");
    myprint("-----------------------\n");

    //加载协议栈
    if (WSAStartup(MAKEWORD(2,2),&wsaData) !=0)
    {
        myprint("Failed to load Winsock." + QString::number(WSAGetLastError(), 10) + "\n");
        return ;
    }


    //创建监听套接字，监听客户请求
    sListen = socket(AF_INET,SOCK_STREAM,0);
    if (sListen == INVALID_SOCKET)
    {
        myprint("Failed to create socket:" + QString::number(WSAGetLastError(), 10) + "\n");
        return ;
    }

    //创建服务器地址：IP+端口号
    ser.sin_family = AF_INET;
    ser.sin_port = htons(serverport);   //调用htonl后四个字节颠倒顺序
    ser.sin_addr.S_un.S_addr = this->ip;    //htons(this->ip);
    if (bind(sListen,(LPSOCKADDR)&ser,sizeof(ser)) == SOCKET_ERROR)
    {
        myprint("Failed to bind:" + QString::number(WSAGetLastError(), 10) + "\n");
        return;
    }

    //监听
    if (listen(sListen,5)==SOCKET_ERROR)
    {
     myprint("Failed to listen:" + QString::number(WSAGetLastError(), 10) + "\n");
     return ;
    }

    //循环等待请求
    while(1)
    {
        sAccept = accept(sListen,(struct sockaddr*)&cli,&iLen);
        if (sAccept == INVALID_SOCKET)
        {
            myprint("Failed to accept:" + QString::number(WSAGetLastError(), 10) + "\n");
            break;
        }

        //创建多线程接受请求
        DWORD ThreadID;
        ClientThread * clientThread = new ClientThread(this, this->parent, sAccept, this->path, &ThreadID);
        clientThread->start();
    }
    closesocket(sListen);
    WSACleanup();
    return;
}

void MyThread::get_ip(int ip)
{
    this->ip = ip;
    return;
}

void MainWindow::on_pushButton_end_clicked()
{
    startsig = 0;
    if (stopsig == 0 && nomove == 0) {
        stopsig = 1;
        server->quit();
        server->wait(5);
        server->terminate();
        closesocket(server->sListen);
        closesocket(server->sAccept);
        delete server;
        WSACleanup();
        append_cosole("Serve end!");
    }
}
