#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <time.h>
#include <qstring.h>
#include <algorithm>
#include <list>
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <QFileDialog>
#include <qmessagebox.h>
#include <QDebug>
#include <QString>
#include <fstream>
#include <QThread>
#include <QDataStream>
#include <QByteArray>
#include <QTcpSocket>
#include <QFile>
#include <QFileInfo>
#include <QtCore>
#include <winsock2.h>
#include <QObject>
#include <malloc.h>

//#pragma comment(lib,"Ws2_32.lib")

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void append_cosole(QString str);
    void on_pushButton_start_clicked();
    void on_pushButton_open_dir_clicked();
    void on_pushButton_end_clicked();

signals:
    void send_ip(int ip);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
