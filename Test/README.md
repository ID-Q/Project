# Test 简易测试框架

对于一个新的函数，或者逻辑结构来说，总会有你考虑不到的方面、或者说你想测试一下你的逻辑是否真的达到预期要求。这时你便需要一些测试用例，并需要相应的接口将测试用例传入进行测试。对于一些OJ网站来讲，测试框架是相当省时的。那么这次咱们就来写一个简易的测试框架。

首先测试框架，顾名思义，我们需要将一些测试用例传入，然后测试逻辑是否正常。这里就有一个很重要的问题，传入数据时是什么类型的数据，由于我们不确定需要的测试用例是什么类型的数据，那么我们又不能每次都重新写一个借口函数吧？

至此，有一种结构不需要明确你的参数类型，那就是宏。

在编写程序时，若你需要写一个通用的模块函数，那么宏定义是你目前最好的选择了；在这里我们运用宏替换来解决参数类型的问题。

**宏替换**

宏替换就是使用宏先声明一个人，定义好这个人是干什么事情的，在程序执行时，程序在预编译时就会先找到这个人，确定他的作用，并分配好他的任务。

```c
#define TEST(a, b) \
    void a##_haizeix_##b(struct TestData *); \
    __attribute__((constructor)) \
    void ADDFUNC_##a##_haizeix_##b() { \
        AddData(#a, #b, a##_haizeix_##b); \
    } \
    void a##_haizeix_##b(struct TestData *__data)

#define EXPECT(a, b) ({ \
    int temp; \
    printf("%s = %s %s\n", #a, #b, (temp =(a == b)) ? "Ture" : "False"); \
    __data->total += 1; \
    __data->expect += temp; \
})
```

接下来我们进行测试总是要有数据输出的，那么我们来定义一个结构体

```c
struct TestData {//记录数据
    int total;
    int expect;
};

typedef void (*test_func)(struct TestData *);//定义一个函数指针 函数参数是TestData中的
struct Data {
    const char *case_a, *case_b;
    test_func func;// func函数
    struct Data *next; //定义一个指针
};
```

我们是使用结构体来存放数据，和函数内容。

至此我们的**test.h**就基本完成了

```c
/*************************************************************************
	> File Name: test.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 18:05:27 2019
 ************************************************************************/

#ifndef _TEST_H
#define _TEST_H
#include <stdlib.h>

struct TestData {//记录数据
    int total;
    int expect;
};

typedef void (*test_func)(struct TestData *);//定义一个函数指针 函数参数是TestData中的
struct Data {
    const char *case_a, *case_b;
    test_func func;// func函数
    struct Data *next; //定义一个指针
};

void AddData(const char *a, const char *b, test_func func); 

#define TEST(a, b) \
    void a##_haizeix_##b(struct TestData *); \
    __attribute__((constructor)) \
    void ADDFUNC_##a##_haizeix_##b() { \
        AddData(#a, #b, a##_haizeix_##b); \
    } \
    void a##_haizeix_##b(struct TestData *__data)

#define EXPECT(a, b) ({ \
    int temp; \
    printf("%s = %s %s\n", #a, #b, (temp =(a == b)) ? "Ture" : "False"); \
    __data->total += 1; \
    __data->expect += temp; \
})

int RUN_ALL_TEST();

#endif
```

接下来我们来写一些核心函数**test.c**

首先每次有测试数据，我们都需要向结构体中添加数据。因此我们使用一个链表结构。

```c
static  struct Data *Data_head = NULL;//开一个头节点

void AddData(
    const char *a,
    const char *b,
    test_func func
) {
    struct Data *p = (struct Data *)malloc(sizeof(struct Data));//开一个结构体的内存空间
    p->case_a = a;
    p->case_b = b;
    p->func = func;
    p->next = Data_head;//指向下一个节点
    Data_head = p;//头更新
    return ;
}
```

去运行测试总要有个带头人吧

```c
int RUN_ALL_TEST() {
    struct Data ret;
    ret.next = NULL;
    for (struct Data *p = Data_head, *q; p; p = q) {
        q = p->next;
        p->next = ret.next;
        ret.next = p;
    }
    Data_head = ret.next;
    char color[2][100] = {
        "[  \033[0;31m%.2lf%%  \033[0m] total : %d  expect : %d\n",
        "[  \033[1;32m%.2lf%%  \033[0m] total : %d  expect : %d\n",
    };
    for (struct Data *p = Data_head; p; p = p->next) {
        struct TestData data = {0, 0};
        printf("[%s %s]\n", p->case_a, p->case_b);
        p->func(&data);
        printf(color[data.expect / data.total], (double)1.0 * data.expect / data.total * 100, data.total, data.expect);
    }
    return 0;
}
```

在输出过程中，我们定义一个颜色数组，将输出结构加上颜色。

在**main.c**中我们直接召唤带头人就好了

```c
/*************************************************************************
	> File Name: main.cpp
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 17:02:34 2019
 ************************************************************************/
#include <stdio.h>
#include "test.h"

int main() {

    return RUN_ALL_TEST();
}
```

接下来我们分别在

```c
testfunc.c testfunc.h //在这里进行测试函数，和声明
testcase.c //在这里输入测试用例
```

