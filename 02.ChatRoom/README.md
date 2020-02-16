# 聊天室涉及知识点汇总

## OSI七层参考模型

|  参考模型  |           各层解释           |          对应设备          |      典型协议       |
| :--------: | :--------------------------: | :------------------------: | :-----------------: |
|   应用层   |      为应用程序提供服务      |           计算机           |      DHCP、DNS      |
|   表示层   |    数据格式转换、数据加密    |           计算机           |    Telnet、SNMP     |
|   会话层   |     建立、管理、维护会话     |           计算机           |      SMTP、SSH      |
|   传输层   | 建立、管理、维护端到端的连接 |         进程和接口         |      TCP、UDP       |
|   网络层   |       IP地址与路由选择       | 路由器、防火墙、多层交换机 |    IP、ICMP、ARP    |
| 数据链路层 |    提供介质访问和链路管理    |     网卡、网桥、交换机     |    PPP、Ethernet    |
|   物理层   |            物理层            |    中继器、集线器、网线    | IEEE802、[1A,2..11] |

| OSI参考模型 | 协议数据单元            |
| ----------- | ----------------------- |
| 应用层      | 报文message             |
| 表示层      | 报文message             |
| 会话层      | 报文message             |
| 传输层      | 报文段segment用户数据报 |
| 网络层      | IP数据报（分组）        |
| 数据链路层  | 帧frame                 |
| 物理层      | 比特bit                 |

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

- 服务端

- ```c
  int socket_create(int port) {
      int _socket_fd;
      struct sockaddr_in _server_addr;
      //创建套接字
      if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
          perror("socket");
          exit(-1);
      }
      //准备网络通信地址
      memset(&_server_addr, 0, sizeof(_server_addr));//数据初始化清零
      _server_addr.sin_family = AF_INET;//设置为TCP通讯
      _server_addr.sin_port = htons(port);//端口
      _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//IP地址
      //绑定连接
      if (bind(_socket_fd, (struct sockaddr*)&_server_addr, sizeof(struct sockaddr)) < 0) {
          perror("bind");
          exit(-1);
      }
      //设置监听
      if (listen(_socket_fd, 20) < 0) {
          perror("listen");
          close(_socket_fd);
          exit(-1);
      }
      return _socket_fd;
  }
  ```

- 客户端

- ```c
  int socket_connect(int port, char *host) {
      int _socket_fd;
      struct sockaddr_in _client_addr;
      //创建套接字
      if ((_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
          perror("socket");
          close(_socket_fd);
          return -1;
      }
      //设置服务器
      memset(&_client_addr, 0, sizeof(_client_addr));//数据初始化清零
      _client_addr.sin_family = AF_INET;//设置为TCP通讯
      _client_addr.sin_port = htons(port);//端口
      _client_addr.sin_addr.s_addr = inet_addr(host);//IP地址
      //链接服务器
      if (connect(_socket_fd, (struct sockaddr *)&_client_addr, sizeof(_client_addr)) < 0) {
          perror("connect");
          close(_socket_fd);
          return 1;
      }
      return _socket_fd;
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

- ```c
  int close(int fd);//关闭相应的套接字
  ```