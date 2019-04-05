/*************************************************************************
	> File Name: testcase2.h
	> Author: zhengdongqi
	> Mail: 
	> Created Time: 四  1/17 18:19:21 2019
 ************************************************************************/

#ifndef _TESTCASE2_H
#define _TESTCASE2_H
#include "test.h"
#include "testfunc2.h"
TEST(test, add_func) {
    EXPECT(add(1, 2), 3);
    EXPECT(add(3, 4), 7);
    EXPECT(add(999, 1), 1000);
}

#endif
