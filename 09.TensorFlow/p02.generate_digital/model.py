#!/usr/bin/env python
# coding=utf-8
import tensorflow as tf
import numpy as np
from ops import *

FLAGS = tf.app.flags.FLAGS

def get_input():
    noise_batch = tf.random_normal(dtype = tf.float32, shape = [FLAGS.batch_size, 100])
    image_batch = tf.placeholder(tf.float32, shape = [FLAGS.batch_size, 784])
    return noise_batch, image_batch

def generator(input_, reuse = False):
    layer = input_
    with tf.variable_scope("g_var", reuse = reuse) as scope:
        with tf.variable_scope("local1") as scope:
            layer = lrelu(linear(layer, 512, scope))
        with tf.variable_scope("output_img") as scope:
            layer = tanh(linear(layer, 784, scope))
    return layer

def discriminator(input_, reuse = False):
    layer = input_
    with tf.variable_scope("d_var", reuse = reuse) as scope:
        with tf.variable_scope("local1") as scope:
            layer = lrelu(linear(layer, 512))
        with tf.variable_scope("output") as scope:
            layer = linear(layer, 1)
    return layer

def generator_loss(fake_score):
    with tf.name_scope("G_loss"):
        G_loss = tf.reduce_mean(
            tf.nn.sigmoid_cross_entropy_with_logits(
                logits=fake_score,
                labels=tf.ones_like(fake_score)*0.9
            )
        )
        return G_loss

def discriminator_loss(truth_score, fake_score):
    with tf.name_scope("D_loss"):
        fake_loss = tf.reduce_mean(
            tf.nn.sigmoid_cross_entropy_with_logits(
                logits=fake_score,
                labels=tf.zeros_like(fake_score)
            )
        )
        real_loss = tf.reduce_mean(
            tf.nn.sigmoid_cross_entropy_with_logits(
                logits=truth_score,
                labels=tf.ones_like(truth_score)*0.9
            )
        )
        return tf.add(real_loss, fake_loss)

def __train(loss, var_list, scope_name):
    with tf.variable_scope(scope_name) as scope:
        global_step = tf.Variable(0, trainable = False)
        lr = tf.train.exponential_decay(learning_rate = 1e-2,
                                    global_step = global_step,
                                    decay_rate = 0.9,
                                    decay_steps = 470)
        optimizer = tf.train.AdamOptimizer(lr)
        train_op = optimizer.minimize(loss, global_step, var_list = var_list)
    return train_op, global_step

def generator_train(loss, var_list):
    return __train(loss, var_list, "G_train_op")

def discriminator_train(loss, var_list):
    return __train(loss, var_list, "D_train_op")

if __name__ == "__main__":
    x = tf.placeholder(tf.float32, shape = [None, 100])
    generator(x)
    discriminator(x)
    discriminator(x, reuse = True)
    t_vars = tf.trainable_variables()
    
    print("build model done :")
    for var in t_vars:
        print(var.name, var.shape)
