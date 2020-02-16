#!/usr/bin/env python
# coding=utf-8
import numpy as np
import os
import cv2
import struct

#get the image and lable set
def load_mnist(path, kind='train'):
    """Load MNIST data from `path`"""
    labels_path = os.path.join(path, '%s-labels-idx1-ubyte' % kind)
    images_path = os.path.join(path, '%s-images-idx3-ubyte' % kind)
    with open(labels_path, 'rb') as lbpath:
        magic, n = struct.unpack('>II', lbpath.read(8))
        labels = np.fromfile(lbpath, dtype=np.uint8)
    with open(images_path, 'rb') as imgpath:
        magic, num, rows, cols = struct.unpack(">IIII", imgpath.read(16))
        images = np.fromfile(imgpath, dtype=np.uint8).reshape(len(labels), 784)
    return images, labels

#print the train image number and shape
X_train, y_train = load_mnist('./dataset/', kind='train')
print('Rows: %d, columns: %d' % (X_train.shape[0], X_train.shape[1]))

#print the test image number and shape
X_test, y_test = load_mnist('', kind='t10k')
print('Rows: %d, columns: %d' % (X_test.shape[0], X_test.shape[1]))

#save the train image and lable sperately
count = np.zeros(10)
nTrain = len(X_train)
train_labels =[]
fo1 = open('./train_lable/train_lable.txt',"w+")
for i in xrange(nTrain):
    label = y_train[i]
    train_labels.append(str(label)+'\n')
    count[label] += 1
    #filename = './train/' + str(label) + '/' + str(label) + '_' + str(int(count[label])) + '.png'
    filename = 'train_image/' + str(i) + '_' + str(label) + '.png'
    img = X_train[i].reshape(28,28)
    cv2.imwrite(filename, img)
for line in train_labels:
    fo1.write(line)
fo1.close

#save the train image and lable sperately
count = np.zeros(10)
nTest = len(X_test)
test_labels = []
fo2 = open('test_lable/test_lable.txt',"w+")
for i in xrange(nTest):
    label = y_test[i]
    test_labels.append(str(label) + '\n')
    count[label] += 1
    filename = 'test_image/' + str(i) + '.png'
    img = X_test[i].reshape(28,28)
    cv2.imwrite(filename, img)
for line in test_labels:
    fo2.write(str(line))
fo2.close
