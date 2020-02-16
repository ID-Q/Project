#!/usr/bin/env python
# coding=utf-8
import tensorflow as tf
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#matplotlib inline
# 数据集下载
(train_image, train_lable), (test_image, test_lable) = tf.keras.datasets.fashion_mnist.load_data()

print(train_image.shape)
print(train_lable.shape)
print(test_image.shape)
print(test_lable.shape)
plt.imshow(train_image[0])

train_image = train_image/255
test_image = test_image/255

model = tf.keras.Sequential()
model.add(tf.keras.layers.Flatten(input_shape=(28, 28)))
model.add(tf.keras.layers.Dense(128, activation='relu'))
model.add(tf.keras.layers.Dense(10, activation='softmax'))

model.compile(optimizer = 'adam',
             loss = 'sparse_categorical_crossentropy',
             metrics = ['acc']
)

model.fit(train_image, train_lable, epochs = 5)
model.evaluate(test_image, test_lable)
