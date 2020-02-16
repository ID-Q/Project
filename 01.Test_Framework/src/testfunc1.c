/*************************************************************************
	> File Name: testfunc2.c
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 18:27:57 2019
 ************************************************************************/

#include<stdio.h>
#include "../include/test.h"
#include "../include/testfunc2.h"

int is_prime(int x) {
    if (x <= 1) return 0;
    for (int i = 2; i * i <= x; i++) {
        if (x % i == 0) return 0;
    }
    return 1;
}
