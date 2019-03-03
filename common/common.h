/*************************************************************************
	> File Name: common.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 六  2/23 11:12:28 2019
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>
#include <signal.h>
#include <math.h>

#ifdef _DEBUG
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

typedef struct Node{
    int fds;
    char name[20];
    struct Node *next;
}Node, *LinkedList;

int socket_create(int port);

int socket_connect(int port, char *host);

int get_conf_value(char *pathname, char* key_name, char *value);

LinkedList insert(LinkedList head, Node *node);

LinkedList delete_node(LinkedList head, char *name);

void clear(LinkedList head);

void client_name(LinkedList head, char *name);

int check_fds(LinkedList head, char *name);

int check_client(LinkedList head, char *name);

Node *node_create(int fds, char *name);

#endif
