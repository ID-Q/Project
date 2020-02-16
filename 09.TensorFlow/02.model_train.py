#!/usr/bin/env python
# coding=utf-8
import tensorflow as tf
try:
    import tensorflow.python.keras as keras
except:
    import tensorflow.keras as keras
from tensorflow.python.keras import layers

mnist = keras.datasets.mnist
(x_train,y_train),(x_test,y_test) = mnist.load_data()
x_train, x_test = x_train/255.0, x_test/255.0  # 除以 255 是为了归一化。

# Sequential 用于建立序列模型
# Flatten 层用于展开张量，input_shape 定义输入形状为 28x28 的图像，展开后为 28*28 的张量。
# Dense 层为全连接层，输出有 128 个神经元，激活函数使用 relu。
# Dropout 层使用 0.2 的失活率。
# 再接一个全连接层，激活函数使用 softmax，得到对各个类别预测的概率。
model = keras.Sequential()
model.add(layers.Flatten(input_shape=(28,28)))
model.add(layers.Dense(128,activation="relu"))
model.add(layers.Dropout(0.2))
model.add(layers.Dense(10,activation="softmax"))

# 优化器选择 Adam 优化器。
# 损失函数使用 sparse_categorical_crossentropy，
# 还有一个损失函数是 categorical_crossentropy，两者的区别在于输入的真实标签的形式，
# sparse_categorical 输入的是整形的标签，例如 [1, 2, 3, 4]，categorical 输入的是 one-hot 编码的标签。
model.compile(optimizer="adam",
              loss="sparse_categorical_crossentropy",
              metrics=['accuracy'])

# fit 用于训练模型，对训练数据遍历一次为一个 epoch，这里遍历 5 次。
# evaluate 用于评估模型，返回的数值分别是损失和指标。
model.fit(x_train,y_train,epochs=5)
model.evaluate(x_test,y_test)


