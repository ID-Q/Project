/*************************************************************************
	> File Name: test.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 18:05:27 2019
 ************************************************************************/

#ifndef _TEST_H
#define _TEST_H
#include <stdlib.h>

/*
定义测试用例需要的结构体->便于在后期计算测试正确率
1、包含了测试用例总数total
2、包含了预期正确的数量expect
*/
struct TestData {
    int total;
    int expect;
};

/*定义一个函数指针test_func 函数参数是TestData指针类型，为之后测试函数制作函数接口*/
typedef void (*test_func)(struct TestData *);

/*
这里我们定义一个测试数据的结构体，我们是使用结构体来存放数据，和函数内容
1、包含了测试需要的测试条件case_a, case_b
2、包含预留的函数接口test_func func
3、结点指向下一个结点，所以用一个next指针struct Data *next
*/
struct Data {
    const char *case_a, *case_b;
    test_func func;
    struct Data *next;
};

void AddData(const char *a, const char *b, test_func func); 

/*
宏定义中
##代表了字符连接，例如a##_haizeix_##b -> a_haizeix_b
#代表将后面的数据转成字符串格式，例如#a, #b -> 'a', 'b'
__attribute__((constructor))提升函数的优先级，也就是在调用主函数之前优先调用一次
*/
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
