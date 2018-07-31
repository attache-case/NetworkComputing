#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>

#define URL_LEN        256

// URL の分析
int http_spliturl(char* url, char*host, char* path, int* port);
// HTTP サーバへの接続
int http_open(char* host, int port);
// HTTP サーバへ受信要求
void http_request(int s, char* host, char* path, int port);
