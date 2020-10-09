/*************************************************************************
	> File Name: network.h
	> Author: 
	> Mail: 
	> Created Time: 四 10/ 8 14:22:56 2020
 ************************************************************************/

#ifndef _NETWORK_H
#define _NETWORK_H

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>

class TCP {
public:
    TCP(int port, const char *host = "0.0.0.0", int sec = 0, double usec = 0.5) : _port(port), _host(host), _sec(sec), _usec(usec) {}
    int socket_create_tcp();
    int socket_connect_tcp();
private:
    int _port;
    const char *_host;
    int _sec;
    double _usec;
};
class UDP {
public:
    UDP(int port, const char *host = "0.0.0.0", int sec = 0, double usec = 0.5) : _port(port), _host(host), _sec(sec), _usec(usec) {}
    int socket_create_udp();
    int socket_connect_udp(char *buff);
private:
    int _port;
    const char *_host;
    int _sec;
    double _usec;
};

int TCP::socket_create_tcp() {
    int socket_fd;
    struct sockaddr_in socket_addr;
    //创建套接字
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        close(socket_fd);
        return -1;
    }
    //设置服务器
    memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
    socket_addr.sin_family = AF_INET;//设置协议族
    socket_addr.sin_port = htons(this->_port);//端口
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
    //端口重用
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        close(socket_fd);
        return -1;
    }
    //绑定连接
    if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(struct sockaddr)) < 0) {
        close(socket_fd);
        return -1;
    }
    //设置监听
    if (listen(socket_fd, 20) < 0) {
        close(socket_fd);
        return -1;
    }
    return socket_fd;    
}

int TCP::socket_connect_tcp() {
    int socket_fd;
    struct sockaddr_in socket_addr;
    //创建套接字
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        close(socket_fd);
        return -1;
    }
    //设置套接字为非阻塞态
    int opt = 1;
    if (ioctl(socket_fd, FIONBIO, &opt) < 0) {
        close(socket_fd);
        return -1;
    }
    //设置服务器
    memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
    socket_addr.sin_family = AF_INET;//设置协议族
    socket_addr.sin_port = htons(this->_port);//端口
    socket_addr.sin_addr.s_addr = inet_addr(this->_host);//IP地址
    //链接服务器
    if (connect(socket_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
        if (errno == EINPROGRESS) {
            int error;
            int len = sizeof(int);
            struct timeval tv;
            tv.tv_sec  = this->_sec;//最多等待时间-秒
            tv.tv_usec = this->_usec * 1000000;//最多等待时间-微秒
            fd_set set;//设置一个套接字集合
            FD_ZERO(&set);//将套节字集合清空
            FD_SET(socket_fd, &set);//加入套节字到集合
            if(select(socket_fd + 1, NULL, &set, NULL, &tv) > 0) {
                getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);//套接字，层次，获取错误状态并清除，缓冲区，长度值
                if(error != 0) {//有错误
                    close(socket_fd);
                    return -1;
                }
            } else {//表示超时
                close(socket_fd);
                return -1;
            }
        } else {
            close(socket_fd);
            return -1;
        }
    }
    //设置为阻塞态
    opt = 0;
    if (ioctl(socket_fd, FIONBIO, &opt) < 0) {
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}

int UDP::socket_create_udp() {
    int socket_fd;
    struct sockaddr_in socket_addr;
    //创建套接字
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        close(socket_fd);
        return -1;
    }
    //设置服务器
    memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
    socket_addr.sin_family = AF_INET;//设置协议族
    socket_addr.sin_port = htons(this->_port);//端口
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
    //端口重用
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        close(socket_fd);
        return -1;
    }
    //绑定连接
    if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(struct sockaddr)) < 0) {
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}

int UDP::socket_connect_udp(char *buff) {
    int socket_fd;
    struct sockaddr_in socket_addr;
    //创建套接字
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        close(socket_fd);
        return -1;
    }
    //设置服务器
    memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
    socket_addr.sin_family = AF_INET;//设置协议族
    socket_addr.sin_port = htons(this->_port);//端口
    socket_addr.sin_addr.s_addr = inet_addr(this->_host);//IP地址
    close(socket_fd);
    int socket_udp;
    socket_udp = sendto(socket_fd, buff, sizeof(buff), 0, (void *)&socket_addr, sizeof(socket_addr));
    if (socket_udp < 0) {
        close(socket_fd);
        return -1;
    }
    close(socket_fd);
    return 1;
}

#endif
