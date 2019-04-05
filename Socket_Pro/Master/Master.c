/*************************************************************************
	> File Name: Master.c
	> Author: zhengdongqi
	> Mail: 1821260963@qq.com
	> Created Time: 二  4/ 2 15:56:47 2019
 ************************************************************************/

#include "./node/node.h"
#include "./epoll/epoll.h"
#include "./common/common.h"
#include "./condition/condition.h"
#include "./threadpool/threadpool.h"

char *Req_Id[N] = {"Req_Id0", "Req_Id1", "Req_Id2", "Req_Id3", "Req_Id4", "Req_Id5"};
char *Yes_Id[N] = {"Yes_Id0", "Yes_Id1", "Yes_Id2", "Yes_Id3", "Yes_Id4", "Yes_Id5"};
char *filename[N] = {"filename0", "filename1", "filename2", "filename3", "filename4", "filename5"};

threadpool_t pool;
LinkedList linkedlist = NULL;

struct timers print_timer; //输出间隔时间
struct timers heart_timer; //心跳间隔时间
struct timers login_timer; //登陆间隔时间
struct timers ctrldata_timer; //数据传输间隔时间

int login_listen;
void user_ip_init(char *from, char *to) {
    struct in_addr from_addr;
    struct in_addr to_addr;
    inet_aton(from, &from_addr);
    inet_aton(to, &to_addr);
    unsigned int from_t = ntohl(from_addr.s_addr);
    unsigned int to_t = ntohl(to_addr.s_addr);
    for (unsigned int i = from_t; i <= to_t; i++) {
        struct in_addr q;
        q.s_addr = ntohl(i);
        struct sockaddr_in user_addr;
        user_addr.sin_addr = q;
        Node *node = node_create(user_addr);
        linkedlist = insert(linkedlist, node);
    }
    return ;
}
/*遍历输出*/
void *print_pthread(void *argv) {
    LinkedList p;
    p = linkedlist;
    output(p);
    return NULL;
}
/*用户登陆*/
void *login_pthread(void *argv) {
    do_epoll(linkedlist, login_listen);
    return NULL;
}
void *heart_pthread(void *argv) {
    LinkedList head = (LinkedList)argv;
    LinkedList p, q;
    
    struct timeval tm;//定义时间结构体
    tm.tv_sec = 0;
    tm.tv_usec = 500000;
    
    fd_set wset;
    FD_ZERO(&wset);
    int maxfd = 0;
    p = head;
    while (p != NULL && p->next != NULL) {
        int heart_fd;
        struct sockaddr_in heart_addr;
        //创建套接字
        if ((heart_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            DBG("heart_pthread->\033[31m创建套接字失败\033[0m");
            write_log(conf.Sys_Log, "heart_pthread->\033[31m创建套接字失败: %s\033[0m", strerror(errno));
            return NULL;
        }
        //设置套接字为非阻塞态
        int opt = 1;
        if (ioctl(heart_fd, FIONBIO, &opt) < 0) {
            DBG("heart_pthread->\033[31m设置非阻塞失败\033[0m");
            write_log(conf.Sys_Log, "heart_pthread->\033[31m设置非阻塞失败: %s\033[0m", strerror(errno));
            return NULL;
        }
            
        //设置服务器
        memset(&heart_addr, 0, sizeof(heart_addr));//数据初始化清零
        heart_addr.sin_family = AF_INET;//设置协议族
        heart_addr.sin_port = htons(conf.Heart_Port);//端口
        heart_addr.sin_addr.s_addr = inet_addr(inet_ntoa(p->next->addr.sin_addr));//IP地址
        //连接
        connect(heart_fd, (struct sockaddr *)&heart_addr, sizeof(heart_addr));
        FD_SET(heart_fd, &wset);
        //判断最大
        if(maxfd < heart_fd) {
            maxfd = heart_fd;
        }
        DBG("max : %d\n", maxfd);
        //放入集合
        p->next->fd = heart_fd;
        p = p->next;
    }
    usleep(500000);
    int nfd = select(maxfd + 1, NULL, &wset, NULL, &tm);
    if (nfd > 0) {
        q = head;
        while(q != NULL && q->next != NULL) {
            if (FD_ISSET(q->next->fd, &wset)) {
                int error = -1;
                int len = sizeof(int);
                getsockopt(q->next->fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);//套接字，层次，获取错误状态并清除，缓冲区，长度值
                if(error == 0) {
                    FD_CLR(q->next->fd, &wset);
                    close(q->next->fd);
                    q = q->next;
                } else {
                    DBG("%d\n", error);
                    DBG("heart_pthread->用户\033[34m%s fd->%d\033[0m已掉线\n", inet_ntoa(q->next->addr.sin_addr), q->next->fd);
                    FD_CLR(q->next->fd, &wset);
                    close(q->next->fd);
                    q->next->fd = -1;
                    linkedlist = delete_node(linkedlist, q->next->addr);
                    DBG("\033[34m用户已删除\033[0m\n");
                }
            } else {
                DBG("heart_pthread->用户\033[34m用户不在集合中%s fd->%d\033[0m已掉线\n", inet_ntoa(q->next->addr.sin_addr), q->next->fd);
                FD_CLR(q->next->fd, &wset);
                close(q->next->fd);
                linkedlist = delete_node(linkedlist, q->next->addr);
                DBG("\033[34m用户已删除\033[0m\n");
            }
        }
    } else {
        //DBG("heart_pthread->\033[31mselect失败: %s\033[0m\n", strerror(errno));
    }
    return NULL;
} 
/*心跳—数据检测*/
void *ctrl_data_pthread(void *argv) {
    struct sockaddr_in ctrl_data_addr = *(struct sockaddr_in *)argv;
    int ctrl_fd;
    if ((ctrl_fd = socket_connect_tcp(conf.Ctrl_Port, inet_ntoa(ctrl_data_addr.sin_addr), 0, 0.5)) < 0) {
        DBG("ctrl_data_pthread->\033[31m创建控制端口套接字失败：%s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "ctrl_data_pthread->\033[31m创建控制端口套接字失败：%s\033[0m", strerror(errno));
        return NULL;
    }
    for (int i = 0; i < N; i++) {
        DBG("ctrl_data_pthread->\033[32m任务 %d %s开始了\033[0m\n", i, inet_ntoa(ctrl_data_addr.sin_addr));
        write_log(conf.Sys_Log, "ctrl_data_pthread->\033[32m任务 %d %s开始了\033[0m", i, inet_ntoa(ctrl_data_addr.sin_addr));
        if (send(ctrl_fd, (const void*)&task_id[i].Req_Id, sizeof(task_id[i].Req_Id), 0) < 0) {
            DBG("ctrl_data_pthread->\033[31m发送请求失败\033[0m\n");
            write_log(conf.Sys_Log, "ctrl_data_pthread->\033[31m发送请求失败: %s\033[0m", strerror(errno));
            return NULL;
        }
        DBG("ctrl_data_pthread->\033[32m请求ID : %d\033[0m\n", task_id[i].Req_Id);
        write_log(conf.Sys_Log, "ctrl_data_pthread->\033[32m请求ID : %d\033[0m", task_id[i].Req_Id);
        int rec_id = 0;
        if (recv(ctrl_fd, (void *)&rec_id, sizeof(rec_id), 0) < 0) {
            DBG("ctrl_data_pthread->\033[31ms接收回复失败\033[0m\n");
            write_log(conf.Sys_Log, "ctrl_data_pthread->\033[31m接收回复失败: %s\033[0m", strerror(errno));
            continue;
        }
        DBG("ctrl_data_pthread->\033[32m比对ID 请求 %d: 回复 %d\033[0m\n", rec_id, task_id[i].Yes_Id);
        write_log(conf.Sys_Log, "ctrl_data_pthread->\033[32m比对ID 请求 %d: 回复 %d\033[0m", rec_id, task_id[i].Yes_Id);
        if (rec_id == task_id[i].Yes_Id) {
            DBG("ctrl_data_pthread->\033[32m存储操作\033[0m\n");
            int data_fd;
            char buffer[4 * MAX_SIZE] = {0};
            conf.Dir = get_conf_value(config, "Dir");
            DBG("ctrl_data_pthread->\033[32m文件夹路径：%s\033[0m\n", conf.Dir);
            write_log(conf.Sys_Log, "ctrl_data_pthread->\033[32m文件夹路径：%s\033[0m", conf.Dir);
            if ((data_fd = socket_connect_tcp(conf.Data_Port, inet_ntoa(ctrl_data_addr.sin_addr), 0, 0.5)) < 0) {
                DBG("ctrl_data_pthread->\033[31m连接数据端口失败\033[0m\n");
                write_log(conf.Sys_Log, "ctrl_data_pthread->\033[31m连接数据端口失败: %s\033[0m", strerror(errno));
                return NULL;
            }
            while (recv(data_fd, buffer, sizeof(buffer), 0) > 0) {
                DBG("ctrl_data_pthread->\033[32m接收文件了\033[0m\n");
                DBG("ctrl_data_pthread->\033[32m文件内容：\n%s\033[0m\n", buffer);
                strcat(conf.Dir, inet_ntoa(ctrl_data_addr.sin_addr));
                if (NULL == opendir(conf.Dir)) {
                    DBG("ctrl_data_pthread->\033[32m客户文件夹不存在，创建一个吧！%s\033[0m\n", conf.Dir);
                    mkdir(conf.Dir, 0777);
                }
                DBG("ctrl_data_pthread->\033[32m客户文件夹创建成功！\033[0m\n");
                strcat(conf.Dir, task_id[i].filename);
                DBG("ctrl_data_pthread->\033[32m客户日志路径：%s\033[0m\n", conf.Dir);
                FILE *log_file = fopen(conf.Dir, "a+");
                flock(log_file->_fileno, LOCK_EX);
                fwrite(buffer, sizeof(char), strlen(buffer), log_file);
                fclose(log_file);
                conf.Dir = NULL;
                memset(buffer, 0, sizeof(buffer));
                close(data_fd);
            }
        }
    }
    close(ctrl_fd);
    return NULL;
}
/*定时器管理者*/
void timers_manager() {
    
    print_timer.interval--;
    if(print_timer.interval == 0) {
        print_timer.interval = 3;//2s
        print_timer.handler();
    }
    heart_timer.interval--;
    if(heart_timer.interval == 0) {
        heart_timer.interval = 7;//5s
        heart_timer.handler();
    }
    login_timer.interval--;
    if(login_timer.interval == 0) {
        login_timer.interval = 13;
        login_timer.handler();
    }
    ctrldata_timer.interval--;
    if(ctrldata_timer.interval == 0) {
        ctrldata_timer.interval = 23;
        ctrldata_timer.handler();
    }
}
/*执行函数*/
void print() {
    threadpool_add_task(&pool, print_pthread, 0);
}
void heart() {
    threadpool_add_task(&pool, heart_pthread, linkedlist);
}
void login() {
    threadpool_add_task(&pool, login_pthread, 0);
}
void ctrldata() {
    LinkedList p;
    p = linkedlist;
    while (p != NULL && p->next != NULL) {
        DBG("ctrldata->\033[32m主人收数据了！喵\033[0m\n");
        threadpool_add_task(&pool, ctrl_data_pthread, &p->next->addr);
        p = p->next;
        DBG("ctrldata->\033[32m主人辛苦了！喵\033[0m\n");
    }
}
/*定时器初始化*/
void timers_init(int sec)  {
    struct itimerval base_tm;
    base_tm.it_value.tv_sec = sec;
    base_tm.it_value.tv_usec = 0;
    base_tm.it_interval = base_tm.it_value;
    setitimer(ITIMER_REAL, &base_tm, NULL);
    
    struct sigaction task;
    task.sa_handler = timers_manager;
    task.sa_flags = 0;
    sigemptyset(&task.sa_mask);
    sigaction(SIGALRM, &task, NULL);
    
    print_timer.interval = 3 * sec;
    print_timer.handler = print;
    
    heart_timer.interval = 7 * sec;
    heart_timer.handler = heart;
    
    login_timer.interval = 13 * sec;
    login_timer.handler = login;
    
    ctrldata_timer.interval = 23 * sec;
    ctrldata_timer.handler = ctrldata;
}

int main() {
    /*获取配置参数*/
    for (int i = 0; i < N; i++) {
        task_id[i].Req_Id = atoi(get_conf_value(config, Req_Id[i]));
        task_id[i].Yes_Id = atoi(get_conf_value(config, Yes_Id[i]));
        task_id[i].filename = get_conf_value(config, filename[i]);
    }
    conf.Master_Port = atoi(get_conf_value(config, "Master_Port"));
    conf.Heart_Port = atoi(get_conf_value(config, "Heart_Port"));
    conf.Ctrl_Port = atoi(get_conf_value(config, "Ctrl_Port"));
    conf.Data_Port = atoi(get_conf_value(config, "Data_Port"));
    conf.Warn_Port = atoi(get_conf_value(config, "Warn_Port"));
    conf.From = get_conf_value(config, "From");
    conf.To = get_conf_value(config, "To");
    conf.Sys_Log = get_conf_value(config, "Sys_Log");
    conf.INS = atoi(get_conf_value(config, "INS"));
    
    DB.database = get_conf_value(config, "database");
    DB.host = get_conf_value(config, "host");
    DB.user = get_conf_value(config, "user");
    DB.passwd = get_conf_value(config, "passwd");
    /*初始化头节点*/
    struct sockaddr_in init_addr;
    memset(&init_addr, 0, sizeof(struct sockaddr_in));
    linkedlist = head_node_init(init_addr, conf.Heart_Port);
	/*用户IP初始化*/
    user_ip_init(conf.From, conf.To);
    if ((login_listen = socket_create_tcp(conf.Master_Port)) < 0) {
        DBG("创建登陆套接字失败：%s\n", strerror(errno));
    }
    /*线程池初始化*/
    threadpool_init(&pool, 2, 100);
    /*添加队列*/
    timers_init(1);
    while(1);
    //摧毁线程池
    threadpool_destroy(&pool);
    return 0;
}


