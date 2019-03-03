/*************************************************************************
	> File Name: Client.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  2/21 17:56:23 2019
    192.168.1.40:8731
 ************************************************************************/
#include "../common/common.h"

#define MAX_SIZE 1024
#define SIZE 20

struct Client_Conf{
    char Server_Port[SIZE];
    char Client_Port[SIZE];
    char My_Name[SIZE];
    char Server_Ip[SIZE];
    char Log_File[SIZE];
}conf;

int sock_fd;
int sock_listen;
char buff[MAX_SIZE] = {0};
char *file = "../ChatRoom_Q2/client.conf";

struct Message {
    char from[SIZE];//储存客户名称
    int flag;
    char message[MAX_SIZE];
};

void logout(int sig) {
    freopen(conf.Log_File, "a+", stdout);
    printf("\033[43m您已退出登录, 我们下次见！\033[0m \n");
    fclose(stdout);
    _exit(0);
}

int main() {
    
    signal(SIGINT, logout);
    get_conf_value(file, "Server_Port", conf.Server_Port);
    get_conf_value(file, "Client_Port", conf.Client_Port);
    get_conf_value(file, "My_Name", conf.My_Name);
    get_conf_value(file, "Log_File", conf.Log_File);
    get_conf_value(file, "Server_Ip", conf.Server_Ip);
    int client_port = atoi(conf.Client_Port);
    int server_port = atoi(conf.Server_Port);
    
    
    int pid = 0;
    
    if ((pid = fork())< 0) {
        perror("fork");
        exit(1);
    }
    
    if (pid != 0) {
        int fd;
        if ((sock_fd = socket_create(client_port)) < 0) {
            printf("Error in socket_create\n");
        }
        struct Message recv_message;
        while (1) {
            if ((fd = accept(sock_fd, NULL, NULL)) < 0)
            {
                perror("accept");
                continue;
            }
            recv(fd, (char *)&recv_message, sizeof(recv_message), 0);
            close(fd);
            freopen(conf.Log_File, "a+", stdout);
            if (recv_message.flag == 0) {
                printf("\033[34m%s\033[0m: %s\n", recv_message.from, recv_message.message);
            } else if (recv_message.flag == 2) {
                printf("\033[43m服务器通知消息: \033[0m %s\n", recv_message.message);
            } else {
                printf("\033[34m%s\033[0m: %s   \033[5m \033[32m * \033[0m\n", recv_message.from, recv_message.message);
            }
            fclose(stdout);
            
        }
        
    } else {
        if ((sock_fd = socket_connect(server_port, conf.Server_Ip)) < 0) {
            printf("Error in connect!\n");
            return -1;
        }
        // DBG("After connect!\n");
        send(sock_fd, conf.My_Name, strlen(conf.My_Name) + 1, 0);
        system("clear");
        while(1) {
            printf("请在以下的对话框输入信息，按回车发送\n\n");
            scanf("%[^\n]s", buff);
            getchar();
            int send_num = send(sock_fd, buff, strlen(buff), 0);
            if (send_num < 0) {
                perror("send");
            }
            memset(buff, 0, sizeof(buff));
            system("clear");
        }
        close(sock_fd);
    }
    return 0;
}

