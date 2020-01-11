服务器

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



修改守护进程

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