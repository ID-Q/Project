/*************************************************************************
	> File Name: node.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 日  2/24 22:30:29 2019
 ************************************************************************/
#include "../common/common.h"

LinkedList insert(LinkedList head, Node *node, int index) {
    if (head == NULL) {
        if (index != 0) {
            printf("failed\n");
            return head;
        }
        head = node;
        //printf("success\n");
        return head;
    }
    if (index == 0) {
        node->next = head;
        head = node;
        //printf("success\n");
        return head;
    }
    Node *current_node = head;
    int count = 0;
    while (current_node->next != NULL && count < index - 1) {
        current_node = current_node->next;
        count++;
    }
    if (count == index - 1) {
        node->next = current_node->next;
        current_node->next = node;
        printf("success\n");
        return head;
    }
    printf("failed\n");
    return head;
}

int look_client(LinkedList head, char *name, int index) {
    if (head == NULL) {
        return 0;
    }
    Node *current_node = head;
    while (current_node->name != name && index > 0) {
        current_node = current_node->next;
        index--;
    }
    if (index < 0) return 0;
    return current_node->fds;
}

int check_client(LinkedList head, char *name, int index) {
    if (head == NULL) {
        return -1;
    }
    Node *current_node = head;
    while (current_node->name != name && index > 0) {
        current_node = current_node->next;
        index--;
    }
    if (index < 0) return 1;
    return 0;
}

int delete_node(LinkedList head, int index) {
	Node ret, *p, *q;
    ret.next = head;
    p = &ret;
    while (p->next && index) {
        p = p->next;
        --index;
    }
    if (p->next) {
        q = p->next;
        p->next = q->next;
        free(q);
    }
    return 1;
}

void clear(LinkedList head) {
    Node *current_node = head;
    while (current_node != NULL) {
        Node *delete_node = current_node;
        current_node = current_node->next;
        free(delete_node);
    }
}

void add_client(int index, int fds, char *name) {
    LinkedList linkedlist = NULL;
    Node *node = (Node *)malloc(sizeof(Node));
    node->fds = fds;
    node->name = name;
    node->next = NULL;
    linkedlist = insert(linkedlist, node, index - 1);
    clear(linkedlist);
}
