# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# https://docs.scrapy.org/en/latest/topics/items.html

import scrapy


class JingdongItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    name = scrapy.Field() #名称
    link = scrapy.Field() #链接
    price = scrapy.Field() #价格
    owner = scrapy.Field() #销售店铺
    comment = scrapy.Field() #评论数
