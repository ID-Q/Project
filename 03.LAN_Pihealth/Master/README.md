# 服务器

- common 公共文件

  > 将函数封装，结构体，宏定义等放入其中 

- config 配置文件

  > 将函数需要的相应参数放入配置文件，很大的程度上减少的修改代码的几率

- condition 状态变量

  > 在使用线程池时，考虑到线程间共享内存，因此使用状态量进行限制

- threadpool 线程池

  > 使用线程池提高并发度

- epoll IO多路复用

  > 使用多路复用提升效率

- node 使用链表

  > 使用链表存放用户信息

# 知识点总结

## 线程池

线程池的优点：典型的生产者与消费者模型

```c

第一：降低资源消耗。通过重复利用已创建的线程降低线程创建和销毁造成的消耗。
第二：提高响应速度。当任务到达时，任务可以不需要等到线程创建就能立即执行。
第三：提高线程的可管理性。线程是稀缺资源，如果无限制地创建，不仅会消耗系统资源。
//1、线程的创建和销毁由线程池维护，一个线程在完成任务后并不会立即销毁，而是由后续的任务复用这个线程，从而减少线程的创建和销毁，节约系统的开销；
//2、线程池旨在线程的复用，这就可以节约我们用以往的方式创建线程和销毁所消耗的时间，减少线程频繁调度的开销，从而节约系统资源，提高系统吞吐量；
//3、在执行大量异步任务时提高了性能；
```

```c
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
```



## IO多路复用

[https://blog.csdn.net/NickDeCodes/article/details/104233102]: 

## 修改守护进程

守护进程名字pihealthd

服务名字pihealthd.master

将**ptintf**修改成**DBG**

`sed -i s/printf/GBD`注意**sprint**也会被修改

守护进程里有加两行代码以防失败

```c
int pid = fork();
    if(pid > 0) exit(0);
```

需要三个文件：

首先在**lib/systemd/system**创建一个服务文件**pihealthd_master.service**

不要找错文件夹

文件内容:

````bash
[Unit]
Description=pihealth.master-1.0
After=syslog.target network.target remote-fs.target nss-lookup.target


[Service]
Type=forking
ExecStart=/usr/bin/pihealth/pihealthd.master.start
ExecStot=/usr/bin/pihealth/pihealthd.client.stop


[Install]
WantedBy=multi-user.target

````

在**usr/bin**下创建一个文件**pihealth**

然后就是创建两个**pihealthd.master.start**　和　**pihealthd.master.stop**

**start**的头文件不能加前缀任何注释。　**stop**可以加但是最好不要加

注意有个**cd**操作是不可以扩展的

**pihealthd.master.start**

```bash
#!/bin/bash
if [[ ! -e /etc/pihealth.pid ]]; then
    touch /etc/pihealth.pid
fi 

pre_pid=`cat /etc/pihealth.pid`

if test -n $pre_pid ;then 
    ps -ef |grep -w ${pre_pid} |grep pihealth > /dev/null 
    if [[ $? == 0 ]]; then
        echo "Pihealth has already started."
        exit 0
    else
        echo "Pihealth is starting."
	cd /home/Project/Socket_Pro/Master/
	./pihealth.master  
        echo "Pihealth.master started."
    fi 
else 
    echo "Pihealth.master is starting."
    cd /home/Project/Socket_Pro/Master/
	./pihealth.master
    echo "Pihealthd.master started."
fi 
pid=`ps -ef | awk '{if ($8 == "pihealth") print $2}'`
echo $pid > /etc/pihealth.pid

```

**pihealthd.master.stop**

````bash
#!/bin/bash  
pid=`ps -ef |awk '{if ($8 == "pihealth") print $2}'`
kill -9  $pid
echo "Stopped."
````