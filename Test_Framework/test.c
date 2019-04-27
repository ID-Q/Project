/*************************************************************************
	> File Name: test.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 18:11:20 2019
 ************************************************************************/

#include<stdio.h>
#include "test.h"
#include "testcase1.h"
#include "testcase2.h"
#include "testcase3.h"

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
