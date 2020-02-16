# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html
import codecs
import json

class DangdangPipeline(object):
    def __init__(self):
        self.file = codecs.open('output.json', "wb", encoding="utf-8")

    def process_item(self, item, spider):
        for j in range(0, len(item["name"])):
            name = item["name"][j]
            price = item["price"][j]
            comnum = item["comnum"][j]
            link = item["link"][j]
            # 将当前页下第j个商品的name、price、comnum、link等信息处理一下，重新组合成一个字典
            goods = {"name": name, "price": price, "comnum": comnum, "link": link}
            # 将组合后的当前页中第j个商品的数据写入json文件
            i = json.dumps(dict(goods), ensure_ascii = False)
            line = i + '\n'
            self.file.write(line)
            # 返回item
        return item

    def close_spider(self, spider):
        self.file.close()
