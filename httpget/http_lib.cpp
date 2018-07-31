#include "http_lib.h"

// URLの分析: URLから[ホスト名],[ファイル名],[ポート番号]の取得
int http_spliturl(char* url, char*host, char* path, int* port)
{
    char *p,host_path[URL_LEN];
    if ( strlen(url) > URL_LEN-1 ) {
        fprintf(stderr,"URLが長すぎます.\n"); return 1;
    }

    // http:// から始まる文字列で，その後に文字列が続く
    if ( strstr(url,"http://") && sscanf(url,"http://%s",host_path) ) {
        // ホスト名の取得
        if ( (p = strchr(host_path,'/')) != NULL ) {
            strcpy(path,p); *p = '\0'; strcpy(host,host_path);
        } else {
            strcpy(host,host_path);
        }
        // ポート番号の取得
        if ( (p = strchr(host,':')) != NULL ) {
            *port = atoi(p+1);
            *p = '\0';
        }
        if ( *port <= 0 ) *port = 80;
    } else {
        fprintf(stderr,"URLは [http://host/path] の形式で指定してください.\n");
        return 1;
    }
    return 0;
}

// HTTP サーバへの接続
int http_open(char* host,int port)
{
    int s;
    struct hostent *servhost;
    struct sockaddr_in server;
    struct servent *service;
    // ホストの情報を取得
    servhost = gethostbyname(host);
    if ( servhost == NULL ) {
        perror(host);
        return -1;
    }
    // IPアドレスを示す構造体をコピー
    bzero((char*)&server,sizeof(server));
    server.sin_family = AF_INET;
    bcopy(servhost->h_addr,(char*)&server.sin_addr,servhost->h_length);
    // ポート番号の設定
    if ( port != 0 ) {
        server.sin_port = htons(port);
    } else {
        service = getservbyname("http", "tcp");
        if ( service != NULL ) server.sin_port = service->s_port;
        else server.sin_port = htons(80);
    }
    // ソケット生成
    if ( (s = socket(AF_INET,SOCK_STREAM,0)) < 0 ) {
        perror("socket");
        return -1;
    }
    // サーバに接続
    if ( connect(s,(struct sockaddr *)&server,sizeof(server)) == -1 ) {
        perror("connect");
        return -1;
    }
    return s;
}

// HTTP サーバへ受信要求
void http_request(int s, char* host, char* path, int port)
{
    char send_buf[URL_LEN];
    // HTTP プロトコル生成 & サーバに送信
    sprintf(send_buf,"GET %s HTTP/1.0\r\n",path);
    write(s,send_buf,strlen(send_buf));
    sprintf(send_buf,"Host: %s:%d\r\n",host,port);
    write(s,send_buf,strlen(send_buf));
    sprintf(send_buf,"\r\n");
    write(s,send_buf,strlen(send_buf));
}
