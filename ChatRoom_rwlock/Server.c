/*************************************************************************
	> File Name: Server.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 五  2/22 12:39:00 2019
 ************************************************************************/
#include "../common/common.h"

#define MAX_SIZE 1024
#define SIZE 20

pthread_rwlock_t rwlock;

struct Message {
    char from[20];
    int flag;
    char message[MAX_SIZE];
}msg;

int sock_listen;
int client_num = 0;
LinkedList linkedlist = NULL;
char *file = "../ChatRoom_rwlock/server.conf";

void *print_pthread() {
    LinkedList temp;
    while (1) {
        temp = linkedlist;
        fflush(stdout);
        sleep(30);
        while (temp->next != NULL) {
            printf("%s -> %d\n", temp->next->name, temp->next->fds);
            temp = temp->next;
        }
        DBG("print************\n");
    }
}

void send_to(char *name, struct Message message) {

    //read_lock
    pthread_rwlock_rdlock(&rwlock);
    int fd = check_fds(linkedlist, name);
    pthread_rwlock_unlock(&rwlock);

    send(fd, (char *)&message, sizeof(message), 0);
}

void send_all(LinkedList head, struct Message message) {
    LinkedList temp = head;
    while (temp->next != NULL) {
        send_to(temp->next->name, message);
        temp = temp->next;
    }
}

struct Message check_message(Node *node, struct Message message, char *buffer, char *to, char *buff) {
    int i = 0, yes = 0;
    for (; i < 20; i++) {
        if (buffer[i] == ' ') {
            yes = 1;
            break;
        }
    }
    if (buffer[0] == '@' && yes) {
        strncpy(to, buffer + 1, i - 1);
        strncpy(buff, buffer + i + 1, strlen(buffer) - i);
        sprintf(message.from, "%s@你", message.from);
        sprintf(message.message, "%s", buff);
        message.flag = 1;
    } else {
        message.flag = 0;
    }
    return message;
}

void *log_pthread(void *argv) {
    Node *p = (Node *)argv;

    //write_lock
    pthread_rwlock_wrlock(&rwlock);
    linkedlist = insert(linkedlist, p);
    pthread_rwlock_unlock(&rwlock);

    struct Message log_message;
    sprintf(log_message.message, "登录成功，您的用户名为:\033[34m%s\033[0m", p->name);
    log_message.flag = 2;

    //write_lock
    pthread_rwlock_wrlock(&rwlock);
    client_num++;
    pthread_rwlock_unlock(&rwlock);

    send_to(p->name, log_message);
    sprintf(log_message.message,"聊天室好友\033[34m%s\033[0m上线了，快和\033[34m%s\033[0m打个招呼吧！当前在线用户\033[34m%d\033[0m", p->name, p->name, client_num);
    log_message.flag = 2;
    send_to(p->name, log_message);
    char to[20] = {0};
    char buff[1024] = {0};
    char buffer[1024] = {0};

    //write_lock
    pthread_rwlock_wrlock(&rwlock);
    while (recv(p->fds, buffer, sizeof(buffer), 0) > 0) {
        pthread_rwlock_unlock(&rwlock);

        struct Message talk_message;
        strcpy(talk_message.from, p->name);
        strcpy(talk_message.message, buffer);
        talk_message.flag = 0;
        if (strcmp(buffer, "#") == 0) {

            //read_lock
            pthread_rwlock_rdlock(&rwlock);
            printf("\033[34m%s\033[0m: 查询了当前人数\n", talk_message.from);
            pthread_rwlock_unlock(&rwlock);

            char names[1024] = {0};
            sprintf(names, "%s( \033[34m%d\033[0m 人 ) :", "当前在线用户", client_num);
            client_name(linkedlist, names);
            strcpy(talk_message.message, names);
            talk_message.flag = 2;
            send(p->fds, (char *)&talk_message, sizeof(talk_message), 0);
            memset(buffer, 0, sizeof(buffer));
            continue;
        }
        talk_message = check_message(p, talk_message, buffer, to, buff);
        if (talk_message.flag == 0) {

            //read_lock
            pthread_rwlock_rdlock(&rwlock);
            printf("\033[34m%s\033[0m: %s\n", talk_message.from, talk_message.message);
            pthread_rwlock_unlock(&rwlock);

            send_all(linkedlist, talk_message);
            memset(buffer, 0, sizeof(buffer));
        }
        if (talk_message.flag == 1) {

            //read_lock
            pthread_rwlock_rdlock(&rwlock);
            if (check_client(linkedlist, to)) {
                printf("\033[34m%s\033[0m发送私聊信息->\033[34m%s\033[0m\n", p->name, to);
                pthread_rwlock_unlock(&rwlock);

                send_to(to, talk_message);
                memset(to, 0, sizeof(to));
                memset(buffer, 0, sizeof(buffer));
            } else {
                sprintf(talk_message.message, " 用户 \033[34m%s\033[0m 不存在或已下线\n", to);
                talk_message.flag = 2;
                send_to(p->name, talk_message);
                memset(buffer, 0, sizeof(buffer));
            }
            continue;
        }
        if (talk_message.flag == 2) {
            printf("%s:\n",talk_message.message);
            send_all(linkedlist, talk_message);
            memset(buffer, 0, sizeof(buffer));
        }
    }

    //read_lock
    pthread_rwlock_rdlock(&rwlock);
    printf("用户\033[34m%s\033[0m 已下线！\n", p->name);
    pthread_rwlock_unlock(&rwlock);

    //write_lock
    pthread_rwlock_wrlock(&rwlock);
    delete_node(linkedlist, p->name);
    client_num--;
    pthread_rwlock_unlock(&rwlock);
    return 0;
}

void server_start() {
    while(1) {
        int fd;
        if ((fd = accept(sock_listen, NULL, NULL)) < 0) {
            DBG("Error in accept");
            continue;
        }
        char name[20] = {0};
        recv(fd, name, sizeof(name), 0);
        if (!check_client(linkedlist, name)) {
            Node *node = node_create(fd, name);
            pthread_rwlock_rdlock(&rwlock);
            printf("\033[34m%s\033[0m Login Server\n", name);
            pthread_rwlock_unlock(&rwlock);
            pthread_t log_pid;
            pthread_create(&log_pid, 0, log_pthread, (void *)node);
        }else {
            struct Message ERR;
            sprintf(ERR.message, "用户\033[34m%s\033[0m已存在", name);
            ERR.flag = 2;
            send(fd, (char *)&ERR, sizeof(ERR), 0);
        }
    }
    return ;
}

int main() {
    DBG("hh\n");
    pthread_rwlock_init(&rwlock,NULL);
    char Server_Port[SIZE] = {0};
    get_conf_value(file, "Server_Port", Server_Port);
    int server_port = atoi(Server_Port);

    Node *p;
    p = (Node *)malloc(sizeof(Node));
    strcpy(p->name, "init");
    p->next = NULL;
    linkedlist = p; 

    //pthread_t print_pid;
    //pthread_create(&print_pid, 0, print_pthread, 0);

    if ((sock_listen = socket_create(server_port)) < 0) {
        DBG("Error in create socket");
        return 1;
    }
    server_start();
    //pthread_join(print_pid, NULL);
    close(sock_listen);
    pthread_rwlock_destroy(&rwlock);
    return 0;
}


