/*************************************************************************
 > File Name: Client.c
 > Author: zhengdognqi
 > Mail: 1821260963@qq.com
 > Created Time: 六  3/16 14:09:59 2019
     Name        Path       Time
 0   Cpu       CpuLog.sh      5
 1   Mem       MemLog.sh      5
 2   Disk      DiskLog.sh     60
 3   Proc      ProcLog.sh     30
 4   SysInfo   SysInfo.sh     60
 5   Users      Users.sh      60
 ************************************************************************/
#include "./common/common.h"

char *Name[N] = {"Name0", "Name1", "Name2", "Name3", "Name4", "Name5"};
char *Path[N] = {"Path0", "Path1", "Path2", "Path3", "Path4", "Path5"};
char *Time[N] = {"Time0", "Time1", "Time2", "Time3", "Time4", "Time5"};
char *Log[N] = {"Log0", "Log1", "Log2", "Log3", "Log4", "Log5"};
char *Back[N] = {"Back0", "Back1", "Back2", "Back3", "Back4", "Back5"};
char *BLog[N] = {"BLog0", "BLog1", "BLog2", "BLog3", "BLog4", "BLog5"};

char *share_memory = NULL;//共享内存首地址
double DyAver = 0;//动态平均值
pthread_mutexattr_t m_attr;//共享互斥属性
pthread_condattr_t c_attr;//共享条件变量

void popen_script(int type) {
    char buffer[4 * MAX_SIZE] = {0};
    FILE *fstream = NULL;
    while(1) {
        for (int i = 0; i < conf.R_W_Times; i++) {
            SC[1].Path = get_conf_value(config, "Path1");
            if (type == 1) sprintf(SC[1].Path, "%s %f", SC[1].Path, DyAver);
            char buff[4 * SIZE] = {0};
            char test[4 * SIZE] = {0};
            if (NULL == (fstream = popen(SC[type].Path, "r"))) {
                DBG("popen运行脚本失败：%s\n", strerror(errno));
                write_log(conf.Sys_Log, "popen_script->\033[31mpopen运行脚本失败: %s\033[0m", strerror(errno));
                exit(1);
            }
            if ((100 + type) == SC[1].ID) {
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
                        DBG("\033[32m发送警告信息: %s\033[0m\n", test);
                        if (socket_connect_udp(conf.Warn_Port, conf.Master_Ip, test) < 0) {
                            DBG("\033[31m发送警告信息失败: %s\033[0m\n", strerror(errno));
                            write_log(conf.Sys_Log, "popen_script->\033[31m发送警告信息失败: %s\033[0m", strerror(errno));
                        }
                        memset(buff, 0, sizeof(buff));
                    }
                }
            }
            sleep(SC[type].Time);
            pclose(fstream);
            if ((100 + type) == SC[0].ID) {
                DBG("\033[31m》 \033[0m");
                fflush(stdout);
                pthread_mutex_lock(&cond->mutex);
                if (cond->cnt++ >= conf.Self_Test - 1) {
                    if (cond->time == 0) {
                        DBG("popen_script->\n系统自检超过\033[33m%d\033[0m 次， Master 无连接\n", cond->cnt);
                        write_log(conf.Sys_Log, "popen_script->\033[31m系统自检超过 \033[33m%d\033[0m 次， Master 无连接\033[0m", cond->cnt);
                        pthread_cond_signal(&cond->ready);
                        DBG("popen_script->发送信号， 开启心跳程序 ❤️\n");
                        write_log(conf.Sys_Log, "popen_script->\033[33m发送信号， 开启心跳程序 ❤️\033[0m");
                    }
                    cond->cnt = 0;
                }
                pthread_mutex_unlock(&cond->mutex);
            }
        }
        FILE *file = fopen(SC[type].Log, "a+");
        if (NULL == file) {
            DBG("popen_script->\033[33m打开文件失败\033[0m\n");
            write_log(conf.Sys_Log, "popen_script->\033[31m打开文件失败：%s\033[0m", strerror(errno));
            exit(1);
        }
        //建立文件锁
        if (flock(file->_fileno, LOCK_EX) < 0) {
            DBG("popen_script->\033[33m建立文件锁失败\033[0m\n");
            write_log(conf.Sys_Log, "popen_script->\033[31m建立文件锁失败: %s\033[0m", strerror(errno));
        }
        fwrite(buffer, 1, strlen(buffer), file);
        fclose(file);
        int len;
        if ((len = file_size(SC[type].Log)) <  0) {
            DBG("popen_script->\033[33m获取文件大小失败\033[0m\n");
            write_log(conf.Sys_Log, "popen_script->\033[31m获取文件大小失败: %s\033[0m", strerror(errno));
        }
        if (len >= (conf.File_Size * MAX_SIZE * MAX_SIZE)) {
            DBG("popen_script->\033[33m文件过大，需要压缩\033[0m\n");
            DBG("popen_script->\033[33m压缩文件：%s\033[0m\n", SC[type].Back);
            if (backup(SC[type].Log, SC[type].Back) < 0) {
                DBG("popen_script->\033[33m压缩失败\033[0m\n");
                write_log(conf.Sys_Log, "popen_script->\033[31m压缩失败: %s\033[0m", strerror(errno));
            }
            DBG("popen_script->\033[33m压缩成功\033[0m\n");
        }
    }
}
int main() {
    //获取配置参数
    //循环获取脚本参数
    for (int i = 0; i < N; i++) {
        SC[i].Name = get_conf_value(config, Name[i]);//获取脚本名字
        SC[i].Path = get_conf_value(config, Path[i]);//获取脚本路径
        SC[i].Time = atoi(get_conf_value(config, Time[i]));//获取脚本运行间隔时间
        SC[i].Log = get_conf_value(config, Log[i]);//获取脚本日志名字
        SC[i].Back = get_conf_value(config, Back[i]);//获取压缩日志文件名字
        SC[i].BLog = get_conf_value(config, BLog[i]);//获取解压后的文件
        SC[i].ID = 100 + i;//脚本执行ID
    }
    //获取其它配置参数
    conf.Master_Ip = get_conf_value(config, "Master_Ip");//获取Master_Ip
    conf.Client_Ip = get_conf_value(config, "Client_Ip");//获取Client_Ip
    conf.Log_Dir = get_conf_value(config, "Log_Dir");//获取日志目录
    conf.Log_Backup = get_conf_value(config, "Log_Backup");//获取日志备份目录
    conf.Sys_Log = get_conf_value(config, "Sys_Log");//获取系统日志;
    conf.Master_Port = atoi(get_conf_value(config, "Master_Port"));//获取Master_Port
    conf.Heart_Port = atoi(get_conf_value(config, "Heart_Port"));//获取心跳端口
    conf.Ctrl_Port = atoi(get_conf_value(config, "Ctrl_Port"));//获取控制端口
    conf.Data_Port = atoi(get_conf_value(config, "Data_Port"));//获取数据端口
    conf.Warn_Port = atoi(get_conf_value(config, "Warn_Port"));//获取警告端口
    conf.R_W_Times = atoi(get_conf_value(config, "R_W_Times"));//获取读写次数
    conf.Self_Test = atoi(get_conf_value(config, "Self_Test"));//获取自检侧次数
    conf.File_Size = atoi(get_conf_value(config, "File_Size"));//获取文件大小
    
    int shmid;//设置共享内存；
    char *share_memory = NULL;//分配的共享内存的原始首地址
    mkdir(conf.Log_Dir, 0755);
    mkdir(conf.Log_Backup, 0755);
    
    //创建共享内存
    if ((shmid = shmget(IPC_PRIVATE, sizeof(struct shared), 0666|IPC_CREAT)) == -1) {
        DBG("shmget->\033[33m创建共享失败\033[0m\n");
        write_log(conf.Sys_Log, "shmget->\033[31m创建共享失败: %s\033[0m", strerror(errno));
        return -1;
    }
    //将共享内存连接到当前进程的地址空间
    if ((share_memory = (char *)shmat(shmid, 0, 0)) == NULL) {
        DBG("shmat->\033[33m共享内存连接失败\033[0m\n");
        write_log(conf.Sys_Log, "shmat->\033[31m共享内存连接失败: %s\033[0m", strerror(errno));
        return -1;
    }
    cond = (struct shared*)share_memory;
    cond->cnt = 0;//初始化心跳次数
    cond->time = 0;//初始化检测次数
    pthread_mutexattr_init(&m_attr);//初始化共享互斥属性
    pthread_condattr_init(&c_attr);//初始化共享条件变量
    pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);//设置共享
    pthread_condattr_setpshared(&c_attr, PTHREAD_PROCESS_SHARED);//设置共享
    pthread_mutex_init(&cond->mutex, &m_attr);//初始化锁
    pthread_cond_init(&cond->ready, &c_attr);//初始化条件
    
    int pid_0;//父进程登陆
    int loginfd;
    char name[MAX_SIZE] = {0};
    char message[MAX_SIZE] = {0};
    loginfd = socket_connect_tcp(conf.Master_Port, conf.Master_Ip, 0, 0.5);
    gethostname(name, sizeof(name));
    send(loginfd, name, strlen(name), 0);
    if (recv(loginfd, (char *)&message, sizeof(message), 0) > 0) {
        printf("\033[35m服务端发送消息：\033[0m%s\n", message);
    }
    write_log(conf.Sys_Log, "父进程->\033[33m父进程登陆\033[0m");
    if((pid_0 = fork()) < 0) {
        perror("fork");
        return -1;
    }
    //父进程心跳监听
    if (pid_0 != 0) {
        DBG("pid_0->\033[33m父进程创建心跳监听\033[0m\n");
        int heart_listen;
        if ((heart_listen = socket_create_tcp(conf.Heart_Port)) < 0) {
            DBG("pid_0->\033[33m创建心跳连接套接字\033[0m\n");
            write_log(conf.Sys_Log, "pid_0->\033[31m创建心跳连接套接字: %s\033[0m", strerror(errno));
            return -1;
        }
        while(1) {
            int heart_fd;
            if ((heart_fd = accept(heart_listen, NULL, NULL)) < 0) {
                DBG("pid_0->\033[33maccept心跳连接套接字\033[0m\n");
                write_log(conf.Sys_Log, "pid_0->\033[31maccept心跳连接套接字: %s\033[0m", strerror(errno));
                close(heart_fd);
            }
            //DBG("\033[35m⌛️ \033[0m");
            //fflush(stdout);
            close(heart_fd);
        }
        write_log(conf.Sys_Log, "父进程->\033[33m父进程心跳监听\033[0m");
    } else {//儿子进程
        int pid_1;
        if ((pid_1 = fork()) < 0) {
            DBG("pid_1->\033[33m创建儿子进程失败\033[0m\n");
            write_log(conf.Sys_Log, "pid_1->\033[31m创建儿子进程失败: %s\033[0m", strerror(errno));
            return -1;
        }
        if (pid_1 == 0) {
            while (1) {
                pthread_mutex_lock(&cond->mutex);
                DBG("孙子进程等待信号开启心跳\n");
                pthread_cond_wait(&cond->ready, &cond->mutex);
                DBG("获得心跳信号, 开始心跳 \033[34m(◐‿◑)\033[0m\n");
                pthread_mutex_unlock(&cond->mutex);
                while(1) {
                    if (heart_test(conf.Master_Port, conf.Master_Ip)) {
                        DBG("\n 第 %d 次: ❤️  \n", cond->time);
                        pthread_mutex_lock(&cond->mutex);
                        cond->time = 0;
                        cond->cnt = 0;
                        pthread_mutex_unlock(&cond->mutex);
                        fflush(stdout);
                        break;
                    } else {
                        DBG("\n 第 %d 次：💔 \n", cond->time);
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
        write_log(conf.Sys_Log, "儿子进程->\033[31m心跳测试\033[0m");
        } else {//返回儿子进程
            DBG("->\033[33m返回儿子进程\033[0m\n");
            int ID = 0;
            int pid_2;
            for(int i = 0; i < N; i++)  {
                ID = i;
                if((pid_2 = fork()) < 0) {
                    DBG("pid_2->\033[33m创建孙子进程失败\033[0m\n");
                    write_log(conf.Sys_Log, "儿子进程->\033[31Error in pid_2: %s\033[0m", strerror(errno));
                    continue;
                }
                if(pid_2 == 0) break;
            }
            if (pid_2 == 0) {
                popen_script(ID);
                write_log(conf.Sys_Log, "儿子进程->\033[31m运行脚本%d\033[0m", ID);
            } else {
                int ctrl_listen;
                if ((ctrl_listen = socket_create_tcp(conf.Ctrl_Port)) < 0) {
                    DBG("父进程->\033[31m创建控制监听套接字失败\033[0m\n");
                    write_log(conf.Sys_Log, "父进程->\033[31m创建控制监听套接字失败\033[0m", strerror(errno));
                    return -1;
                }
                int data_listen;
                if ((data_listen = socket_create_tcp(conf.Data_Port)) < 0) {
                    DBG("父进程->\033[31m创建数据监听套接字失败\033[0m\n");
                    write_log(conf.Sys_Log, "父进程->\033[31m创建数据监听套接字失败\033[0m", strerror(errno));
                    return -1;
                }
                write_log(conf.Sys_Log, "父进程->\033[31m数据传输\033[0m");
                while(1) {
                    int ctrl_fd;
                    if ((ctrl_fd = accept(ctrl_listen, NULL, NULL)) < 0) {
                        DBG("父进程->\033[31m接收控制监听套接字失败\033[0m\n");
                        write_log(conf.Sys_Log, "父进程->\033[31m接收控制监听套接字失败\033[0m", strerror(errno));
                        continue;
                    }
                    DBG("\033[35mctrl👌\033[0m\n");
                    fflush(stdout);
                    int ctrl_id = 0;
                    while(recv(ctrl_fd, (void *)&ctrl_id, sizeof(ctrl_id), 0) > 0) {
                        DBG("父进程->\033[33m接受到ID\033[0m\n");
                        DBG("ctrl_id : %d\n", ctrl_id);
                        for (int i = 0; i < N; i++) {
                            if (ctrl_id == SC[i].ID) {
                                int flag = 0;
                                FILE *fp, *zp;
                                if ((zp = fopen(SC[i].Back, "r")) == NULL) {
                                    DBG("没有压缩文件\n");
                                    if ((fp = fopen(SC[i].Log, "r")) == NULL) {
                                        flag = 400 + i;
                                        send(ctrl_fd, (const char *)&flag, sizeof(flag), 0);
                                        DBG("文件不存在\n");
                                    } else {
                                        flag = 200 + i;
                                        send(ctrl_fd, (const char *)&flag, sizeof(flag), 0);
                                        int data_fd;
                                        if ((data_fd = accept(data_listen, NULL, NULL)) < 0) {
                                            DBG("父进程->\033[31m接收数据监听套接字失败\033[0m\n");
                                            write_log(conf.Sys_Log, "父进程->\033[33m接收数据监听套接字\033[0m: %s\033[0m", strerror(errno));
                                            continue;
                                        }
                                        DBG("\033[35mdata👌\033[0m\n");
                                        if (send_file(data_fd, SC[i].Log) < 0) {
                                            DBG("父进程->\033[31m发送数据文件失败\033[0m\n");
                                            write_log(conf.Sys_Log, "父进程->\033[31m发送数据文件失败\033[0m: %s\033[0m", strerror(errno));
                                            continue;
                                        }
                                        if (remove(SC[i].Log) < 0) {
                                            DBG("Error in remove: %s\n", strerror(errno));
                                            write_log(conf.Sys_Log, "儿子进程->\033[31Error in remove: %s\033[0m", strerror(errno));
                                            continue;
                                        }
                                        close(data_fd);
                                    }
                                } else {
                                    DBG("有压缩文件\n");
                                    if (unback(SC[i].Back, SC[i].BLog) < 0) {
                                        DBG("Error in unback: %s", strerror(errno));
                                        write_log(conf.Sys_Log, "儿子进程->\033[31Error in remove: %s\033[0m", strerror(errno));
                                        continue;
                                    }
                                    if (remove(SC[i].Back) < 0) {
                                        DBG("Error in remove: %s\n", strerror(errno));
                                        write_log(conf.Sys_Log, "儿子进程->\033[31Error in remove: %s\033[0m", strerror(errno));
                                        continue;
                                    }
                                    flag = 200 + i;
                                    send(ctrl_fd, (const void *)&flag, sizeof(flag), 0);
                                    int data_fd;
                                    if ((data_fd = accept(data_listen, NULL, NULL)) < 0) {
                                        DBG("父进程->\033[31m接收数据监听套接字失败\033[0m\n");
                                        write_log(conf.Sys_Log, "父进程->\033[33m接收数据监听套接字\033[0m: %s\033[0m", strerror(errno));
                                        continue;
                                    }
                                    DBG("\033[35mdata👌\033[0m\n");
                                    DBG("发送压缩文件\n");
                                    if (send_file(data_fd, SC[i].BLog) < 0) {
                                        DBG("父进程->\033[31m发送数据文件失败\033[0m\n");
                                        write_log(conf.Sys_Log, "父进程->\033[31m发送数据文件失败\033[0m: %s\033[0m", strerror(errno));
                                        continue;
                                    }
                                    if (remove(SC[i].BLog) < 0) {
                                        DBG("Error in remove: %s\n", strerror(errno));
                                        write_log(conf.Sys_Log, "儿子进程->\033[31Error in remove: %s\033[0m", strerror(errno));
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                    DBG("结束了\n");
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

