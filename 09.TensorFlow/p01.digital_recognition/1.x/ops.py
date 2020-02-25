#!/usr/bin/env python
# coding=utf-8
import tensorflow as tf

def relu(x):
    return tf.nn.relu(x)

def pooling(
    x, 
    name = "pooling",
    ksize = [1, 2, 2, 1], 
    strides = [1, 2, 2, 1]
):
    return tf.nn.max_pool(x, ksize = ksize, strides = strides, padding = "SAME", name = name)

def conv2d(x, kernel = [5, 5], output_size = 32, scope = None):
    initializer1 = tf.truncated_normal_initializer(stddev = 1e-4)
    initializer2 = tf.constant_initializer(0.1)
    shape = x.get_shape().as_list()

    with tf.variable_scope(scope or "conv"):
        kernel = tf.get_variable(
            "weights", 
            kernel + [shape[-1], output_size], 
            initializer = initializer1
        )
        conv = tf.nn.conv2d(x, kernel, [1, 1, 1, 1], padding = "SAME")
        biases = tf.get_variable("biases", [output_size], initializer = initializer2)
        return conv + biases

def dropout(x, rate):
    return tf.nn.dropout(x, rate = rate)

def linear(input_, output_size, scope = None) : 
    shape = input_.get_shape().as_list()

    with tf.variable_scope(scope or "Linear"):
        W = tf.get_variable("weights", [shape[1], output_size], tf.float32,
                tf.random_normal_initializer(stddev=1e-4))
        bias = tf.get_variable("bias", [output_size],
                initializer=tf.constant_initializer(0.1))
        return tf.matmul(input_, W) + bias
