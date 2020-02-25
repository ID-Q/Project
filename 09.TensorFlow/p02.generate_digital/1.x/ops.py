#!/usr/bin/env python
# coding=utf-8
import tensorflow as tf
import numpy as np

def sigmoid(x):
    return tf.sigmoid(x)

def tanh(x):
    return tf.nn.tanh(x)

def lrelu(x, leak = 0.2):
    return tf.nn.leaky_relu(x, leak)

def linear(input_, output_size, scope = None, stddev = 0.02, bias_start = 0.1):
    shape = input_.get_shape().as_list()
    
    with tf.variable_scope(scope or "Linear"):
        W = tf.get_variable("weights", [shape[1], output_size], tf.float32,
                tf.random_normal_initializer(stddev=stddev))
        bias = tf.get_variable("bias", [output_size],
                initializer=tf.constant_initializer(bias_start))
        return tf.matmul(input_, W) + bias
