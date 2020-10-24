#!/usr/bin/env python
# coding=utf-8

# 为了解决SGD随机梯度下降算法的缺点，引入了Momentum
# v表示在梯度方向上的受力
import numpy as np


class Momentum:
    def __init__(self, learning_rate=0.01, momentum=0.9):
        self.learning_rate = learning_rate
        self.momentum = momentum
        self.v = None

    def update(self, params, grads):
        if self.v is None:
            self.v = {}
            for key, value in params.items():
                self.v[key] = np.zeros_like(value) # 生成和value一样的数组（和梯度一样的数组）

            for key in params.keys():
                self.v[key] = self.momentum * self.v[key] - self.learning_rate * grads[key]
                params[key] += self.v[key]

"""实例变量v会保存物体的速度，初始化时什么v都不保存。 在第一次调用update（）时，v会以字典的形式保存与参数结构相同的数据"""
