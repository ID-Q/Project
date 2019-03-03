/*************************************************************************
	> File Name: Server.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 五  2/22 12:39:00 2019
 ************************************************************************/
#include "../common/common.h"

#define MAX_SIZE 1024
#define SIZE 20

struct Message {
    char name[20];//储存客户名称
    int flag;
    char message[MAX_SIZE];
};

struct Server_Conf {
    char Server_Port[SIZE];
    char Clinet_Port[SIZE];
}conf;

int sock_fd;
int sock_cd;
int client_num = 0;
//struct sockaddr_in client_addr;
LinkedList linkedlist = NULL;
char *file = "../ChatRoom_Q2/server.conf";

void server_send(char *name, int index) {
    char hint[MAX_SIZE + 5] = {0};
    //发送群消息
    sprintf(hint, "欢迎 \e[34m%s\e[0m 登录聊天室！", name);
    int i = 0;
    for (; i < index; i++) {
        send(look_client(linkedlist, name, index), hint, strlen(hint), 0);
    }

}

void *server_pthread(void *p) {
    int fd = *(int *)p;//拿到客户端的socketfd
    //记录客端的socketfd
    l[size].fds = fd;
    char name[20] = {0};
    if (recv(fd, name, sizeof(name), 0) > 0) {
        strcpy(l[size].name, name);//拿到昵称
        printf("用户：\e[34m%s\e[0m从\e[35m%s\e[0m登录聊天室\n", name, inet_ntoa(client_addr.sin_addr));
    }
    size++;
    char hint[MAX_SIZE + 5] = {0};
    //发送群消息
    sprintf(hint, "欢迎 \e[34m%s\e[0m 登录聊天室！", name);
    //发给所有人
    server_send(hint);
    while(1) {
        char buffer[MAX_SIZE + 5] = {0};
        if (recv(fd, buffer, sizeof(buffer), 0) == 0) {
            // 返回0，表示TCP另一端断开连接
            // 有客户端退出
            printf("\e[34m%s\e[0m 离开聊天室\n", name);
            int i;
            char name[20] = {0};
            for (i = 0; i < size; i++) {
                if (l[i].fds == fd) {
                    strcpy(name, l[i].name);
                    l[i].fds = l[size - 1].fds;
                    strcpy(l[i].name, l[size - 1].name);
                }
            }
            size--;
            char message[MAX_SIZE + 5] = {0};
            sprintf(message, "\e[34m%s\e[0m 离开聊天室", name);
            //发送退出消息
            server_send(message);
            close(fd);
            return 0;
        }
        server_send(buffer);
        printf("\e[34m%s\e[0m: %s\n", name, buffer);
    }
}

void server_start() {
    printf("\e[33m聊天室启动成功\e[0m\n");
    pthread_t pid;
    pthread_create(&pid, 0, server_pthread, &fd);
    if ((sock_fd = socket_connect(port, ip)) < 0) {
            perror("socket");
            return -1;
    } 
    while(1) {
        if ((sock_fd = socket_connect(port, ip)) < 0) {
            perror("socket");
            return -1;
        } 

        printf("\e[33m登录成功\e[0m\n");
        send(sock_fd, name, strlen(name) + 1, 0);

        while(1) {
            scanf("%[^\n]s", buff);
            getchar();
            send(sock_fd, buff, strlen(buff), 0);
            memset(buff, 0, sizeof(buff));
        }

        close(sock_fd); 
    }
    while(1) {
        int len = sizeof(struct sockaddr_in), fd = 0;
        if ((fd = accept(sock_fd, (struct sockaddr*)&client_addr, (socklen_t *)&len)) < 0) {
            perror("accept");
            continue;
        }
        int pid = fork();
        if (pid < 0) {
            printf("Error in Fork!\n");
            continue;
        }
        if (pid == 0) {
            close(sock_fd);
            if ((sock_fd = socket_connect(atoi(conf.Clinet_Port), conf.Server_Ip)) < 0) {
                perror("sock_fd");
                continue;
            }
            char name[20] = {0};
            if (recv(fd, name, sizeof(name), 0) > 0) {
                if (check_client(linkedlist, name, client_num) == 0) {
                    printf("用户已存在");
                    continue;
                }
                client_num++;
                add_client(client_num, fd, name);
                printf("用户：\e[34m%s\e[0m从\e[35m%s\e[0m登录聊天室\n", name, inet_ntoa(client_addr.sin_addr));
                server_send(name, client_num);
            }
            char buff[MAX_SIZE] = {0};
            while( recv(fd, buff, sizeof(buff), 0) > 0) {
                printf("\e[34m%s\e[0m: %s\n", name, buff);
                fflush(stdout);
                memset(buff, 0, sizeof(buff));
            }
            delete_node(linkedlist, client_num);
            printf("\e[34m%s\e[0m 离开聊天室\n", name);
            client_num--;
            close(fd);
            exit(0);
        } 
        close(fd);
    }
}

int main() {
    get_conf_value(file, "Server_Port", conf.Server_Port);
    get_conf_value(file, "Client_Port", conf.Clinet_Port);
    int client_port = atoi(conf.Clinet_Port);
    int server_port = atoi(conf.Server_Port);
    printf("\e[33m聊天室启动中...\e[0m\n");

    if ((sock_fd = socket_create(server_port)) < 0) {
        perror("socket");
        exit(-1);
    }
    
    server_start();
    return 0;
}


