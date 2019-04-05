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
#include <sys/shm.h>
#include <sys/ipc.h>
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
#include <zlib.h>

#ifdef _DEBUG
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#define MAX_SIZE 1024
#define SIZE 128
#define N 6
#define config "../Client/config/client.conf"

struct Client_Conf{
    char *Master_Ip;//Master_Ip地址
    char *Client_Ip;//Client_Ip地址
    char *Log_Dir;//日志文件目录
    char *Log_Backup;//日志文件备份目录
    char *Sys_Log;//系统日志文件目录
    int Master_Port;//Master端口
    int Heart_Port;//心跳端口
    int Ctrl_Port;//控制端口
    int Data_Port;//数据端口
    int Warn_Port;//警告端口
    int R_W_Times;//读写次数
    int Self_Test;//自测次数
    int File_Size;//文件大小
}conf;

typedef struct script{
    char *Name;//脚本名字
    char *Path;//脚本路径
    char *Log;//日志文件名字
    char *Back;//压缩文件名字
    char *BLog;//解压后的文件
    int Time;//执行时间
    int ID;//执行ID
}script;
script SC[N];//定义六个脚本结构体

struct shared{
    int cnt;     //检测次数
    int time;    //心跳次数
    pthread_mutex_t mutex;//互斥变量
    pthread_cond_t ready;//条件变量
};
struct shared *cond;//共享内存状态结构体

/*服务器使用 TCP 通讯*/
int socket_create_tcp(int port);

/*服务器使用 UDP 通讯*/
int socket_create_udp(int port);

/*客户使用 tcp 链接*/
int socket_connect_tcp(int port, char *host, int sec, double usec);

/*客户使用 UDP 链接*/
int send_warning_udp(int port, char *host, char *buff);

//获取配置文件数据：参数存储
int get_conf_argv(char *pathname, char* key_name, char *value);

//获取配置文件数据：指针
char *get_conf_value(const char *pathname, const char *key_name);

//写入日志数据
int write_log(char *pathname, const char *format, ...);

//发送文件
int send_file(int fd, char *filename);

//实现ntoa功能
char *my_inet_ntoa(struct in_addr in);

//心跳检测
bool heart_test(int port, char *host);

//文件压缩函数
int backup(char *filenames, char *backfilenames);

//获取文件大小
int file_size(char *filename);

//文件解压函数
int unback(char *backfilenames, char *filenames);

//字符判断函数
int strtok_func(char *buff, char *option, char *flag);
#endif
