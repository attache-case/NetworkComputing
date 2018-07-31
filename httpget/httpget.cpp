//-----------------------------------------------------------------
// HTTP クライアント (ソケット通信)
// % ./httpget [URL]
//-----------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "http_lib.h"

int main(int argc, char *argv[])
{
    int read_size,r_head;
    int s;                            // ソケットのためのファイルディスクリプタ
    int port = 0;                     // 接続するポート番号
    char host[URL_LEN] = "localhost"; // 接続するホスト名
    char path[URL_LEN] = "/";         // 要求するパス
    char buf[BUFSIZ];

    if ( argc != 2 ) {
        printf("usage: %s URL\n",argv[0]);
        return 0;
    }
    if ( http_spliturl(argv[1],host,path,&port) ) return 0;
    
    fprintf(stderr,"%s を取得します。\n\n",argv[1]);
    
    // サーバに接続
    if ( (s = http_open(host,port)) < 0 ) return 0;
    // サーバに受信要求
    http_request(s,host,path,port);
    
    // 受信＆ファイルに書き込み
    r_head = 1;
    FILE *fp;
    char filename[URL_LEN];
    char filenamebuf[URL_LEN];
    char* filenamepointer;
    strcpy(filenamebuf, path);
    filenamepointer = filenamebuf;
    while (filenamebuf != NULL) {
        strcpy(filename, filenamepointer);
        filenamepointer = strchr(filenamepointer, '/');
    }
    fp = fopen(filename, "w");
    while ( 1 ) {
        read_size = read(s,buf,BUFSIZ);
        if ( read_size <= 0 ) break;
        for ( int i=0; i<read_size; i++ ) {
            if ( r_head == 1 ) { // ヘッダ部の無視
                if ( !strncmp(buf+i,"\r\n\r\n",4) ) {
                    i += 3;
                    r_head = 0;
                }
            } else {
                if ( buf[i] != '\r' ) {fprintf(fp, "%c", buf[i]); write(1,buf+i,1);}
            }
        }
    }
    // 後始末
    fclose(fp);
    close(s);
    return 0;
}
