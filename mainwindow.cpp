#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mythread.h"

MyThread *server;

extern int startsig;
extern int stopsig;
extern int nomove;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
    ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QString("My web serve"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_start_clicked() //start
{
    ui->console->clear();
    stopsig = 0;
    nomove = 0;
    if (startsig == 0)
    {
        startsig = 1;
        QString str_console;

        QString str_ip[4];
        bool ok_ip[4];
        QString str_duankou = ui->lineEdit->text();
        QString str_path = ui->textBrowser->toPlainText();
        str_ip[0] = ui->lineEdit_2->text();
        str_ip[1] = ui->lineEdit_3->text();
        str_ip[2] = ui->lineEdit_4->text();
        str_ip[3] = ui->lineEdit_5->text();

        if(str_path.length() == 0  || str_duankou.length() == 0 || str_ip[0].length() == 0 || str_ip[1].length() == 0 || str_ip[2].length() == 0 || str_ip[3].length() == 0 ){
            QMessageBox::information(NULL, tr("Start"), tr("You intput illegal data."));
            return;
        }
        int int_ip[4];
        int int_duankou = str_duankou.toInt();
        int_ip[0] = str_ip[0].toInt(&ok_ip[0]);
        int_ip[1] = str_ip[1].toInt(&ok_ip[1]);
        int_ip[2] = str_ip[2].toInt(&ok_ip[2]);
        int_ip[3] = str_ip[3].toInt(&ok_ip[3]);
        //qDebug() << int_duankou << endl << int_ip1 << endl << int_ip2 << endl << int_ip3 << endl << int_ip4 << endl;
        if(int_duankou < 0 || ok_ip[0] == false || ok_ip[1] == false || ok_ip[2] == false || ok_ip[3] == false || int_ip[0] < 0 || int_ip[1] < 0 || int_ip[2] < 0 || int_ip[3] < 0 || int_ip[0] > 255 || int_ip[1] > 255 || int_ip[2] > 255 || int_ip[3] > 255 )
        {
            QMessageBox::information(NULL, tr("Start"), tr("You intput illegal data."));
            return;
        }
        else
        {
            int ip = int_ip[0] | int_ip[1] << 8 | int_ip[2] << 16 | int_ip[3] << 24;

            str_console = str_ip[0] + "." + str_ip[1] + "." + str_ip[2] + "." + str_ip[3];
            append_cosole(str_console);
            server =  new MyThread(this, ip, int_duankou, str_path);
            connect(this, SIGNAL(send_ip(int)), server, SLOT(get_ip(int)));
            emit send_ip(ip);
            server->start();
            return ;
        }
    }
    return;
}

void MainWindow::append_cosole(QString str)
{
    ui->console->append(str);
}




void MainWindow::on_pushButton_open_dir_clicked()  //open dir
{
    QString path = QFileDialog::getExistingDirectory(this,"请选择路径...","./");
    path.replace(QString("/"), QString("\\"));
    if(path.isEmpty())
    {
        QMessageBox::information(NULL, tr("Path"), tr("You didn't select any dir."));
        return;
    }
//    else{
//        QMessageBox::information(NULL, tr("Path"), tr("You selected ") + path);
//    }
    ui->textBrowser->setText(path);
}


