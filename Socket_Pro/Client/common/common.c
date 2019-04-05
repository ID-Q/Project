/*************************************************************************
	> File Name: common.c
	> Author: zhengdongqi
	> Mail: 1821260963@qq.com 
	> Created Time: 六  2/23 11:01:24 2019
 ************************************************************************/

#include "common.h"

/*服务器使用 TCP 通讯*/
int socket_create_tcp(int port) {
    int _socket_fd;
    struct sockaddr_in _socket_addr;
    //创建套接字
    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        DBG("Error in socket tcp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "socket_create_tcp->\033[31Error in socket tcp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    //准备网络通信地址
    memset(&_socket_addr, 0, sizeof(_socket_addr));//数据初始化清零
    _socket_addr.sin_family = AF_INET;//设置为TCP通讯
    _socket_addr.sin_port = htons(port);//端口
    _socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
    //端口重用
    int yes = 1;
    if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        DBG("Error in setsockopt tcp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "socket_create_tcp->\033[31Error in setsockopt tcp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return 1;
    }
    //绑定连接
    if (bind(_socket_fd, (struct sockaddr*)&_socket_addr, sizeof(struct sockaddr)) < 0) {
        DBG("Error in bind tcp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "socket_create_tcp->\033[31Error in bind tcp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    //设置监听
    if (listen(_socket_fd, 20) < 0) {
        DBG("Error in listen tcp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "socket_create_tcp->\033[31Error in listen tcp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    return _socket_fd;
}
/*服务器使用 UDP 通讯*/
int socket_create_udp(int port) {
    int _socket_fd;
    struct sockaddr_in _socket_addr;
    //创建套接字
    if ((_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        DBG("Error in socket udp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "socket_create_udp->\033[31Error in socket udp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    //准备网络通信地址
    memset(&_socket_addr, 0, sizeof(_socket_addr));//数据初始化清零
    _socket_addr.sin_family = AF_INET;//设置为TCP通讯
    _socket_addr.sin_port = htons(port);//端口
    _socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
    //端口重用
    int yes = 1;
    if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        DBG("Error in setsockopt udp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "socket_create_udp->\033[31Error in setsockopt udp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return 1;
    }
    //绑定连接
    if (bind(_socket_fd, (struct sockaddr*)&_socket_addr, sizeof(struct sockaddr)) < 0) {
        DBG("Error in bind udp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "socket_create_udp->\033[31Error in bind udp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    return _socket_fd;
}

/*客户使用 tcp 链接*/
int socket_connect_tcp(int port, char *host, int sec, double usec) {
    int _socket_fd;
    struct sockaddr_in _socket_addr;
    //创建套接字
    if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        DBG("Error in socket tcp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "ssocket_connect_tcp->\033[31Error in socket tcp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    //设置套接字为非阻塞态
    int opt = 1;
    if (ioctl(_socket_fd, FIONBIO, &opt) < 0) {
        DBG("Error in ioctl: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "ssocket_connect_tcp->\033[31Error in ioctl: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    
    //设置服务器
    memset(&_socket_addr, 0, sizeof(_socket_addr));//数据初始化清零
    _socket_addr.sin_family = AF_INET;//设置为TCP通讯
    _socket_addr.sin_port = htons(port);//端口
    _socket_addr.sin_addr.s_addr = inet_addr(host);//IP地址
    
    //链接服务器
    if (connect(_socket_fd, (struct sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0) {
        if (errno == EINPROGRESS) {
            int error;
            int len = sizeof(int);
            struct timeval tv;
            tv.tv_sec  = sec;//最多等待时间-秒
            tv.tv_usec = usec * 1000000;//最多等待时间-微秒
            fd_set set;//设置一个套接字集合
            FD_ZERO(&set);//将套节字集合清空
            FD_SET(_socket_fd, &set);//加入套节字到集合
            if(select(_socket_fd + 1, NULL, &set, NULL, &tv) > 0) {
                getsockopt(_socket_fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);//套接字，层次，获取错误状态并清除，缓冲区，长度值
                if(error != 0) {//有错误
                    close(_socket_fd);
                    return -1;
                }
            } else {//表示超时
                close(_socket_fd);
                return -1;
            }
        } else {
            DBG("Error in connect: %s\n", strerror(errno));
            write_log(conf.Log_Dir, "ssocket_connect_tcp->\033[31Error in connect: %s\033[0m", strerror(errno));
            close(_socket_fd);
            return -1;
        }
    }
    //设置为阻塞态
    opt = 0;
    if (ioctl(_socket_fd, FIONBIO, &opt) < 0) {
        DBG("Error in ioctl: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "ssocket_connect_tcp->\033[31Error in ioctl: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    return _socket_fd;
}
/*客户使用 UDP 链接*/
int send_warning_udp(int port, char *host, char *buff) {
    int _socket_fd;
    struct sockaddr_in _socket_addr;
    //创建套接字
    if ((_socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        DBG("Error in socket udp: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "send_warning_udp->\033[31Error in socket udp: %s\033[0m", strerror(errno));
        close(_socket_fd);
        return -1;
    }
    //设置服务器
    memset(&_socket_addr, 0, sizeof(_socket_addr));//数据初始化清零
    _socket_addr.sin_family = AF_INET;//设置为TCP通讯
    _socket_addr.sin_port = htons(port);//端口
    _socket_addr.sin_addr.s_addr = inet_addr(host);//IP地址
    
    int _socket_udp;
    if ((_socket_udp = sendto(_socket_fd, buff, strlen(buff), 0, (void *)&_socket_addr, sizeof(_socket_addr))) < 0) {
        DBG("Error in sendto: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "send_warning_udp->\033[31Error in sendto: %s\033[0m", strerror(errno));
        return -1;
    }
    close(_socket_fd);
    return 1;
}
//获取配置文件数据：参数存储
int get_conf_argv(char *pathname, char* key_name, char *value) {
    FILE *fd = NULL;
    char *line = NULL;
    char *substr = NULL;
    ssize_t read = 0;
    size_t len = 0;
    int tmp = 0;
    
    fd = fopen(pathname, "r");
    if (fd == NULL) {
        DBG("Error in Open\n");
        write_log(conf.Log_Dir, "get_conf_argv->\033[31Error in Open: %s\033[0m", strerror(errno));
        exit(1);
    }
    while ((read = getline(&line, &len, fd)) != 1) {
        fflush(stdout);
        substr = strstr(line, key_name);
        if (substr == NULL) {
            continue;
        } else {
            tmp = strlen(key_name);
            if (line[tmp] == '=') {
                strncpy(value, &line[tmp + 1], (int)read - tmp - 1);
                tmp = strlen(value);
                *(value + tmp - 1) = '\0';
                break;
            }
            else {
                DBG("Next\n");
                continue;
            }
        }
    }
    return 0;
}
//获取配置文件数据：指针
char *get_conf_value(const char *pathname, const char *key_name) {
    FILE *fp = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    char *value = (char *)calloc(sizeof(char), 100);
    fp = fopen(pathname, "r");
    if (fp == NULL) {
        perror("fopen:");
        write_log(conf.Log_Dir, "get_conf_value->\033[31Error in Open: %s\033[0m", strerror(errno));
        return NULL;
    }
    while ((read = getline(&line,&len,fp)) > 0) {
        char *ptr = strstr(line,key_name);
        if (ptr == NULL) continue;
        ptr += strlen(key_name);
        if (*ptr != '=') continue;
        strncpy(value, (ptr+1), strlen(ptr+2));
        int tempvalue = strlen(value);
        value[tempvalue] = '\0';
    }
    fclose(fp);
    return value;
}
//写入日志数据
int write_log(char *pathname, const char *format, ...) {
    FILE *fp = fopen(pathname, "a");
    va_list arg;
    int ret;
    va_start(arg, format);
    time_t log_t = time(NULL);
    struct tm *tm_log = localtime(&log_t);
    fprintf(fp, "%d-%02d-%02d %02d:%02d:%02d :", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);
    ret = vfprintf(fp, format, arg);
    fprintf(fp, "\n");
    va_end(arg);
    fflush(fp);
    fclose(fp);
    return ret;
}

//发送文件
int send_file(int fd, char *filename) {
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        DBG("Error in fopen: %s", strerror(errno));
        write_log(conf.Log_Dir, "send_file->\033[31Error in Open: %s\033[0m", strerror(errno));
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *data = (char *)malloc(len + 1);
    fread(data, len, 1, fp);
    printf("%s\n", data);
    if(send(fd, data, strlen(data), 0) < 0) {
        DBG("Error in send: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "send_file->\033[31Error in send: %s\033[0m", strerror(errno));
    }
    fclose(fp);
    memset(data, 0, sizeof(len + 1));
    return 1;
}
//实现ntoa功能
char *my_inet_ntoa(struct in_addr in) {
    static char ip[20] = {0};
    char *p;
    p = (char *)&in.s_addr;
    sprintf(ip, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return ip;
}
//心跳检测
bool heart_test(int port, char *host) {
    bool ret = true;
    int fd;
    if ((fd = socket_connect_tcp(port, host, 0, 0.5)) < 0) {
        ret = false;
    }
    close(fd);
    return ret;
}
//文件压缩函数
int backup(char *filenames, char *backfilenames) {
    FILE *ffile;
    unsigned long int flen, clen;
    unsigned char *fbuf = NULL;
    unsigned char *cbuf = NULL;
    //int len = strlen(filename);
    
    //通过命令行参数将filenames文件的数据压缩后存放到backfilenames文件中
    if((ffile = fopen(filenames, "rb")) == NULL) {
        DBG("can not open %s!\n", filenames);
        return -1;
    }
    //装载源文件数据到缓冲区
    fseek(ffile, 0, SEEK_END);//跳到文件末尾
    flen = ftell(ffile);//获取文件长度
    fseek(ffile, 0, SEEK_SET);
    if((fbuf = (unsigned char *)malloc(sizeof(unsigned char) * flen)) == NULL) {
        DBG("No enough memory!\n");
        fclose(ffile);
        return -1;
    }
    fread(fbuf, sizeof(unsigned char), flen, ffile);
    //压缩数据
    clen = compressBound(flen);
    if((cbuf = (unsigned char *)malloc(sizeof(unsigned char) * clen)) == NULL) {
        DBG("NO enough memory!\n");
        fclose(ffile);
        return -1;
    }
    if(compress(cbuf, &clen, fbuf, flen) != Z_OK) {
        DBG("Compress %s failed!\n", filenames);
        return -1;
    }
    fclose(ffile);
    if((ffile = fopen(backfilenames, "a+")) == NULL) {
        DBG("Can not create %s!\n", backfilenames);
        return -1;
    }
    //保存压缩后的数据到目标文件
    fwrite(&flen, sizeof(unsigned long int), 1, ffile);//写入源文件长度
    fwrite(&clen, sizeof(unsigned long int), 1, ffile);//写入目标数据长度
    fwrite(cbuf, sizeof(unsigned char), clen, ffile);
    fclose(ffile);
    free(fbuf);
    free(cbuf);
    return 1;
}
//获取文件大小
int file_size(char *filename) {
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        DBG("Error in fopen: %s\n", strerror(errno));
        write_log(conf.Log_Dir, "file_size->\033[31Error in fopen: %s\033[0m", strerror(errno));
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    return len;
}
//文件压缩函数
int unback(char *backfilenames, char *filenames){
    //解压函数
    FILE *ffile;
    unsigned long int flen, ulen;
    unsigned char *fbuf = NULL;
    unsigned char *ubuf = NULL;
    //通过命令行参数将文件的数据解压缩后存放到文件中
    if((ffile = fopen(backfilenames, "rb")) == NULL) {
        DBG("Can\'t open %s!\n", backfilenames);
        return -1;
    }
    //装载源文件数据到缓冲区
    fread(&ulen, sizeof(unsigned long int), 1, ffile);//获取缓冲区大小
    fread(&flen, sizeof(unsigned long int), 1, ffile);//获取数据流大小
    
    if((fbuf = (unsigned char*)malloc(sizeof(unsigned char) * flen)) == NULL) {
        DBG("No enough memory!\n");
        fclose(ffile);
        return -1;
    }
    fread(fbuf, sizeof(unsigned char), flen, ffile);
    //解压缩数据
    if((ubuf = (unsigned char*)malloc(sizeof(unsigned char) * ulen)) == NULL) {
        DBG("No enough memory!\n");
        fclose(ffile);
        return -1;
    }
    if(uncompress(ubuf, &ulen, fbuf, flen) != Z_OK) {
        DBG("Uncompress %s failed!\n", backfilenames);
        return -1;
    }
    fclose(ffile);
    if((ffile = fopen(filenames, "a+")) == NULL) {
        DBG("Can\'t create %s!\n", filenames);
        return -1;
    }
    //保存解压缩后的数据到目标文件
    fwrite(ubuf, sizeof(unsigned char), ulen, ffile);
    fclose(ffile);
    free(fbuf);
    free(ubuf);
    return 1;
}
//字符判断函数
int strtok_func(char *buff, char *option, char *flag) {
    char *p = strtok(buff, flag);
    p = strtok(NULL, flag);
    while (p) {
        if (strcmp(p, option) == 0) {
            //DBG("OK\n");
            return 1;
        }
        p = strtok(NULL, flag);
    }
    //DBG("NO\n");
    return 0;
}
