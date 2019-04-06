/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: Sat 06 Apr 2019 19:38:10 CST
 ************************************************************************/

#include "common.h"

int create_shm(int size) {
    int shmid = 0;
    key_t key = ftok(PATHNAME, PROJ_ID);
    if ((shmid = shmget(key, size, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
    }
    return shmid;
}

int get_shm(int size) {
    int shmid = 0;
    key_t key = ftok(PATHNAME, PROJ_ID);
    if (key < 0) perror("ftok");
    if ((shmid = shmget(key, size, IPC_CREAT)) < 0) {
        perror("errget");
    }
    return shmid;
}

int destory(int shmid) {
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}

