#!/usr/bin/env python
# coding=utf-8
import tensorflow as tf
import init_data as input_data
from ops import *
from functools import reduce

tf.app.flags.DEFINE_integer("max_steps", "10000", "iterator numbers")
tf.app.flags.DEFINE_integer("batch_size", "128", "batch size")

FLAGS = tf.app.flags.FLAGS

def get_input():
    with tf.variable_scope("input") as scope:
        x = tf.placeholder(tf.float32, shape = [None, 784], name = "x")
        y = tf.placeholder(tf.float32, shape = [None, 10], name = "y")
        rate = tf.placeholder(tf.float32, shape = None, name = "drop_rate")
    return x, y, rate

def inference(image, rate):
    image = tf.reshape(image, shape = [-1, 28, 28, 1])

    conv1 = relu(conv2d(image, [5, 5], 32, "conv1"))
    pool1 = pooling(conv1, "pool1")
    
    conv2 = relu(conv2d(pool1, [5, 5], 64, "conv2"))
    pool2 = pooling(conv2, "pool2")
    
    pool2_shape = pool2.get_shape().as_list()
    dim = reduce(lambda a, b: a * b, pool2_shape[1:])

    input = tf.reshape(pool2, shape = [-1, dim])
    
    fc1 = dropout(linear(input, 1024, "local1"), rate)
    output = linear(fc1, 10, "output")
    return output


def get_loss(y, y_):
    return tf.reduce_mean(
        tf.nn.softmax_cross_entropy_with_logits(
            logits = y_, labels = y
        )
    )

def get_train(loss):
    return tf.train.AdamOptimizer(1e-4).minimize(loss)

def get_accuracy(y, y_):
    label1 = tf.cast(tf.argmax(y, 1), tf.int32)
    label2 = tf.cast(tf.argmax(y_, 1), tf.int32)
    return tf.reduce_mean(tf.cast(tf.equal(label1, label2), tf.float32))

def main(_):
    sess = tf.Session()

    x, y, rate = get_input()
    logits = inference(x, rate)
    loss = get_loss(y, logits)
    train_op = get_train(loss)
    accuracy = get_accuracy(logits, y)
    
    data = input_data.read_data_sets("hug_data/", one_hot = True)
    
    init = tf.global_variables_initializer()
    sess.run(init)

    for i in range(0, FLAGS.max_steps):
        x_data, y_data = data.train.next_batch(FLAGS.batch_size)
        sess.run(train_op, feed_dict = {x : x_data, y : y_data, rate : 0.5})
        if i % 20 == 1:
            acc, loss_val = sess.run(
                [accuracy, loss], 
                feed_dict = {x : data.test.images, y : data.test.labels, rate : 0}
            )
            print("loss : %lf, accuracy : %lf" % (loss_val, acc))

if __name__ == "__main__" :
    tf.app.run()
