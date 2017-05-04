#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include "mainwindow.h"
#include "mythread.h"

#include <ctype.h>
#define BUF_LENGTH 1024
#define MIN_BUF 256
#define SERVER "Server: csr_http1.1\r\n"
#define ISspace(x) isspace((int)(x))
//#define qstr2char(x) x.toLatin1().data()
#define USER_ERROR -1

class ClientThread : public QThread
{
    Q_OBJECT
public:
    ClientThread(QObject *parent, MainWindow * ui, SOCKET sAccept, QString path = "./", DWORD* ThreadID = 0) : QThread(parent){
        this->path = path;
        path_initial = path;
        this->sAccept = sAccept;
        this->ThreadID = ThreadID;
        connect(this, SIGNAL(myprint(QString)), ui, SLOT(append_cosole(QString)));
    }
    virtual ~ClientThread() {}
    virtual void run();
    SOCKET sAccept;
    DWORD* ThreadID;
    QString path;
    QString path_initial;
    int file_not_found(SOCKET sAccept);
    int file_ok(SOCKET sAccept, long flen, QString file_type);
    int send_not_found(SOCKET sAccept);
    int send_unplemented(SOCKET sAccept);
    int send_file(SOCKET sAccept, FILE *resource);
    int web_file_func(QString file_path, SOCKET sAccept, QString method, int type_flag); //type_flag: 0--unknow, 1--txt(web)
    int create_index_html(QString fail_path);
    int create_folder_html(QString fail_path);
    const char * qstr2char(QString str);
    QString get_filetype_message(QString file_path);

signals:
    void myprint(QString text);

};
#endif // CLIENTTHREAD_H
