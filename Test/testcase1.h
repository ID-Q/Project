/*************************************************************************
	> File Name: testcase1.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 18:17:25 2019
 ************************************************************************/

#ifndef _TESTCASE1_H
#define _TESTCASE1_H
#include "test.h"
#include "testfunc1.h"
TEST(test, is_prime_func) {
    EXPECT(is_prime(2), 0);
    EXPECT(is_prime(-2), 0);
    EXPECT(is_prime(15), 0);
    EXPECT(is_prime(9973), 1);
}

#endif
