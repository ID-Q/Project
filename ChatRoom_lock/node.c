/*************************************************************************
	> File Name: node.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 日  2/24 22:30:29 2019
 ************************************************************************/
#include "../common/common.h"
//插入节点
LinkedList insert(LinkedList head, Node *node) {
    Node *p = head;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = node;
    
    return head;
}
//删除节点
LinkedList delete_node(LinkedList head, char *name) {
    Node *p, *q;
    p = head;
    while (p->next != NULL && strcmp(p->next->name, name)) {
        p = p->next;
    }
    if (p->next == NULL) {
        return 0;
    }
    q = p->next;
    p->next = p->next->next;
    free(q);
    return head;
}
//释放内存
void clear(LinkedList head) {
    Node *p = head;
    while (p != NULL) {
        Node *d = p;
        p = p->next;
        free(d);
    }
}
//遍历在线用户
void client_name(LinkedList head, char *name) {
    Node *p = head->next;
    while(p) {
        sprintf(name, "%s %s", name, p->name);
        p = p->next;
    }
}
//搜索套接字
int check_fds(LinkedList head, char *name) {
    LinkedList temp = head;
    while (temp->next != NULL) {
        if (strcmp(temp->next->name, name) == 0) {
            return temp->next->fds;
        }
        temp = temp->next;
    }
    return 0;
}
//核查用户
int check_client(LinkedList head, char *name) {
    LinkedList temp = head;
    while (temp->next != NULL) {
        if (strcmp(temp->next->name, name) == 0) {
            fflush(stdout);
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}
//添加用户
Node *node_create(int fds, char *name) {
    Node *node = (Node *)malloc(sizeof(Node));
    node->fds = fds;
    strcpy(node->name, name);
    node->next = NULL;
    return node;
}

