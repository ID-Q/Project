/*************************************************************************
	> File Name: Server.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 五  2/22 12:39:00 2019
 ************************************************************************/
#include "../common/common.h"

#define MAX_SIZE 256

int sock_fd;
struct sockaddr_in client_addr;

void server_start() {
    printf("\e[33m聊天室启动成功\e[0m\n");
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
            char name[20] = {0};
            if (recv(fd, name, sizeof(name), 0) > 0) {
                printf("用户：\e[34m%s\e[0m从\e[35m%s\e[0m登录聊天室\n", name, inet_ntoa(client_addr.sin_addr));
            }
            char buff[MAX_SIZE] = {0};
            while( recv(fd, buff, sizeof(buff), 0) > 0) {
                printf("\e[34m%s\e[0m: %s\n", name, buff);
                fflush(stdout);
                memset(buff, 0, sizeof(buff));
            }
            printf("\e[34m%s\e[0m 离开聊天室\n", name);
            close(fd);
            exit(0);
        } 
        close(fd);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ./server port\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    printf("\e[33m聊天室启动中...\e[0m\n");

    if ((sock_fd = socket_create(port)) < 0) {
        perror("socket");
        exit(-1);
    }

    server_start();
    return 0;
}


