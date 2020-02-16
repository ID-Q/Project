/*************************************************************************
	> File Name: threadpool.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  3/ 7 14:00:31 2019
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#ifdef _DEBUG
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

//需要引入状态的头文件--条件变量和互斥锁
#include "../condition/condition.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>

/*封装线程池中的对象需要执行的任务对象*/
typedef struct task {
    void *(*run)(void *args);  //函数指针，需要执行的任务
    void *argv;                 //参数
    struct task *next;         //任务队列中下一个任务
}task_t;


/*下面是线程池结构体*/
typedef struct threadpool {
    condition_t ready;    //状态量
    task_t *first;        //任务队列中第一个任务
    task_t *last;         //任务队列中最后一个任务
    pthread_t *tid;       //指向线程id的指针
    int counter;          //线程池中已有线程数
    int idle;             //线程池中空闲线程数
    int max_threads;      //线程池最大线程数
    int quit;             //是否退出标志 1/0
}threadpool_t;


/*线程池初始化*/
void threadpool_init(threadpool_t *pool, int idle_threads, int max_threads);

/*往线程池中加入任务*/
int threadpool_add_task(threadpool_t *pool, void *(*run)(void *args), void *argv);

/*摧毁线程池*/
void threadpool_destroy(threadpool_t *pool);

#endif
