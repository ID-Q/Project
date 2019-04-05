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
