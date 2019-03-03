/*************************************************************************
	> File Name: common.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 六  2/23 11:01:24 2019
 ************************************************************************/

#include "../common/common.h"

/*服务器使用*/
int socket_create(int port) {
    int _socket_fd;
    struct sockaddr_in _server_addr;
    //创建套接字
    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }
    //准备网络通信地址
    memset(&_server_addr, 0, sizeof(_server_addr));//数据初始化清零
    _server_addr.sin_family = AF_INET;//设置为TCP通讯
    _server_addr.sin_port = htons(port);//端口
    _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
    //绑定连接
    if (bind(_socket_fd, (struct sockaddr*)&_server_addr, sizeof(struct sockaddr)) < 0) {
        perror("bind");
        exit(-1);
    }
    //设置监听
    if (listen(_socket_fd, 20) < 0) {
        perror("listen");
        close(_socket_fd);
        exit(-1);
    }
    return _socket_fd;
}

/*客户使用*/
int socket_connect(int port, char *host) {
    int _socket_fd;
    struct sockaddr_in _client_addr;
    //创建套接字
    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        close(_socket_fd);
        return -1;
    }
    //设置服务器
    memset(&_client_addr, 0, sizeof(_client_addr));//数据初始化清零
    _client_addr.sin_family = AF_INET;//设置为TCP通讯
    _client_addr.sin_port = htons(port);//端口
    _client_addr.sin_addr.s_addr = inet_addr(host);//IP地址
    //链接服务器
    if (connect(_socket_fd, (struct sockaddr *)&_client_addr, sizeof(_client_addr)) < 0) {
        perror("connect");
        close(_socket_fd);
        return 1;
    }
    return _socket_fd;
}
//获取配置文件
int get_conf_value(char *pathname, char* key_name, char *value) {
    FILE *fd = NULL;
    char *line = NULL;
    char *substr = NULL;
    ssize_t read = 0;
    size_t len = 0;
    int tmp = 0;

    fd = fopen(pathname, "r");
    
    if (fd == NULL) {
        printf("Error in Open\n");
        exit(1);
    }
    
    while ((read = getline(&line, &len, fd)) != 1) {
        DBG("%s", line);
        fflush(stdout);
        substr = strstr(line, key_name);
        if (substr == NULL) {
            continue;
        } else {
            tmp = strlen(key_name);
            if (line[tmp] == '=') {
                DBG("tmp = %d\n", tmp);
                DBG("read = %d\n", read);

                strncpy(value, &line[tmp + 1], (int)read - tmp - 1);
                tmp = strlen(value);
                *(value + tmp - 1) = '\0';
                break;
            }
            else {
                printf("Next\n");
                continue;
            }
        }
    }
    return 0;
}


