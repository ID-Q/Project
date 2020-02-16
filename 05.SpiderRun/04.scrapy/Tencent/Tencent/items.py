# -*- coding: utf-8 -*-

# Define here the models for your scraped items
#
# See documentation in:
# https://docs.scrapy.org/en/latest/topics/items.html

import scrapy


class TencentItem(scrapy.Item):
    # define the fields for your item here like:
    # name = scrapy.Field()
    positionName = scrapy.Field() #职位名称
    businessGroup = scrapy.Field() #事业群
    workLocation = scrapy.Field() #工作地点
    positionType = scrapy.Field() #职位类别
    publishTime = scrapy.Field() #发布时间
    positionDetail = scrapy.Field() #职位明细
