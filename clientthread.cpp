#include "clientthread.h"
#include "mythread.h"
#include "mainwindow.h"
#include <QTextCodec>


// 发送404 file_not_found报头
int ClientThread::file_not_found(SOCKET sAccept)
{
    char send_buf[MIN_BUF];
//  time_t timep;
//  time(&timep);
    sprintf(send_buf, "HTTP/1.1 404 NOT FOUND\r\n");
    send(sAccept, send_buf, strlen(send_buf), 0);
//  sprintf(send_buf, "Date: %s\r\n", ctime(&timep));
//  send(sAccept, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "Connection: keep-alive\r\n");
    send(sAccept, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, SERVER);
    send(sAccept, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "Content-Type: text/html\r\n");
    send(sAccept, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "\r\n");
    send(sAccept, send_buf, strlen(send_buf), 0);
    return 0;
}

// 发送200 ok报头
int ClientThread::file_ok(SOCKET sAccept, long flen, QString file_type)
{
    char buffer[MIN_BUF];
    //  time_t timep;
    //  time(&timep);
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    send(sAccept, buffer, strlen(buffer), 0);
    sprintf(buffer, "Connection: keep-alive\r\n");
    send(sAccept, buffer, strlen(buffer), 0);
    sprintf(buffer, "Accept-Range:bytes\r\n");
    send(sAccept, buffer, strlen(buffer), 0);
    //  sprintf(buffer, "Date: %s\r\n", ctime(&timep));
    //  send(sAccept, buffer, strlen(buffer), 0);
    sprintf(buffer, SERVER);
    send(sAccept, buffer, strlen(buffer), 0);
    sprintf(buffer, "Content-Length: %ld\r\n", flen);
    send(sAccept, buffer, strlen(buffer), 0);
    //sprintf(buffer, "Content-Type:image/jpeg\r\n");
    sprintf(buffer, qstr2char(file_type));
    send(sAccept, buffer, strlen(buffer), 0);
    sprintf(buffer, "\r\n");
    send(sAccept, buffer, strlen(buffer), 0);

    return 0;
}

// 发送自定义的file_not_found页面
int ClientThread::send_not_found(SOCKET sAccept)
{
    char send_buf[MIN_BUF];
    sprintf(send_buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    send(sAccept, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "<BODY><h1 align='center'>404</h1><br/><h1 align='center'>file not found.</h1>\r\n");
    send(sAccept, send_buf, strlen(send_buf), 0);
    sprintf(send_buf, "</BODY></HTML>\r\n");
    send(sAccept, send_buf, strlen(send_buf), 0);
    return 0;
}

// 发送请求的资源
int ClientThread::send_file(SOCKET sAccept, FILE *file)
{
    char buffer[BUF_LENGTH];
    while (!feof(file))
    {
        memset(buffer, 0, sizeof(buffer));       //缓存清0
        int len = fread(buffer, sizeof(char), sizeof(buffer), file);
        if (len > 0) {
            if (SOCKET_ERROR == send(sAccept, buffer, len, 0)) {
                myprint("send() Failed:" + QString::number(WSAGetLastError(), 10)+ "\n" );
                return USER_ERROR;
            }
        }
    }
    return 0;
}

// 发送自定义的unplemented页面
int ClientThread::send_unplemented(SOCKET sAccept)
{
    char buf[1024];

    /* HTTP method 不被支持*/
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(sAccept, buf, strlen(buf), 0);
    /*服务器信息*/
    sprintf(buf, SERVER);
    send(sAccept, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(sAccept, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(sAccept, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(sAccept, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(sAccept, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(sAccept, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(sAccept, buf, strlen(buf), 0);

    return 0;
}

void ClientThread::run()
{

    qDebug() << "ClientThread run: ";
    qDebug() << "this: " << this->thread();
    qDebug() << "current: " << currentThread();

    myprint("client thread start running\n");

    char recv_buf[BUF_LENGTH];
    char method[MIN_BUF];
    char url[MIN_BUF];
    QString file_path;

    memset(recv_buf,0,sizeof(recv_buf));
    memset(method,0,sizeof(method));
    memset(url,0,sizeof(url));
    //memset(file_path,0,sizeof(file_path));

    if (recv(sAccept,recv_buf,sizeof(recv_buf),0) == SOCKET_ERROR)      //接收错误
    {
        myprint("recv() Failed:" + QString::number(WSAGetLastError(), 10) + "\n");
        return ;
    }
    else
    {
        myprint("recv data from client: \n" + QString(recv_buf) + "\n");           //接收成功，打印请求报文
    }

    //char * str = (char *)"HTTP/1.0 200 OK\nDate: Thu, 06 Aug 1998 12:00:15 GMT \nServer: Apache/1.3.0 (Unix) \nLast-Modified: Mon, 22 Jun 1998\nContent-Length: 50\nContent-Type: text/html\n\n\r\n fuck you!";
    //send(sAccept, str, strlen(str), 0);

    if(!strcmp(recv_buf,""))
        return ;

    //获得method
    int i, j;
    i = 0; j = 0;
    while (!ISspace(recv_buf[j]) && (i < sizeof(method) - 1))
    {
        method[i] = recv_buf[j];
        i++; j++;
    }
    method[i] = '\0';
    if (stricmp(method, "GET") && stricmp(method, "HEAD"))
    {
        send_unplemented(sAccept);
        closesocket(sAccept);                   //结束通信
        myprint("HTTP/1.0 501 Method Not Implemented\r\n\n");
        return ;
    }
    myprint("method: " + QString(method) + "\n");


    //获得url地址
    i = 0;
    while ((recv_buf[j] == ' ') && (j < sizeof(recv_buf))) {
        j++;
    }
    while (!(recv_buf[j] == ' ') && (i < sizeof(recv_buf) - 1) && (j < sizeof(recv_buf))) {
        if (recv_buf[j] == '/') {
            url[i] = '\\';
        }
        else if (recv_buf[j] == ' ') {
            break;
        }
        else {
            url[i] = recv_buf[j];
        }
        i++;
        j++;
    }
    url[i] = '\0';
    if (strcmp(url, "\\") == 1 && url[i - 1] == '\\') {
        url[i - 1] = '\0';
    }

    // 将请求的url路径转换为本地路径
    file_path = this->path;
    if(file_path.at(file_path.length()-1) == QChar('\\'))
        file_path = file_path.left(file_path.length() - 1);
    file_path += QString(url);

    myprint("path: " + file_path + "\n");
    myprint("url: " + QString(url) + "\n");

    if(!strcmp(url,"\\"))   //index
    {
        qDebug() << "want index";
        //if(file_path[file_path.length() - 1] == '/')    //目录
        if(create_index_html(file_path) == -1)
        {
            file_not_found(sAccept);
            // 如果method是GET，则发送404页面
            if(0 == stricmp(method, "GET"))
            {
                send_not_found(sAccept);
                myprint("send_not_found");
            }
            closesocket(sAccept); //结束通信
            myprint("file not found.\nclose socket.\n");

            return ;
        }
        else
        {
            file_path += QString("index.html");
            web_file_func(file_path,sAccept,QString(method), 1);

        }
    }
    else
    {
        QFileInfo fi(file_path);
        qDebug() << "file_path: " << file_path;
        qDebug() << "fi.exists: " << fi.exists();
        qDebug() << "fi.isfile: " << fi.isFile();
        qDebug() << "fi.isdir: " << fi.isDir();

        if(!fi.exists())
        {
            qDebug() << "file not exists";
            file_not_found(sAccept);
            // 如果method是GET，则发送404页面
            if(0 == stricmp(method, "GET"))
            {
                send_not_found(sAccept);
                myprint("send_not_found");
            }
            closesocket(sAccept); //结束通信
            myprint("file not found.\nclose socket.\n");

            return ;
        }
        else if(fi.isFile())   //文件
        {
            qDebug() << "file found!";
            web_file_func(file_path,sAccept,QString(method), 0);

        }
        else                    //文件夹
        {
            qDebug() << "folder found";
            if(create_folder_html(file_path) == -1)
            {
                file_not_found(sAccept);
                // 如果method是GET，则发送404页面
                if(0 == stricmp(method, "GET"))
                {
                    send_not_found(sAccept);
                    myprint("send_not_found");
                }
                closesocket(sAccept); //结束通信
                myprint("file not found.\nclose socket.\n");

                return ;
            }
            else
            {
                file_path += QString("\\folder.html");
                web_file_func(file_path,sAccept,QString(method),1);

            }
        }
    }
    closesocket(sAccept);

    return;
}

int ClientThread::create_index_html(QString file_path)
{
    QDir dir(file_path);
    QString  temp = file_path + QString("index.html");

    FILE *index_html;
    index_html = fopen(qstr2char(temp), "wb+ ");
    fprintf(index_html, "<!DOCTYPE HTML><html><head><meta http-equiv=\"Content - Type\" content=\"text / html; charset = utf-8\"><script>function cka(o){o.getElementsByTagName\a\")[0].click();}</script></head><body><style> tr{background-color:#ffffff;line-height:1.5}body{background-color:#CCFFCC;}a{TEXT-DECORATION:none}tbody tr:hover a{color:white; } tbody tr:hover td {background-color:#66CCCC;color:white} .thd tr {background-color:#339999;height:24px;border-bottom:1px solid #99CC99;line-height:1}tr{ background-color: expression((this.sectionRowIndex % 2 == 0) ? \"#fff\" : \"#e5ffe5\" ); } tr{ background: #fff;}tr:nth-child(2n){ background: #e5ffe5; } </STYLE>");
    fprintf(index_html, "<h3>/</h3>");
    fprintf(index_html, "<table style='width:101.5%;border-collapse: collapse;border-bottom:1px solid #888888;border-right:1px solid #cccccc' id='mtb'><thead Class='thd'><tr ><td >&nbsp;&nbsp;File_name/Name</td></tr></thead><tfoot Class='thd'><tr><td colspan='3'><div style='float:RIGHT;margin-right:10px'>MyWebServer/(By hust_iquit)&nbsp;</div></td></tr></tfoot>");
    foreach(QFileInfo mfi ,dir.entryInfoList())
    {
        //QString filename_temp = QString::fromUtf8(qstr2char(mfi.fileName()));
        fprintf(index_html, "<tr ondblclick=cka(this)><td ><a href='%s' >%s</a></td><td id=\"tright\"></td></tr>", qstr2char(QString(mfi.fileName())), qstr2char(mfi.fileName()));
        qDebug() << "中文测试1" << mfi.fileName();
        qDebug() << "中文测试2" << qstr2char(QString(mfi.fileName()));
    }
    fprintf(index_html, "</tbody></table></body></html>");
    fclose(index_html);
    return 0;
}

int ClientThread::create_folder_html(QString file_path)
{
    QDir dir(file_path);
    QString temp = file_path + QString("\\folder.html");
    FILE *index_html;
    index_html = fopen(qstr2char(temp), "wb+ ");

    QString str_href,str_href_fi;
    int index = file_path.indexOf(path_initial) + path_initial.length();
    str_href = file_path.right(file_path.length() - index - 1);
    qDebug() << "create_folder_html index: " << index;
    qDebug() << "create_folder_html str_href: " << str_href;

    QString folder_name;
    int last_slash_index = file_path.lastIndexOf(QChar('\\'));
    folder_name = file_path.right(file_path.length() - last_slash_index - 1);

    fprintf(index_html, "<!DOCTYPE HTML><html><head><meta http-equiv=\"Content - Type\" content=\"text / html; charset = utf-8\"><script>function cka(o){o.getElementsByTagName\a\")[0].click();}</script></head><body><style> tr{background-color:#ffffff;line-height:1.5}body{background-color:#CCFFCC;}a{TEXT-DECORATION:none}tbody tr:hover a{color:white; } tbody tr:hover td {background-color:#66CCCC;color:white} .thd tr {background-color:#339999;height:24px;border-bottom:1px solid #99CC99;line-height:1}tr{ background-color: expression((this.sectionRowIndex % 2 == 0) ? \"#fff\" : \"#e5ffe5\" ); } tr{ background: #fff;}tr:nth-child(2n){ background: #e5ffe5; } </STYLE>");
    fprintf(index_html, "<h3>/%s</h3>",qstr2char(folder_name));
    fprintf(index_html, "<table style='width:101.5%;border-collapse: collapse;border-bottom:1px solid #888888;border-right:1px solid #cccccc' id='mtb'><thead Class='thd'><tr ><td >&nbsp;&nbsp;File_name/Name</td></tr></thead><tfoot Class='thd'><tr><td colspan='3'><div style='float:RIGHT;margin-right:10px'>MyWebServer/(By hust_iquit)&nbsp;</div></td></tr></tfoot>");
    foreach(QFileInfo mfi ,dir.entryInfoList())
    {
        str_href_fi = str_href + "/" + mfi.fileName();
        fprintf(index_html, "<tr ondblclick=cka(this)><td ><a href='\\%s' >%s</a></td><td id=\"tright\"></td></tr>",qstr2char(str_href_fi), qstr2char(mfi.fileName()));
        qDebug() << "中文测试3" << str_href_fi;
        qDebug() << "中文测试4" << qstr2char(str_href_fi);
    }
    fprintf(index_html, "</tbody></table></body></html>");
    fclose(index_html);
    return 0;
}
int ClientThread::web_file_func(QString file_path, SOCKET sAccept, QString method, int type_flag) //type: 0--unknow, 1--txt(web)
{
    FILE *file_resource = fopen(qstr2char(file_path),"rb ");
    if(file_resource == NULL)
    {
        qDebug() << "file_path: " << file_path << "open file fail!";
        return -1;
    }
    else
        qDebug() << "open file success!";
    fseek(file_resource,0,SEEK_SET);
    fseek(file_resource,0,SEEK_END);
    int flen=ftell(file_resource);
    myprint("file length: " + QString::number(flen, 10) + "\n" );
    fseek(file_resource,0,SEEK_SET);

    if(type_flag==1)
        file_ok(sAccept, flen,"Content-Type: text/html\r\n");
    else
        file_ok(sAccept, flen,get_filetype_message(file_path));
    // 如果是GET方法则发送请求的资源
    if(method == "GET")
    {
        if(0 == send_file(sAccept, file_resource))
            myprint("file send ok.\n");
        else
        {
            myprint("file send fail.\n");
            return -1;
        }
    }
    fclose(file_resource);
    return 0;
}

QString ClientThread::get_filetype_message(QString file_path)
{
    QString file_type;
    char utemp[100],file_message[100];
    memset(utemp,0,sizeof(utemp));
    memset(file_message,0,sizeof(file_message));
    int last_dot_index = file_path.lastIndexOf(QChar('.'));
    file_type = file_path.right(file_path.length() - last_dot_index - 1);
//    qDebug() << "file_type: " << file_type;
//    qDebug() << "last_dot_index: " << last_dot_index;
//    qDebug() << "file_path.length: " << file_path.length();
    strcpy(utemp,qstr2char(file_type));

    if (strcmp(utemp, "html") == 0)
        strcpy(file_message, "Content-Type: text/html\r\n");
    else if (strcmp(utemp, "doc") == 0)
        strcpy(file_message, "Content-Type: application/msword\r\n");
    else if (strcmp(utemp, "gif") == 0)
        strcpy(file_message, "Content-Type: image/gif\r\n");
    else if (strcmp(utemp, "img") == 0)
        strcpy(file_message, "Content-Type: application/x-img\r\n");
    else if (strcmp(utemp, "java") == 0)
        strcpy(file_message, "Content-Type: java/*\r\n");
    else if (strcmp(utemp, "jpe") == 0)
        strcpy(file_message, "Content-Type: image/jpeg\r\n");
    else if (strcmp(utemp, "jpeg") == 0)
        strcpy(file_message, "Content-Type: image/jpeg\r\n");
    else if (strcmp(utemp, "jsp") == 0)
        strcpy(file_message, "Content-Type: text/html\r\n");
    else if (strcmp(utemp, "m2v") == 0)
        strcpy(file_message, "Content-Type: video/x-mpeg\r\n");
    else if (strcmp(utemp, "m4e") == 0)
        strcpy(file_message, "Content-Type: video/mpeg4\r\n");
    else if (strcmp(utemp, "mtx") == 0)
        strcpy(file_message, "Content-Type: text/xml\r\n");
    else if (strcmp(utemp, "image/pnetvue") == 0)
        strcpy(file_message, "Content-Type: image/pnetvue\r\n");
    else if (strcmp(utemp, "pdf") == 0)
        strcpy(file_message, "Content-Type: application/pdf\r\n");
    else if (strcmp(utemp, "pdx") == 0)
        strcpy(file_message, "Content-Type: application/vnd.adobe.pdx\r\n");
    else if (strcmp(utemp, "plg") == 0)
        strcpy(file_message, "Content-Type: text/html\r\n");
    else if (strcmp(utemp, "png") == 0)
        strcpy(file_message, "Content-Type: application/x-png\r\n");
    else if (strcmp(utemp, "ppa") == 0)
        strcpy(file_message, "Content-Type: application/vnd.ms-powerpoint\r\n");
    else if (strcmp(utemp, "pps") == 0)
        strcpy(file_message, "Content-Type: application/vnd.ms-powerpoint\r\n");
    else if (strcmp(utemp, "ppt") == 0)
        strcpy(file_message, "Content-Type: application/x-ppt\r\n");
    else if (strcmp(utemp, "rdf") == 0)
        strcpy(file_message, "Content-Type: text/xml\r\n");
    else if (strcmp(utemp, "tif") == 0)
        strcpy(file_message, "Content-Type: image/tiff\r\n");
    else if (strcmp(utemp, "avi") == 0)
        strcpy(file_message, "Content-Type: video/avi\r\n");
    else if (strcmp(utemp, "css") == 0)
        strcpy(file_message, "Content-Type: text/css\r\n");
    else if (strcmp(utemp, "dll") == 0)
        strcpy(file_message, "Content-Type: application/x-msdownload\r\n");
    else if (strcmp(utemp, "htm") == 0)
        strcpy(file_message, "Content-Type: text/html\r\n");
    else if (strcmp(utemp, "jpe") == 0)
        strcpy(file_message, "Content-Type: application/x-jpe\r\n");
    else if (strcmp(utemp, "jpg") == 0)
        strcpy(file_message, "Content-Type: image/jpeg\r\n");
    else if (strcmp(utemp, "js") == 0)
        strcpy(file_message, "Content-Type: application/x-javascript\r\n");
    else if (strcmp(utemp, "movie") == 0)
        strcpy(file_message, "Content-Type: video/x-sgi-movie\r\n");
    else if (strcmp(utemp, "mp3") == 0)
        strcpy(file_message, "Content-Type: audio/mp3\r\n");
    else if (strcmp(utemp, "rmvb") == 0)
        strcpy(file_message, "Content-Type: application/vnd.rn-realmedia-vbr\r\n");
    else if (strcmp(utemp, "torrent") == 0)
        strcpy(file_message, "Content-Type: application/x-bittorrent\r\n");
    else if (strcmp(utemp, "txt") == 0)
        strcpy(file_message, "Content-Type: text/plain\r\n");
    else if (strcmp(utemp, "svg") == 0)
        strcpy(file_message, "Content-Type: text/xml\r\n");
    else if (strcmp(utemp, "swf") == 0)
        strcpy(file_message, "Content-Type: application/x-shockwave-flash\r\n");
    else strcpy(file_message, "Content-Type: application/octet-stream\r\n");
    return QString(file_message);
}

const char * ClientThread::qstr2char(QString qstr)
{
    const char * ch = (char *)malloc(qstr.length()+2);
    QByteArray ba = qstr.toLocal8Bit();
    ch = ba.data();
    return ch;
}
