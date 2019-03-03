/*************************************************************************
	> File Name: testcase3.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 18:20:22 2019
 ************************************************************************/

#ifndef _TESTCASE3_H
#define _TESTCASE3_H
#include "test.h"
#include "testfunc3.h"
TEST(test, sum_func) {
    EXPECT(sum(1), 6);
    EXPECT(sum(4), 0);
    EXPECT(sum(5), 0);
}

#endif
