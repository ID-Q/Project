/*************************************************************************
	> File Name: client.cpp
	> Author: 
	> Mail: 
	> Created Time: 二 10/ 6 17:30:09 2020
 ************************************************************************/

#include "../lib/config.h"
#include "../lib/logger.h"
#include "../lib/network.h"
#include "../lib/heartbeat.h"
#include "../lib/document.h"

#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "stdio.h"
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/file.h>

#define CONFIG_PATH "./set.conf"
#define MAX_SIZE 1024
#define SIZE 128
#define N 6

struct Client_Conf{
    char * Master_Ip; //Master_Ip地址
    char *Client_Ip; //Client_Ip地址
    char *Log_Dir; //日志文件目录
    char *Log_Backup; //日志文件备份目录
    char *Sys_Log; //系统日志文件目录
    int Master_Port; //Master端口
    int Heart_Port; //心跳端口
    int Ctrl_Port; //控制端口
    int Data_Port; //数据端口
    int Warn_Port; //警告端口
    int R_W_Times; //读写次数
    int Self_Test; //自测次数
    int File_Size; //文件大小
} conf;

typedef struct shell {
    char *name; // 脚本名字
    char *path; // 脚本路径
    char *log; // 日志文件名字
    char *zip; // 压缩文件名字
    char *back; // 解压后的文件
    int time; // 执行时间
    int id; // 执行ID
} shell;
shell SH[N]; // 定义六个脚本结构体
std::vector<std::string> script{"Cpu", "Memory", "Disk", "Proc", "Sysinfo", "User"};
std::vector<std::string> add{"Name", "Path", "Time", "Log", "Log.zip", "Back.path"};

struct shared{
    int cnt; // 检测次数
    int time; // 心跳次数
    pthread_mutex_t mutex; // 互斥变量
    pthread_cond_t ready; // 条件变量
};
struct shared *cond; //共享内存状态结构体

char *share_memory = nullptr; // 共享内存首地址
double DyAver = 0; // 动态平均值
pthread_mutexattr_t m_attr; // 共享互斥属性
pthread_condattr_t c_attr; // 共享条件变量

int strtok_func(char *buff, char *option, char *flag) {
    char *p = strtok(buff, flag);
    p = strtok(NULL, flag);
    while (p) {
        if (strcmp(p, option) == 0) {
            return 1;
        }
        p = strtok(NULL, flag);
    }
    return 0;
}

void popen_script(int type) {
    Document f;
    char buffer[4 * MAX_SIZE] = {0};
    FILE *fstream = NULL;
    while(1) {
        for (int i = 0; i < conf.R_W_Times; i++) {
            if (type == 1) sprintf(SH[1].path, "%s %f", SH[1].path, DyAver);
            char buff[4 * SIZE] = {0};
            char test[4 * SIZE] = {0};
            if (NULL == (fstream = popen(SH[type].path, "r"))) {
                exit(1);
            }
            if ((100 + type) == SH[1].id) {
                if (NULL != fgets(buff, sizeof(buff), fstream)) {
                    strcat(buffer, buff);
                }
                if (NULL != fgets(buff, sizeof(buff), fstream)) {
                    DyAver = atof(buff);
                }else {
                    DyAver = 0;
                }
            } else {
                while (NULL != fgets(buff, sizeof(buff), fstream)) {
                    strcat(buffer, buff);
                    strcpy(test, buff);
                    if (strtok_func(test, "note\n", " ") || strtok_func(test, "warning\n", " ")) {
                        memset(test, 0, sizeof(test));
                        sprintf(test, "%d %s", 100 + type, buff);
                        UDP warn(conf.Warn_Port, conf.Master_Ip, 0, 0.5);
                        if (warn.socket_connect_udp(test) < 0) {
                        }
                        memset(buff, 0, sizeof(buff));
                    }
                }
            }
            sleep(SH[type].time);
            pclose(fstream);
            if ((100 + type) == SH[0].id) {
                fflush(stdout);
                pthread_mutex_lock(&cond->mutex);
                if (cond->cnt++ >= conf.Self_Test - 1) {
                    if (cond->time == 0) {
                        pthread_cond_signal(&cond->ready);
                    }
                    cond->cnt = 0;
                }
                pthread_mutex_unlock(&cond->mutex);
            }
        }
        FILE *file = fopen(SH[type].log, "a+");
        if (NULL == file) {
            exit(1);
        }
        //建立文件锁
        if (flock(file->_fileno, LOCK_EX) < 0) {
        }
        fwrite(buffer, 1, strlen(buffer), file);
        fclose(file);
        int len;
        if ((len = f.size(SH[type].log)) <  0) {
        }
        if (len >= (conf.File_Size * MAX_SIZE * MAX_SIZE)) {
            if (f.backup(SH[type].log, SH[type].zip) < 0) {
            }
        }
    }
}

int main() {
    Document f;
    //获取配置参数
    //循环获取脚本参数
    Config cset(CONFIG_PATH);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            SH[i].name = cset.read((script[i] + add[j]), SH[i].name);
            SH[i].id = 100 + i;
        }
    }
    //获取其它配置参数
    conf.Master_Ip = cset.read("Master_Ip", conf.Master_Ip); // 获取Master_Ip
    conf.Client_Ip = cset.read("Client_Ip", conf.Client_Ip); // 获取Client_Ip
    conf.Log_Dir = cset.read("Log_Dir", conf.Log_Dir); // 获取日志目录
    conf.Log_Backup = cset.read("Log_Backup", conf.Log_Backup); // 获取日志备份目录
    conf.Sys_Log = cset.read("Sys_Log", conf.Sys_Log); // 获取系统日志;
    conf.Master_Port = cset.read("Master_Port", conf.Master_Port); // 获取Master_Port
    conf.Heart_Port = cset.read("Heart_Port", conf.Heart_Port); // 获取心跳端口
    conf.Ctrl_Port = cset.read("Ctrl_Port", conf.Ctrl_Port); // 获取控制端口
    conf.Data_Port = cset.read("Data_Port", conf.Data_Port); // 获取数据端口
    conf.Warn_Port = cset.read("Warn_Port", conf.Warn_Port); // 获取警告端口
    conf.R_W_Times = cset.read("R_W_Times", conf.R_W_Times); // 获取读写次数
    conf.Self_Test = cset.read("Self_Test", conf.Self_Test); // 获取自检侧次数
    conf.File_Size = cset.read("File_Size", conf.File_Size); // 获取文件大小	

    int shmid; //设置共享内存；
    char *share_memory = nullptr; //分配的共享内存的原始首地址
    mkdir(conf.Log_Dir, 0755);
    mkdir(conf.Log_Backup, 0755);

    // 创建共享内存
    if ((shmid = shmget(IPC_PRIVATE, sizeof(struct shared), 0666|IPC_CREAT)) == -1) {
        return -1;
    }
    // 将共享内存连接到当前进程的地址空间
    if ((share_memory = (char *)shmat(shmid, 0, 0)) == NULL) {
        return -1;
    }

    cond = (struct shared*)share_memory;
    cond->cnt = 0; // 初始化心跳次数
    cond->time = 0; // 初始化检测次数
    pthread_mutexattr_init(&m_attr); // 初始化共享互斥属性
    pthread_condattr_init(&c_attr); // 初始化共享条件变量
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED); // 设置共享
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED); // 设置共享
    pthread_mutex_init(&cond->mutex, &m_attr); // 初始化锁
    pthread_cond_init(&cond->ready, &c_attr); // 初始化条件

    int pid_0; //父进程登陆
    int loginfd;
    char name[MAX_SIZE] = {0};
    char message[MAX_SIZE] = {0};

    TCP login(conf.Master_Port, conf.Master_Ip, 0, 0.5);
    loginfd = login.socket_create_tcp();
    gethostname(name, sizeof(name));
    send(loginfd, name, strlen(name), 0);
    if (recv(loginfd, (char *)&message, sizeof(message), 0) > 0) {
        printf("\033[35m服务端发送消息：\033[0m%s\n", message);
    }
    if ((pid_0 = fork()) < 0) {
        perror("fork");
        return -1;
    }

    // 父进程心跳监听
    if (pid_0 != 0) {
        int heart_listen;
        TCP heart(conf.Heart_Port, conf.Master_Ip, 0, 0.5);
        if ((heart_listen = heart.socket_create_tcp()) < 0) {
            return -1;
        }
        while(1) {
            int heart_fd;
            if ((heart_fd = accept(heart_listen, NULL, NULL)) < 0) {
                close(heart_fd);
            }
            //DBG("\033[35m⌛️ \033[0m");
            //fflush(stdout);
            close(heart_fd);
        }
    } else { // 儿子进程
        int pid_1;
        if ((pid_1 = fork()) < 0) {
            return -1;
        }
        if (pid_1 == 0) {
            HeartBeat hb(conf.Master_Port, conf.Master_Ip);
            while (1) {
                pthread_mutex_lock(&cond->mutex);
                pthread_cond_wait(&cond->ready, &cond->mutex);
                pthread_mutex_unlock(&cond->mutex);
                while(1) {
                    if (hb.heart_test()) {
                        pthread_mutex_lock(&cond->mutex);
                        cond->time = 0;
                        cond->cnt = 0;
                        pthread_mutex_unlock(&cond->mutex);
                        fflush(stdout);
                        break;
                    } else {
                        pthread_mutex_lock(&cond->mutex);
                        cond->time++;
                        pthread_mutex_unlock(&cond->mutex);
                        fflush(stdout);
                    }
                    sleep(6 * cond->time);
                    if (cond->time > conf.Self_Test) cond->time = conf.Self_Test;
                    pthread_mutex_unlock(&cond->mutex);
                }
            }
        } else { // 返回儿子进程
            int ID = 0;
            int pid_2;
            for (int i = 0; i < N; i++)  {
                ID = i;
                if ((pid_2 = fork()) < 0) {
                    continue;
                }
                if (pid_2 == 0) break;
            }
            if (pid_2 == 0) {
                popen_script(ID);
            } else {
                int ctrl_listen;
                TCP ctrl(conf.Ctrl_Port, conf.Master_Ip, 0, 0.5);
                if ((ctrl_listen = ctrl.socket_create_tcp()) < 0) {
                    return -1;
                }
                int data_listen;
                TCP data(conf.Data_Port, conf.Master_Ip, 0, 0.5);
                if ((data_listen = data.socket_create_tcp()) < 0) {
                    return -1;
                }
                while(1) {
                    int ctrl_fd;
                    if ((ctrl_fd = accept(ctrl_listen, NULL, NULL)) < 0) {
                        continue;
                    }
                    fflush(stdout);
                    int ctrl_id = 0;
                    while(recv(ctrl_fd, (void *)&ctrl_id, sizeof(ctrl_id), 0) > 0) {
                        for (int i = 0; i < N; i++) {
                            if (ctrl_id == SH[i].id) {
                                int flag = 0;
                                FILE *fp, *zp;
                                if ((zp = fopen(SH[i].zip, "r")) == NULL) {
                                    if ((fp = fopen(SH[i].log, "r")) == NULL) {
                                        flag = 400 + i;
                                        send(ctrl_fd, (const char *)&flag, sizeof(flag), 0);
                                    } else {
                                        flag = 200 + i;
                                        send(ctrl_fd, (const char *)&flag, sizeof(flag), 0);
                                        int data_fd;
                                        if ((data_fd = accept(data_listen, NULL, NULL)) < 0) {
                                            continue;
                                        }
                                        if (f.send_to(data_fd, SH[i].log) < 0) {
                                            continue;
                                        }
                                        if (remove(SH[i].log) < 0) {
                                            continue;
                                        }
                                        close(data_fd);
                                    }
                                } else {
                                    if (f.unback(SH[i].zip, SH[i].back) < 0) {
                                        continue;
                                    }
                                    if (remove(SH[i].zip) < 0) {
                                        continue;
                                    }
                                    flag = 200 + i;
                                    send(ctrl_fd, (const void *)&flag, sizeof(flag), 0);
                                    int data_fd;
                                    if ((data_fd = accept(data_listen, NULL, NULL)) < 0) {
                                        continue;
                                    }
                                    if (f.send_to(data_fd, SH[i].back) < 0) {
                                        continue;
                                    }
                                    if (remove(SH[i].back) < 0) {
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                    close(ctrl_fd);
                    pthread_mutex_lock(&cond->mutex);
                    cond->cnt = 0;
                    pthread_mutex_unlock(&cond->mutex);
                }
                close(ctrl_listen);
            }
        }
    }
    return 0;
}
