#!/usr/bin/env python
# coding=utf-8
import oss2
import uuid
import pyperclip
import sys
import time
import os
from PIL import Image
from PIL import ImageGrab

#OSS信息
AccessKeyID = "LTAIdJCXLuaVGlv5"
AccessKeySecret = "UIwifTY72U2DSkWZ0WCcZb9r7J01Tx"
EndPoint = "oss-cn-beijing.aliyuncs.com"
BucketName = "zhengdongqi"
#存储路径
Path = "/Users/zhengdongqi/Desktop/" 

def is_img(ext):
    ext = ext.lower()
    if ext in ['.jpg', '.png', '.jpeg', '.bmp', '.gif']:
        return True
    else: 
        return False

def has_im(): 
    if len(sys.argv) == 1:
        im = ImageGrab.grabclipboard() 
        if isinstance(im, Image.Image):
            src_file = Path + "Oss.jpg"
            im.save(src_file)
            return src_file
        else: 
            print "剪贴板中无图片"
            sys.exit()
    elif len(sys.argv) == 2:
        if not os.path.exists(sys.argv[1]):
            print "文件不存在"
            sys.exit()
        else:
            if not is_img(os.path.splitext(sys.argv[1])[1]):
                print "%s" % os.path.splitext(sys.argv[1])[1]
                sys.exit()
            else: 
                src_file = sys.argv[1]
    else:
        print "Error in args"
        sys.exit()

def oss_file_name(local_name):
    name = uuid.uuid4().__str__().replace("-", "").upper()
    Date = time.strftime('%Y-%m-%d', time.localtime(time.time()))
    local_name = str(local_name).rsplit(".")
    return "%s/%s.%s" % (Date, name, local_name[-1])

def Oss(img_name, img_path):
    auth = oss2.Auth(AccessKeyID, AccessKeySecret)
    bucket = oss2.Bucket(auth, EndPoint, BucketName)
    bucket.put_object_from_file(img_name, img_path)

def Url_Clip(name):
    result_str = "![](http://%s.%s/%s)" % (BucketName, EndPoint, name)
    pyperclip.copy(result_str)

if __name__=="__main__":
    img_path = has_im()
    img_name = oss_file_name(img_path)
    Oss(img_name, img_path)
    Url_Clip(img_name) 
