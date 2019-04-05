/*************************************************************************
	> File Name: common.h
	> Author: zhengdongqi
	> Mail: 1821260963@qq.com
	> Created Time: 二  4/ 2 15:57:11 2019
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#ifdef _DEBUG
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define config "../Master/config/master.conf"
#define MAX_SIZE 1024
#define SIZE 128
#define N 6

struct Master_Conf{
    char *From;//初始化用户起始IP地址
    char *To;//初始化用户结束IP地址
    char *Dir;//文件目录
    char *Sys_Log;//日志文件路径；
    int Master_Port;//Master端口
    int Heart_Port;//心跳端口
    int Ctrl_Port;//控制端口
    int Data_Port;//数据端口
    int Warn_Port;//警告端口
    int INS;//线程并发度
}conf;

typedef struct ID {
    int Req_Id;//请求ID
    int Yes_Id;//确认ID
    char *filename;//文件名字
}ID;
ID task_id[N];//初始化六个结构体

struct database {
    char *database;
    char *host;
    char *user;
    char *passwd;
}DB;

struct timers {
    int interval; //定时时间
    void (*handler)(); //处理函数
};

/*创建一个socket 类型TCP*/
int socket_create_tcp(int port);

/*创建一个socket 类型UDP*/
int socket_create_udp(int port);

/*进行 socket 连接 类型TCP*/
int socket_connect_tcp(int port, char *host, int sec, double usec);

/*进行 socket 连接 类型UDP*/
int socket_connect_udp(int port, char *host, char *buff);

/*获取配置文件数据：参数存储*/
int get_conf_argv(char *pathname, char* key_name, char *value);

/*获取配置文件数据：返回s指针*/
char *get_conf_value(const char *pathname, const char *key_name);

/*写入日志数据*/
int write_log(char *pathname, const char *format, ...);

/*发送文件*/
int send_file(int fd, char *file);

/*实现ntoa功能*/
char *my_inet_ntoa(struct in_addr in);

/*心跳检测*/
bool heart_test(int port, char *host);

/*定时器函数*/
void set_timer(int sec, double usec);

/*字符判断函数*/
int strtok_func(char *buff, char *option, char *flag);

/*提取字段函数*/
char *rss_func(char *buff, int ind, char *flag);

/*像数据中添加数据*/
int addto_database(struct database DB, char *time, char *ip, int types, char *details, char *table);

#endif
