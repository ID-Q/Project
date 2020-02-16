/*************************************************************************
	> File Name: echo_server.c
	> Author: 
	> Mail: 
	> Created Time: Sat 06 Apr 2019 19:35:45 CST
 ************************************************************************/

#include "./common/common.h"

int main() {
    int shmid = 0;
    shmid = create_shm(2048);
    void *addr = shmat(shmid, NULL, 0);

    while (1) {
        scanf("%[^\n]s", addr);
        getchar();
        strcat(addr, "\n");
        fflush(stdin);
        usleep(2000);
        memset(addr, 0, sizeof(&addr));
    }
    shmdt(addr);
    destroy(shmid);
    return 0;
}

