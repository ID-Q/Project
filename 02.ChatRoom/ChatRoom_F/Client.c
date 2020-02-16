/*************************************************************************
	> File Name: Client.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  2/21 17:56:23 2019
    192.168.1.40:8731
 ************************************************************************/
#include "../common/common.h"

#define MAX_SIZE 256

int sock_fd;
char buff[MAX_SIZE] = {0};

int main(int argc, char *argv[]) {
    //打印提示信息
    if (argc != 4) {
        printf("Usage: ./server ip port userame\n");
        exit(1);
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    char *name = argv[3];

    printf("\e[33m登录中...\e[0m\n");
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
    return 0;
}




