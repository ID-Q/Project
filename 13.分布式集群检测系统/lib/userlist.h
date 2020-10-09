/*************************************************************************
	> File Name: userlist.h
	> Author: 
	> Mail: 
	> Created Time: 二 10/ 6 17:34:43 2020
 ************************************************************************/

#ifndef _USERLIST_H
#define _USERLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

/*创建链表节点*/
typedef struct Node{
    int fd;
    struct sockaddr_in addr;
    struct Node *next;
}Node, *LinkedList;

/*插入节点*/
LinkedList insert(LinkedList head, Node *node) {
    Node *p = head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = node;
    return head;
}
/*删除节点*/
LinkedList delete_node(LinkedList head, struct sockaddr_in addr) {
    Node *p, *q;
    p = head;
    while (p->next != NULL && (p->next->addr.sin_addr.s_addr != addr.sin_addr.s_addr)) {
        p = p->next;
    }
    if (p->next == NULL) {
        return 0;
    }
    q = p->next;
    p->next = q->next;
    free(q);
    return head;
}

/*释放内存*/
void clear(LinkedList head) {
    Node *p = head;
    while (p != NULL) {
        Node *d = p;
        p = p->next;
        free(d);
    }
}
/*遍历数据*/
void output(LinkedList head) {
    if (head == NULL) {
        return ;
    }
    Node *p = head;
    while (p->next != NULL) {
        printf("%s fd->%d\n", inet_ntoa(p->next->addr.sin_addr), p->next->fd);
        p = p->next;
    }
}
/*遍历在线ip*/
void client_ip(LinkedList head, char *ips) {
    Node *p = head->next;
    while(p) {
        sprintf(ips, "%s %d", ips, p->addr.sin_addr.s_addr);
        p = p->next;
    }
}
/*搜索ip*/
int look_ip(LinkedList head, struct sockaddr_in addr) {
    LinkedList p = head;
    while (p->next != NULL) {
        if (p->next->addr.sin_addr.s_addr == addr.sin_addr.s_addr) {
            return p->next->addr.sin_addr.s_addr;
        }
        p = p->next;
    }
    return 0;
}
/*核查ip*/
int check_ip(LinkedList head, struct sockaddr_in addr) {
    LinkedList p = head;
    while (p->next != NULL) {
        if (p->next->addr.sin_addr.s_addr == addr.sin_addr.s_addr) {
            return 1;
        }
        p = p->next;
    }
    return 0;
}
/*链表反转*/
LinkedList reverse(LinkedList head) {
    Node ret;
    ret.next = NULL;
    Node *p, *q;
    p = head;
    while (p) {
        q = p->next;
        p->next = ret.next;
        ret.next = p;
        p = q;           
    }
    return ret.next;
}
/*添加ip*/
Node *node_create(struct sockaddr_in addr) {
    Node *p = (Node *)malloc(sizeof(Node));
    p->fd = -1;
    p->addr = addr;
    p->next = NULL;
    return p;
}
/*添加头节点*/
Node *head_node_init(struct sockaddr_in addr, int port) {
    addr.sin_family = AF_INET;//设置协议族
    addr.sin_port = htons(port);//设置端口
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");//设置IP地址
    Node *p;
    p = (Node *)malloc(sizeof(Node));
    p->fd = -1;
    p->addr = addr;
    p->next = NULL;
    return p;
}

#endif
