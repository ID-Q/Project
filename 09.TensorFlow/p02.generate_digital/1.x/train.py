#!/usr/bin/env python
# coding=utf-8
import os
import tensorflow as tf
import numpy as np
from tensorflow.python import pywrap_tensorflow
import init_data as input_data
from PIL import Image
from model import *
import matplotlib.pyplot as plt

tf.app.flags.DEFINE_integer("batch_size", 128, "batch_size for trainning")
tf.app.flags.DEFINE_integer("max_steps", 100000, "steps for trainning.")

FLAGS = tf.app.flags.FLAGS

def main(_):
    sess = tf.Session()

    # prepare data
    data = input_data.read_data_sets("data/", one_hot = True)
    
    # define input
    noise_batch, image_batch = get_input()
    
    # define model
    gen_imgs = generator(noise_batch)
    truth_score = discriminator(image_batch)
    fake_score = discriminator(gen_imgs, reuse = True)
    
    # define loss
    g_loss = generator_loss(fake_score)
    d_loss = discriminator_loss(truth_score, fake_score)

    #define train op
    t_vars = tf.trainable_variables()

    g_var_list = [var for var in t_vars if "g_var" in var.name]
    d_var_list = [var for var in t_vars if "d_var" in var.name]
    
    g_train_op, g_step = generator_train(g_loss, g_var_list)
    d_train_op, d_step = discriminator_train(d_loss, d_var_list)
  
    # init all variables
    init = tf.global_variables_initializer()
    sess.run(init)
    
    tf.summary.FileWriter("./log", sess.graph)

    for i in range(FLAGS.max_steps):
        xs, ys = data.train.next_batch(FLAGS.batch_size)
        xs = xs * 2.0 - 1.0; 
        dlv, _, d_steps = sess.run(
            [d_loss, d_train_op, d_step], 
            feed_dict = {image_batch : xs})

        glv, _, g_steps = sess.run([g_loss, g_train_op, g_step])

        steps_per_save = 200
        if d_steps % steps_per_save == 0:
            print("d loss val : ", dlv, " d steps : ", d_steps)
            print("g loss val : ", glv, " d steps : ", g_steps)
            print("======================")

            imgs = sess.run(gen_imgs)
            for i in range(100):
                img = ((imgs[i] + 1.0) / 2.0) * 255
                img = img.reshape(28,28)
                plt.subplot(10,10,i+1)
                plt.imshow(img,cmap='gray')
                plt.axis('off')
            plt.savefig(os.path.join("./result", '%d.png' % (d_steps / steps_per_save)))
            
if __name__ == "__main__":
    tf.app.run()
