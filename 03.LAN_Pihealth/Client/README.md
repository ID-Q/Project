# 客户端

- common 公共文件

  > 将函数封装，结构体，宏定义等放入其中 

- config 配置文件

  > 将函数需要的相应参数放入配置文件，很大的程度上减少的修改代码的几率

- log 日志文件

  > 将提取到的系统健康信息，按照类型的命名，存放起来

- logbackup 压缩文件

  > 考虑到监测文件可能过大，因此在文件过大时将其压缩

- script 监测脚本

  > 几种监测脚本，提取系统状况

# 程序思路及应用函数

## 进程间共享内存

```
在Linux中，每个进程都有属于自己的进程控制块（PCB）和地址空间（Addr Space），并且都有一个与之对应的页表，负责将进程的虚拟地址与物理地址进行映射，通过内存管理单元（MMU）进行管理。两个不同的虚拟地址通过页表映射到物理空间的同一区域，它们所指向的这块区域即共享内存。
```

共享内存的通信原理示意图：

![](http://zhengdongqi.oss-cn-beijing.aliyuncs.com/2020-02-13/48CCAE4A8C6F4857B06962E2B144A273.jpg)

```
对于上图我的理解是：当两个进程通过页表将虚拟地址映射到物理地址时，在物理地址中有一块共同的内存区，即共享内存，这块内存可以被两个进程同时看到。这样当一个进程进行写操作，另一个进程读操作就可以实现进程间通信。但是，我们要确保一个进程在写的时候不能被读，因此我们使用信号量来实现同步与互斥。

对于一个共享内存，实现采用的是引用计数的原理，当进程脱离共享存储区后，计数器减一，挂架成功时，计数器加一，只有当计数器变为零时，才能被删除。当进程终止时，它所附加的共享存储区都会自动脱离。
```

```c
//创建共享内存

int shmget(key_t key, size_t size, int shmflg);
[参数key]：由ftok生成的key标识，标识系统的唯一IPC资源。

[参数size]：需要申请共享内存的大小。在操作系统中，申请内存的最小单位为页，一页是4k字节，为了避免内存碎片，我们一般申请的内存大小为页的整数倍。

[参数shmflg]：如果要创建新的共享内存，需要使用IPC_CREAT，IPC_EXCL，如果是已经存在的，可以使用IPC_CREAT或直接传0。

[返回值]：成功时返回一个新建或已经存在的的共享内存标识符，取决于shmflg的参数。失败返回-1并设置错误码。

//挂接共享内存

void *shmat(int shmid, const void *shmaddr, int shmflg);
[参数shmid]：共享存储段的标识符。

[参数*shmaddr]：shmaddr = 0，则存储段连接到由内核选择的第一个可以地址上（推荐使用）。

[参数shmflg]：若指定了SHM_RDONLY位，则以只读方式连接此段，否则以读写方式连接此段。

[返回值]：成功返回共享存储段的指针（虚拟地址），并且内核将使其与该共享存储段相关的shmid_ds结构中的shm_nattch计数器加1（类似于引用计数）；出错返回-1。

//去关联共享内存

当一个进程不需要共享内存的时候，就需要去关联。该函数并不删除所指定的共享内存区，而是将之前用shmat函数连接好的共享内存区脱离目前的进程。

int shmdt(const void *shmaddr);
[参数*shmaddr]：连接以后返回的地址。

[返回值]：成功返回0，并将shmid_ds结构体中的 shm_nattch计数器减1；出错返回-1。

//销毁共享内存

int shmctl(int shmid, int cmd, struct shmid_ds *buf);
[参数shmid]：共享存储段标识符。

[参数cmd]：指定的执行操作，设置为IPC_RMID时表示可以删除共享内存。

[参数*buf]：设置为NULL即可。

[返回值]：成功返回0，失败返回-1。
```

## 脚本语言

## TCP协议

- TCP是面向连接的，提供全双工的服务：数据流可以双向传输。也是点对点的，即在单个发送方与单个接收方之间的连接
- TCP报文段结构

  - 序号

    - TCP的序号是数据流中的字节数，不是分组的序号。表示该报文段数据字段首字节的序号

  - 确认号

    - TCP使用累积确认，确认号是第一个未收到的字节序号，表示希望接收到的下一个字节

  - 首部长度

    - 通常选项字段为空，所以一般TCP首部的长度是20字节

  - 选项字段

    - 用于发送方与接收方协商MSS(最大报文段长)，或在高速网络环境下用作窗口调节因子

  - 标志字段

    - ACK

      - 指示确认字段中的值是有效的

    - RST,SYN,FIN

      - 连接建立与拆除

    - PSH

      - 指示接收方应立即将数据交给上层

    - URG

      - 报文段中存在着(被发送方的上层实体置位)“紧急”的数据

  - 接收窗口

    - 用于流量控制（表示接收方还有多少可用的缓存空间）

- 流量控制

  -  如果应用程序读取数据相当慢，而发送方发送数据太多、太快，会很容易使接收方的接收缓存溢出，流量控制就是用来进行发送速度和接收速度的匹配。发送方维护一个“接收窗口”变量，这个变量表示接收方当前可用的缓存空间

- 连接管理

  - 3次握手

    - 客户端向服务器发送SYN报文段（不包含应用层数据，首部的一个标志位(即SYN比特)被置位，客户端随机化选择(避免攻击)一个起始序号x）
    - 服务器为该TCP连接分配TCP缓存和变量，返回一个SYNACK报文段（也不包含应用层数据，SYN比特被置为1，ACK为x+1，服务器选择自己的初始序列y）
    - 客户机为该连接分配缓存和变量，**返回一个对SYNACK报文段进行确认的报文段**（因为连接已经建立了，所以SYN比特被置为0）

  - 4次挥手

    - 客户端发送一个FIN报文（首部中的FIN比特被置位）
    - 服务器返回一个对FIN报文的确认报文
    - 服务器发送一个FIN报文（首部中的FIN比特被置位）
    - 客户端返回一个对FIN报文的确认报文

- 拥塞控制

  - TCP拥塞控制

    - 由于IP层不向端系统提供显示的网络拥塞反馈，所以TCP必须使用端到端拥塞控制，而不是网络辅助拥塞控制
    - 两个拥塞指示

      - 3次冗余ACK
      - 超时

    - TCP拥塞控制算法包括三个主要部分

      - 加性增、乘性减

        - - 加性增：缓慢增加CongWin，每个RTT增加1个MSS，线性增长（拥塞避免）
- 乘性减：发生丢包时，设置CongWin = CongWin/2（不低于1个MSS），从而控制发送速度 

  - 慢启动

    - TCP连接开始时，CongWin的初始值为1个MSS，指数型增长

      - 对拥塞指示作出反应

        - - 3次冗余ACK：CongWin = CongWin/2，然后线性增加（拥塞避免）
- 超时：CongWin被设置为1个MSS，然后指数增长，直到CongWin达到超时前的一半为止

## UDP协议

出于下列原因可能使用UDP：

- 应用层能更好地控制要发送的数据和发送时间（TCP拥塞时会遏制发送方发送）
- 无需建立连接
- 无连接状态（TCP需要维护连接状态，包括接收和发送缓存、拥塞控制参数、序号与确认号的参数）
- 分组首部开销小（**每个TCP报文段有20字节的首部开销，而UDP仅有8字节的开销**）

## Socket通信

- 什么是Socket？

  ```c
  /*
  Socket(套接字)，用来描述IP地址和端口，是通信链的句柄，应用工程序可以通过Socket向网络发送请求或者应答网络请求！Socket是支持TCP/IP协议的网络通讯的基本单元，是对网络通信过程中断点的抽象表示，包含了进行网络通信所必须的五种信息：连接使用的协议；本地主机的IP地址；本地远程的协议端口；远地主机的IP地址以及远地进程的协议端口
  */
  ```

- 三次握手

  ![](http://zhengdongqi.oss-cn-beijing.aliyuncs.com/2020-02-13/4FDCA596990F43D78E688E00CDADB1E8.jpg)

  - **第一次握手**：起初两端都处于CLOSED关闭状态，Client将标志位SYN置为1，随机产生一个值seq=x，并将该数据包发送给Server，Client进入SYN-SENT状态，等待Server确认；
  - **第二次握手**：Server收到数据包后由标志位SYN=1得知Client请求建立连接，Server将标志位SYN和ACK都置为1，ack=x+1，随机产生一个值seq=y，并将该数据包发送给Client以确认连接请求，Server进入SYN-RCVD状态，此时操作系统为该TCP连接分配TCP缓存和变量；
  - **第三次握手**：Client收到确认后，检查ack是否为x+1，ACK是否为1，如果正确则将标志位ACK置为1，ack=y+1，并且此时操作系统为该TCP连接分配TCP缓存和变量，并将该数据包发送给Server，Server检查ack是否为y+1，ACK是否为1，如果正确则连接建立成功，Client和Server进入ESTABLISHED状态，完成三次握手，随后Client和Server就可以开始传输数据。

- 四次挥手

  ![](http://zhengdongqi.oss-cn-beijing.aliyuncs.com/2020-02-13/0F3BE741A998480F86A4A734C7EAC604.jpg)

  - 起初A和B处于**ESTAB-LISHED状态**——A发出连接释放报文段并处于**FIN-WAIT-1状态**
  - B发出确认报文段且进入**CLOSE-WAIT状态**——A收到确认后，进入**FIN-WAIT-2状态**
  - B发出连接释放报文段且进入**LAST-ACK状态**——A发出确认报文段且进入**TIME-WAIT状态**——
  - B收到确认报文段后进入**CLOSED状态**——A经过等待计时器时间2MSL后，进入**CLOSED状态**。

## Socket编程

![](http://zhengdongqi.oss-cn-beijing.aliyuncs.com/2020-02-13/48FBEDE78C3B4F8B83BAD88B97A0E4D2.jpg)

- Socket—TCP

- ```c
  /*创建一个socket 类型TCP*/
  int socket_create_tcp(int port) {
      int socket_fd;
      struct sockaddr_in socket_addr;
      //创建套接字
      socket_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (socket_fd < 0) {
          DBG("common.c->socket_create_tcp\033[31m创建套接字失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_create_tcp\033[31m创建套接字失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      //设置服务器
      memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
      socket_addr.sin_family = AF_INET;//设置协议族
      socket_addr.sin_port = htons(port);//端口
      socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
      //端口重用
      int reuse = 1;
      if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
          DBG("common.c->socket_create_tcp\033[31m设置端口重用失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_create_tcp\033[31m设置端口重用失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      //绑定连接
      if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(struct sockaddr)) < 0) {
          DBG("common.c->socket_create_tcp\033[31m绑定失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_create_tcp\033[31m绑定失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      //设置监听
      if (listen(socket_fd, 20) < 0) {
          DBG("common.c->socket_create_tcp\033[31m监听失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_create_tcp\033[31m监听失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      return socket_fd;
  }
  /*进行 socket 连接 类型TCP*/
  int socket_connect_tcp(int port, char *host, int sec, double usec) {
      int socket_fd;
      struct sockaddr_in socket_addr;
      //创建套接字
      socket_fd = socket(AF_INET, SOCK_STREAM, 0);
      if (socket_fd < 0) {
          DBG("common.c->socket_connect_tcp\033[31m创建套接字失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_connect_tcp\033[31m创建套接字失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      //设置套接字为非阻塞态
      int opt = 1;
      if (ioctl(socket_fd, FIONBIO, &opt) < 0) {
          DBG("common.c->socket_connect_tcp\033[31m设置非阻塞失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_connect_tcp\033[31m设置非阻塞失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      //设置服务器
      memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
      socket_addr.sin_family = AF_INET;//设置协议族
      socket_addr.sin_port = htons(port);//端口
      socket_addr.sin_addr.s_addr = inet_addr(host);//IP地址
      //链接服务器
      if (connect(socket_fd, (struct sockaddr *)&socket_addr, sizeof(socket_addr)) < 0) {
          if (errno == EINPROGRESS) {
              int error;
              int len = sizeof(int);
              struct timeval tv;
              tv.tv_sec  = sec;//最多等待时间-秒
              tv.tv_usec = usec * 1000000;//最多等待时间-微秒
              fd_set set;//设置一个套接字集合
              FD_ZERO(&set);//将套节字集合清空
              FD_SET(socket_fd, &set);//加入套节字到集合
              if(select(socket_fd + 1, NULL, &set, NULL, &tv) > 0) {
                  getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);//套接字，层次，获取错误状态并清除，缓冲区，长度值
                  if(error != 0) {//有错误
                      close(socket_fd);
                      return -1;
                  }
              } else {//表示超时
                  close(socket_fd);
                  return -1;
              }
          } else {
              DBG("common.c->socket_connect_tcp\033[31m连接失败: %s\033[0m\n", strerror(errno));
              write_log(conf.Sys_Log, "common.c->socket_connect_tcp\033[31m连接失败: %s\033[0m", strerror(errno));
              close(socket_fd);
              return -1;
          }
      }
      //设置为阻塞态
      opt = 0;
      if (ioctl(socket_fd, FIONBIO, &opt) < 0) {
          DBG("common.c->socket_connect_tcp\033[31m设置为阻塞态失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_connect_tcp\033[31m设置为阻塞态失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      return socket_fd;
  }
  ```

- Socket—UDP

- ```c
  int socket_create_udp(int port) {
      int socket_fd;
      struct sockaddr_in socket_addr;
      //创建套接字
      socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
      if (socket_fd < 0) {
          DBG("common.c->socket_create_udp创建套接字失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_create_udp\033[31m创建套接字失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      //设置服务器
      memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
      socket_addr.sin_family = AF_INET;//设置协议族
      socket_addr.sin_port = htons(port);//端口
      socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
      //端口重用
      int reuse = 1;
      if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1) {
          DBG("common.c->socket_create_udp\033[31m设置端口重用失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_create_udp\033[31m设置端口重用失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
   //绑定连接
      if (bind(socket_fd, (struct sockaddr*)&socket_addr, sizeof(struct sockaddr)) < 0) {
          DBG("common.c->socket_create_udp\033[31m绑定失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_create_udp\033[31m绑定失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      return socket_fd;
  }
  /*进行 socket 连接 类型UDP*/
  int socket_connect_udp(int port, char *host, char *buff) {
      int socket_fd;
      struct sockaddr_in socket_addr;
      //创建套接字
      socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
      if (socket_fd < 0) {
          DBG("common.c->socket_connect_udp\033[31m创建套接字失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_connect_udp\033[31m创建套接字失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      //设置服务器
      memset(&socket_addr, 0, sizeof(socket_addr));//数据初始化清零
      socket_addr.sin_family = AF_INET;//设置协议族
      socket_addr.sin_port = htons(port);//端口
      socket_addr.sin_addr.s_addr = inet_addr(host);//IP地址
      
      int socket_udp;
      socket_udp = sendto(socket_fd, buff, sizeof(buff), 0, (void *)&socket_addr, sizeof(socket_addr));
      if (socket_udp < 0) {
          DBG("common.c->socket_connect_udp\033[31m发送失败: %s\033[0m\n", strerror(errno));
          write_log(conf.Sys_Log, "common.c->socket_connect_udp\033[31m发送失败: %s\033[0m", strerror(errno));
          close(socket_fd);
          return -1;
      }
      close(socket_fd);
      return 1;
  }
  ```

- 函数使用

- 创建套接字

- ```c
  int socket(int protofamily, int type, int protocol);
  //protofamily：即协议域，又称为协议族（family）。常用的协议族有，AF_INET(IPV4)、AF_INET6(IPV6)、AF_LOCAL（或称AF_UNIX，Unix域socket）、AF_ROUTE等等。协议族决定了socket的地址类型，在通信中必须采用对应的地址，如AF_INET决定了要用ipv4地址（32位的）与端口号（16位的）的组合、AF_UNIX决定了要用一个绝对路径名作为地址。
  
  //type：指定socket类型。常用的socket类型有，SOCK_STREAM、SOCK_DGRAM、SOCK_RAW、SOCK_PACKET、SOCK_SEQPACKET等等
  
  //protocol：故名思意，就是指定协议。常用的协议有，IPPROTO_TCP、IPPTOTO_UDP、IPPROTO_SCTP、IPPROTO_TIPC等，它们分别对应TCP传输协议、UDP传输协议、STCP传输协议、TIPC传输协议
  ```

- 绑定套接字

- ```c
  int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  /*
  sockfd：即socket描述字，它是通过socket()函数创建了，唯一标识一个socket。bind()函数就是将给这个描述字绑定一个名字。
  addr：一个const struct sockaddr *指针，指向要绑定给sockfd的协议地址。这个地址结构根据地址创建socket时的地址协议族的不同而不同，如ipv4对应的是： 
  struct sockaddr_in {
      sa_family_t    sin_family; //address family: AF_INET
      in_port_t      sin_port;   //port in network byte order
      struct in_addr sin_addr;   //internet address
  };
  
  //Internet address
  struct in_addr {
      uint32_t       s_addr; //address in network byte order
  };
  ipv6对应的是： 
  struct sockaddr_in6 { 
      sa_family_t     sin6_family;   //AF_INET6
      in_port_t       sin6_port;     //port number
      uint32_t        sin6_flowinfo; //IPv6 flow information
      struct in6_addr sin6_addr;     //IPv6 address
      uint32_t        sin6_scope_id; //Scope ID (new in 2.4)
  };
  
  struct in6_addr { 
      unsigned char   s6_addr[16];   //IPv6 address 
  };
  Unix域对应的是： 
  #define UNIX_PATH_MAX    108
  
  struct sockaddr_un { 
      sa_family_t sun_family;               //AF_UNIX 
      char        sun_path[UNIX_PATH_MAX];  //pathname
  };
  addrlen：对应的是地址的长度。
  */
  ```

- 创建监听

- ```c
  int listen(int sockfd, int backlog);
  /*listen函数的第一个参数即为要监听的socket描述字，第二个参数为相应socket可以排队的最大连接个数。socket()函数创建的socket默认是一个主动类型的，listen函数将socket变为被动类型的，等待客户的连接请求。*/
  ```

- 请求连接

- ```c
  int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
  /*connect函数的第一个参数即为客户端的socket描述字，第二参数为服务器的socket地址，第三个参数为socket地址的长度。客户端通过调用connect函数来建立与TCP服务器的连接。*/
  ```

- 接受请求

- ```c
  int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
  /*
  参数sockfd
  参数sockfd就是上面解释中的监听套接字，这个套接字用来监听一个端口，当有一个客户与服务器连接时，它使用这个一个端口号，而此时这个端口号正与这个套接字关联。当然客户不知道套接字这些细节，它只知道一个地址和一个端口号。
  参数addr
  这是一个结果参数，它用来接受一个返回值，这返回值指定客户端的地址，当然这个地址是通过某个地址结构来描述的，用户应该知道这一个什么样的地址结构。如果对客户的地址不感兴趣，那么可以把这个值设置为NULL。
  参数len
  如同大家所认为的，它也是结果的参数，用来接受上述addr的结构的大小的，它指明addr结构所占有的字节个数。同样的，它也可以被设置为NULL。
  */
  ```

- 读/写/发送/接收

- ```c
  #include <unistd.h>
  
  ssize_t read(int fd, void *buf, size_t count);
  ssize_t write(int fd, const void *buf, size_t count);
  
  #include <sys/types.h>
  #include <sys/socket.h>
  
  ssize_t send(int sockfd, const void *buf, size_t len, int flags);
  ssize_t recv(int sockfd, void *buf, size_t len, int flags);
  
  ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, 
                 const struct sockaddr *dest_addr, socklen_t addrlen);
  ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                   struct sockaddr *src_addr, socklen_t *addrlen);
  
  ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
  ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
  ```

- 关闭连接

  ```c
  int close(int fd);//关闭相应的套接字
  ```

## 端口重用

- ```c
  //在linux socket网络编程中，大规模并发TCP或UDP连接时，经常会用到端口复用：
  
   int opt = 1;
   if(setsockopt(sockfd, SOL_SOCKET,SO_REUSEADDR, (const void *) &opt, sizeof(opt))){
      perror("setsockopt");
      return -1;
  }
  /*那么什么是端口复用呢，如何理解呢，可以解释成如下： 
  在A机上进行客户端网络编程，假如它所使用的本地端口号是1234，如果没有开启端口复用的话，它用本地端口1234去连接B机再用本地端口连接C机时就不可以，若开启端口复用的话在用本地端口1234访问B机的情况下还可以用本地端口1234访问C机。若是服务器程序中监听的端口，即使开启了复用，也不可以用该端口望外发起连接了。*/
  
  SO_REUSEADDR和SO_REUSEPORT
  
  SO_REUSEADDR提供如下四个功能：
  
  SO_REUSEADDR 允许启动一个监听服务器并捆绑其众所周知端口，即使以前建立的将此端口用做他们的
          本地端口的连接仍存在。这通常是重启监听服务器时出现，若不设置此选项，则bind时将出错。
  
  SO_REUSEADDR 允许在同一端口上启动同一服务器的多个实例，只要每个实例捆绑一个不同的本地IP地址
              即可。对于TCP，我们根本不可能启动捆绑相同IP地址和相同端口号的多个服务器。
  
  SO_REUSEADDR 允许单个进程捆绑同一端口到多个套接口上，只要每个捆绑指定不同的本地IP地址即
              可。这一般不用于TCP服务器。
  
  SO_REUSEADDR 允许完全重复的捆绑：当一个IP地址和端口绑定到某个套接口上时，还允许此IP地址和端
              口捆绑到另一个套接口上。一般来说，这个特性仅在支持多播的系统上才有，而且只对UDP套
              接口而言（TCP不支持多播）。
    
  SO_REUSEPORT选项有如下语义：
  
  此选项允许完全重复捆绑，但仅在想捆绑相同IP地址和端口的套接口都指定了此套接口选项才行。
  
  如果被捆绑的IP地址是一个多播地址，则SO_REUSEADDR和SO_REUSEPORT等效。
  使用这两个套接口选项的建议：
  
  在所有TCP服务器中，在调用bind之前设置SO_REUSEADDR套接口选项；
  当编写一个同一时刻在同一主机上可运行多次的多播应用程序时，设置SO_REUSEADDR选项，并将本组的多播地址作为本地IP地址捆绑。
  ```

## 进程逻辑

```
父进程：登录、负责与服务器心跳连接、发送数据
						儿子进程：创建心跳、运行脚本、压缩数据
									孙子进程：心跳连接服务器检查服务器状态
```

