/*************************************************************************
	> File Name: echo_client.c
	> Author: 
	> Mail: 
	> Created Time: Sat 06 Apr 2019 19:35:33 CST
 ************************************************************************/

#include "./common/common.h"

int main() {
    int shmid = 0;
    shmid = create_shm(2048);
    void *addr = shmat(shmid, NULL, 0);

    while(1) {
        usleep(2000);
        printf("%s", addr);
        fflush(stdout);
    }

    shmdt(addr);
    destroy(shmid);
    return 0;
}

