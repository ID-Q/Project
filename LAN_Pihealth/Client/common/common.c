/*************************************************************************
	> File Name: common.c
	> Author: zhengdongqi
	> Mail: 1821260963@qq.com
	> Created Time: 二  4/ 2 15:57:20 2019
 ************************************************************************/

#include "common.h"

/*创建一个socket 类型TCP*/
int socket_create_tcp(int port) {
    int socket_fd;
    struct sockaddr_in socket_addr;
    //创建套接字
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        DBG("common.c->socket_create_tcp\033[31m创建套接字失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_create_tcp\033[31m创建套接字失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    //设置服务器
    memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
    socket_addr.sin_family = AF_INET;//设置协议族
    socket_addr.sin_port = htons(port);//端口
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
    //端口重用
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        DBG("common.c->socket_create_tcp\033[31m设置端口重用失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_create_tcp\033[31m设置端口重用失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    //绑定连接
    if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(struct sockaddr)) < 0) {
        DBG("common.c->socket_create_tcp\033[31m绑定失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_create_tcp\033[31m绑定失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    //设置监听
    if (listen(socket_fd, 20) < 0) {
        DBG("common.c->socket_create_tcp\033[31m监听失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_create_tcp\033[31m监听失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}
/*创建一个socket 类型UDP*/
int socket_create_udp(int port) {
    int socket_fd;
    struct sockaddr_in socket_addr;
    //创建套接字
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        DBG("common.c->socket_create_udp创建套接字失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_create_udp\033[31m创建套接字失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    //设置服务器
    memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
    socket_addr.sin_family = AF_INET;//设置协议族
    socket_addr.sin_port = htons(port);//端口
    socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
    //端口重用
    int reuse = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
        DBG("common.c->socket_create_udp\033[31m设置端口重用失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_create_udp\033[31m设置端口重用失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    //绑定连接
    if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(struct sockaddr)) < 0) {
        DBG("common.c->socket_create_udp\033[31m绑定失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_create_udp\033[31m绑定失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}
/*进行 socket 连接 类型TCP*/
int socket_connect_tcp(int port, char *host, int sec, double usec) {
    int socket_fd;
    struct sockaddr_in socket_addr;
    //创建套接字
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        DBG("common.c->socket_connect_tcp\033[31m创建套接字失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_connect_tcp\033[31m创建套接字失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    //设置套接字为非阻塞态
    int opt = 1;
    if (ioctl(socket_fd, FIONBIO, &opt) < 0) {
        DBG("common.c->socket_connect_tcp\033[31m设置非阻塞失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_connect_tcp\033[31m设置非阻塞失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    //设置服务器
    memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
    socket_addr.sin_family = AF_INET;//设置协议族
    socket_addr.sin_port = htons(port);//端口
    socket_addr.sin_addr.s_addr = inet_addr(host);//IP地址
    //链接服务器
    if (connect(socket_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
        if (errno == EINPROGRESS) {
            int error;
            int len = sizeof(int);
            struct timeval tv;
            tv.tv_sec  = sec;//最多等待时间-秒
            tv.tv_usec = usec * 1000000;//最多等待时间-微秒
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
            DBG("common.c->socket_connect_tcp\033[31m连接失败: %s\033[0m\n", strerror(errno));
            write_log(conf.Sys_Log, "common.c->socket_connect_tcp\033[31m连接失败: %s\033[0m", strerror(errno));
            close(socket_fd);
            return -1;
        }
    }
    //设置为阻塞态
    opt = 0;
    if (ioctl(socket_fd, FIONBIO, &opt) < 0) {
        DBG("common.c->socket_connect_tcp\033[31m设置为阻塞态失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_connect_tcp\033[31m设置为阻塞态失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}
/*进行 socket 连接 类型UDP*/
int socket_connect_udp(int port, char *host, char *buff) {
    int socket_fd;
    struct sockaddr_in socket_addr;
    //创建套接字
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        DBG("common.c->socket_connect_udp\033[31m创建套接字失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_connect_udp\033[31m创建套接字失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    //设置服务器
    memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
    socket_addr.sin_family = AF_INET;//设置协议族
    socket_addr.sin_port = htons(port);//端口
    socket_addr.sin_addr.s_addr = inet_addr(host);//IP地址
    
    int socket_udp;
    socket_udp = sendto(socket_fd, buff, sizeof(buff), 0, (void *)&socket_addr, sizeof(socket_addr));
    if (socket_udp < 0) {
        DBG("common.c->socket_connect_udp\033[31m发送失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->socket_connect_udp\033[31m发送失败: %s\033[0m", strerror(errno));
        close(socket_fd);
        return -1;
    }
    close(socket_fd);
    return 1;
}
/*获取配置文件数据：参数存储*/
int get_conf_argv(char *pathname, char* key_name, char *value) {
    FILE *fd = NULL;
    char *line = NULL;
    char *substr = NULL;
    ssize_t read = 0;
    size_t len = 0;
    int tmp = 0;
    
    fd = fopen(pathname, "r");
    if (fd == NULL) {
        DBG("common.c->get_conf_argv\033[31m文件打开失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->get_conf_argv\033[31m文件打开失败: %s\033[0m", strerror(errno));
        return -1;
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
                continue;
            }
        }
    }
    return 0;
}
/*获取配置文件数据：返回指针*/
char *get_conf_value(const char *pathname, const char *key_name) {
    FILE *fp = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    char *value = (char *)calloc(sizeof(char), 100);
    fp = fopen(pathname, "r");
    if (fp == NULL) {
        DBG("common.c->get_conf_value\033[31m文件打开失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->get_conf_value\033[31m文件打开失败: %s\033[0m", strerror(errno));
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
/*写入日志数据*/
int write_log(char *pathname, const char *format, ...) {
    FILE *fp = fopen(pathname, "a");
    flock(fp->_fileno, LOCK_EX);
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
/*发送文件*/
int send_file(int fd, char *file) {
    FILE *fp;
    if ((fp = fopen(file, "r")) == NULL) {
        DBG("common.c->send_file\033[31m文件打开失败: %s\033[0m\n", strerror(errno));
        write_log(conf.Sys_Log, "common.c->send_file\033[31m文件打开失败: %s\033[0m", strerror(errno));
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *data = (char *)malloc(len + 1);
    fread(data, len, 1, fp);
    printf("%s\n", data);
    if(send(fd, data, strlen(data), 0) < 0) {
        DBG("common.c->send_file\033[31m文件发送失败 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->send_file\033[31m文件发送失败 %s\033[0m", strerror(errno));
    }
    fclose(fp);
    memset(data, 0, sizeof(len + 1));
    return 1;
}
/*实现ntoa功能*/
char *my_inet_ntoa(struct in_addr in) {
    static char ip[20] = {0};
    char *p;
    p = (char *)&in.s_addr;
    sprintf(ip, "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    return ip;
}
/*心跳检测*/
bool heart_test(int port, char *host) {
    bool ret = true;
    int fd;
    if ((fd = socket_connect_tcp(port, host, 0, 0.5)) < 0) {
        ret = false;
    }
    close(fd);
    return ret;
}
/*文件压缩函数*/
int backup(char *filenames, char *backfilenames) {
    FILE *ffile;
    unsigned long int flen, clen;
    unsigned char *fbuf = NULL;
    unsigned char *cbuf = NULL;
    
    //通过命令行参数将filenames文件的数据压缩后存放到backfilenames文件中
    if((ffile = fopen(filenames, "rb")) == NULL) {
        DBG("common.c->backup\033[31m文件打开失败 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->backup\033[31m文件打开失败 %s\033[0m", strerror(errno));
        return -1;
    }
    //装载源文件数据到缓冲区
    fseek(ffile, 0, SEEK_END);//跳到文件末尾
    flen = ftell(ffile);//获取文件长度
    fseek(ffile, 0, SEEK_SET);
    if((fbuf = (unsigned char *)malloc(sizeof(unsigned char) * flen)) == NULL) {
        DBG("common.c->backup\033[31m空间不足 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->backup\033[31m空间不足 %s\033[0m", strerror(errno));
        fclose(ffile);
        return -1;
    }
    fread(fbuf, sizeof(unsigned char), flen, ffile);
    //压缩数据
    clen = compressBound(flen);
    if((cbuf = (unsigned char *)malloc(sizeof(unsigned char) * clen)) == NULL) {
        DBG("common.c->backup\033[31m空间不足 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->backup\033[31m空间不足 %s\033[0m", strerror(errno));
        fclose(ffile);
        return -1;
    }
    if(compress(cbuf, &clen, fbuf, flen) != Z_OK) {
        DBG("common.c->backup\033[31m压缩失败 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->backup\033[31m压缩失败 %s\033[0m", strerror(errno));
        return -1;
    }
    fclose(ffile);
    if((ffile = fopen(backfilenames, "a+")) == NULL) {
        DBG("common.c->backup\033[31m压缩文件创建失败 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->backup\033[31m压缩文件创建失败 %s\033[0m", strerror(errno));
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
/*获取文件大小*/
int file_size(char *filename) {
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        DBG("common.c->file_size\033[31m文件打开失败 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->file_size\033[31m文件打开失败 %s\033[0m", strerror(errno));
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    return len;
}
/*文件解压函数*/
int unback(char *backfilenames, char *filenames){
    //解压函数
    FILE *ffile;
    unsigned long int flen, ulen;
    unsigned char *fbuf = NULL;
    unsigned char *ubuf = NULL;
    //通过命令行参数将文件的数据解压缩后存放到文件中
    if((ffile = fopen(backfilenames, "rb")) == NULL) {
        DBG("common.c->unback\033[31m文件打开失败 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->unback\033[31m文件打开失败 %s\033[0m", strerror(errno));
        return -1;
    }
    //装载源文件数据到缓冲区
    fread(&ulen, sizeof(unsigned long int), 1, ffile);//获取缓冲区大小
    fread(&flen, sizeof(unsigned long int), 1, ffile);//获取数据流大小
    
    if((fbuf = (unsigned char*)malloc(sizeof(unsigned char) * flen)) == NULL) {
        DBG("common.c->unback\033[31m空间不足 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->unback\033[31m空间不足 %s\033[0m", strerror(errno));
        fclose(ffile);
        return -1;
    }
    fread(fbuf, sizeof(unsigned char), flen, ffile);
    //解压缩数据
    if((ubuf = (unsigned char*)malloc(sizeof(unsigned char) * ulen)) == NULL) {
        DBG("common.c->unback\033[31m空间不足 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->unback\033[31m空间不足 %s\033[0m", strerror(errno));
        fclose(ffile);
        return -1;
    }
    if(uncompress(ubuf, &ulen, fbuf, flen) != Z_OK) {
        DBG("common.c->unback\033[31m解压失败 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->unback\033[31m解压失败 %s\033[0m", strerror(errno));
        return -1;
    }
    fclose(ffile);
    if((ffile = fopen(filenames, "a+")) == NULL) {
        DBG("common.c->unback\033[31m解压文件创建失败 %s\033[0m", strerror(errno));
        write_log(conf.Sys_Log, "common.c->unback\033[31m解压文件创建失败 %s\033[0m", strerror(errno));
        return -1;
    }
    //保存解压缩后的数据到目标文件
    fwrite(ubuf, sizeof(unsigned char), ulen, ffile);
    fclose(ffile);
    free(fbuf);
    free(ubuf);
    return 1;
}
/*字符判断函数*/
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

