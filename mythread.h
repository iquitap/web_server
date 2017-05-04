#ifndef MYTHREAD_H
#define MYTHREAD_H

#include "mainwindow.h"

#define DEFAULT_PORT 80
#define DEFAULT_IP 127<<24|1
#define DEFAULT_PATH "./"
#define THREAD_ERROR -1



class MyThread : public QThread
{
    Q_OBJECT
public:
    MyThread(MainWindow* parent=0,int ip = DEFAULT_IP, int port = DEFAULT_PORT, QString path = DEFAULT_PATH) : QThread(parent){
        this->parent = parent;
        this->ip = ip;
        this->port = port;
        this->path = path;
        connect(this, SIGNAL(myprint(QString)), parent, SLOT(append_cosole(QString)));
    }
    virtual ~MyThread() {}
    MainWindow  * parent;
    virtual void run();
    int ip;
    int port;
    SOCKET sListen, sAccept;
    QString path;

signals:
    void myprint(QString text);

public slots:
    void get_ip(int ip);
};


#endif // MYTHREAD_H
