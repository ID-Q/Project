/*************************************************************************
	> File Name: condition.h
	> Author: zhengdongqi
	> Mail:1821260963@qq.com 
	> Created Time: 四  3/ 7 13:54:38 2019
 ************************************************************************/

#ifndef _CONDITION_H
#define _CONDITION_H

#include <pthread.h>
#include <stdio.h>
#include <time.h>

/*封装一个互斥量和条件变量作为状态*/
typedef struct condition {  
    pthread_mutex_t pmutex;//互斥量
    pthread_cond_t pcond;//条件变量
}condition_t;

/*对状态的操作函数*/
/*初始化*/
int condition_init(condition_t *cond);
/*加锁*/
int condition_lock(condition_t *cond);
/*解锁*/
int condition_unlock(condition_t *cond);
/*条件等待*/
int condition_wait(condition_t *cond);
/*计时等待*/
int condition_timedwait(condition_t *cond, const struct timespec *abstime);
/*唤醒一个睡眠线程*/
int condition_signal(condition_t* cond);
/*唤醒所有睡眠线程*/
int condition_broadcast(condition_t *cond);
/*销毁互斥锁和条件变量*/
int condition_destroy(condition_t *cond);

#endif
