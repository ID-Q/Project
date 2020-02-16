# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://docs.scrapy.org/en/latest/topics/item-pipeline.html

import codecs
import json

class JingdongPipeline(object):
    def __init__(self):
        self.file = codecs.open('output.json', "wb", encoding="utf-8")

    def process_item(self, item, spider):
        name = item["name"]
        price = item["price"]
        owner = item["owner"]
        comment = item["comment"]
        link = item["link"]
        goods = {"name": name, "price": price, "owner": owner, "comment": comment, "link": link}
        i = json.dumps(dict(goods), ensure_ascii=False)
        line = i + '\n'
        self.file.write(line)
        return item

    def close_spider(self, spider):
        self.file.close()
