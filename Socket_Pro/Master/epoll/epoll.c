/*************************************************************************
	> File Name: epoll.c
	> Author: zhengdongqi
	> Mail: 1821260963@qq.com
	> Created Time: 二  4/ 2 13:17:56 2019
 ************************************************************************/

#include "epoll.h"

/*IO多路复用epoll*/
void do_epoll(LinkedList head, int listenfd) {
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int ret;
    char buf[MAXSIZE] = {0};
    //创建一个描述符
    epollfd = epoll_create(FDSIZE);
    //添加监听描述符事件
    add_event(head, epollfd, listenfd, EPOLLIN);
    for ( ; ; ) {
        //获取已经准备好的描述符事件
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        handle_events(head, epollfd, events, ret, listenfd, buf);
    }
    close(epollfd);
    return ;
}
/*事件处理函数*/
void handle_events(LinkedList head, int epollfd, struct epoll_event *events, int num, int listenfd, char *buf) {
    int i;
    int fd;
    //进行选好遍历
    for (i = 0; i < num; i++) {
        fd = events[i].data.fd;
        //根据描述符的类型和事件类型进行处理
        if ((fd == listenfd) && (events[i].events & EPOLLIN))
            handle_accpet(head, epollfd, listenfd);
        else if (events[i].events & EPOLLIN)
            do_read(head, epollfd, fd, buf);
        else if (events[i].events & EPOLLOUT)
            do_write(head, epollfd, fd, buf);
    }
}
/*处理接收到的连接*/
void handle_accpet(LinkedList head, int epollfd, int listenfd) {
    struct sockaddr_in fdaddr;
    int len = sizeof(struct sockaddr_in), fd;
    fd = accept(listenfd,(struct sockaddr*)&fdaddr, (socklen_t *)&len);
    if (fd == -1) {
        DBG("epoll.c->handle_accpet\033[31m接受套接字失败: %s\033[0m\n", strerror(errno));
    } else {
        LinkedList p = head;
        if (!check_ip(p, fdaddr)) {
            Node *node = node_create(fdaddr);
            head = insert(p, node);
            DBG("epoll.c->handle_accpet\033[34m%s\033[0m 登陆成功\n", inet_ntoa(fdaddr.sin_addr));
        }else {
            DBG("epoll.c->handle_accpet用户\033[34m%s\033[0m已存在\n", inet_ntoa(fdaddr.sin_addr));
        }
        //添加一个客户描述符和事件
        add_event(head, epollfd, fd, EPOLLIN);
    }
}
/*处理进行连接*/
void handle_connect(LinkedList head, int fd) {
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    char buf[MAXSIZE];
    int ret;
    epollfd = epoll_create(FDSIZE);
    add_event(head, epollfd, STDIN_FILENO, EPOLLIN);
    for ( ; ; ) {
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        handle_events(head, epollfd, events, ret, fd, buf);
    }
    close(epollfd);
}
/*读处理*/
void do_read(LinkedList head, int epollfd, int fd, char *buf) {
    int nread;
    nread = read(fd, buf, MAXSIZE);
    if (nread == -1) {
        DBG("epoll.c->do_read\033[31m读操作失败失败: %s\033[0m\n", strerror(errno));
        close(fd);
        delete_event(head, epollfd, fd, EPOLLIN);
    } else if (nread == 0) {
        DBG("epoll.c->do_read\033[34m客户关闭\033[0m\n");
        close(fd);
        delete_event(head, epollfd, fd, EPOLLIN);
    } else {
        DBG("epoll.c->do_read\033[34mread：%s\033[0m\n", buf);
        char message[MAXSIZE] = {0};
        sprintf(message, "\033[34m欢迎%s登陆\033[0m", buf);
        memset(buf, 0, strlen(buf));
        strcpy(buf, message);
        //修改描述符对应的事件，由读改为写
        modify_event(head, epollfd, fd, EPOLLOUT);
    }
}
/*写处理*/
void do_write(LinkedList head, int epollfd, int fd, char *buf) {
    int nwrite;
    nwrite = write(fd, buf, strlen(buf));
    if (nwrite == -1) {
        DBG("epoll.c->do_write\033[31m写操作失败失败: %s\033[0m\n", strerror(errno));
        close(fd);
        delete_event(head, epollfd, fd, EPOLLOUT);
    } else {
        modify_event(head, epollfd, fd, EPOLLIN);
    }
    memset(buf, 0, MAXSIZE);
}
/*添加事件*/
void add_event(LinkedList head, int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}
/*修改事件*/
void modify_event(LinkedList head, int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}
/*删除事件*/
void delete_event(LinkedList head, int epollfd, int fd, int state) {
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}
