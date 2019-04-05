/*************************************************************************
	> File Name: testfunc3.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 18:29:53 2019
 ************************************************************************/

#include<stdio.h>
#include "test.h"
#include "testfunc3.h"

int sum(int a) {
    int num = 1;
    if (a < 0) return -1;
    for (int i = 1; i <= a; i++)
    num += i;
    return num;
}
