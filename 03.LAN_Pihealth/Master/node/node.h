/*************************************************************************
	> File Name: node.h
	> Author: zhengdongqi
	> Mail: 1821260963@qq.com
	> Created Time: 二  4/ 2 15:58:16 2019
 ************************************************************************/

#ifndef _NODE_H
#define _NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#ifdef _DEBUG
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

/*创建链表节点*/
typedef struct Node{
    int fd;
    struct sockaddr_in addr;
    struct Node *next;
}Node, *LinkedList;

/*插入节点*/
LinkedList insert(LinkedList head, Node *node);
/*删除节点*/
LinkedList delete_node(LinkedList head, struct sockaddr_in addr);
/*释放链表*/
void clear(LinkedList head);
/*输出链表*/
void output(LinkedList head);
/*汇总数据*/
void client_ip(LinkedList head, char *ips);
/*寻找节点*/
int look_ip(LinkedList head, struct sockaddr_in addr);
/*核查节点*/
int check_ip(LinkedList head, struct sockaddr_in addr);
/*链表反转*/
LinkedList reverse(LinkedList head);
/*创建节点*/
Node *node_create(struct sockaddr_in addr);
/*初始化头*/
Node *head_node_init(struct sockaddr_in addr, int port);

#endif
