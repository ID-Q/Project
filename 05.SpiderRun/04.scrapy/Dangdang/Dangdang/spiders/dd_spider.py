# -*- coding: utf-8 -*-
import scrapy
from scrapy.http import Request
from Dangdang.items import DangdangItem


class DdSpiderSpider(scrapy.Spider):
    name = 'dd_spider'
    allowed_domains = ['dangdang.com']
    start_urls = [
        'http://category.dangdang.com/pg1-cid4004279.html',
    ]

    def parse(self, response):
        item = DangdangItem()
        item["name"] = response.xpath("//a[@class='pic']/@title").extract()
        item["price"] = response.xpath("//span[@class='price_n']/text()").extract()
        item["link"] = response.xpath("//a[@class='pic']/@href").extract()
        item["comnum"] = response.xpath("//a[@name='itemlist-review']/text()").extract()

        yield item
        # 通过循环自动爬取76页的数据
        for i in range(2, 77):
            # 通过上面总结的网址格式构造要爬取的网址
            url = "http://category.dangdang.com/pg" + str(i) + "-cid4004279.html"
            # 通过yield返回Request，并指定要爬取的网址和回调函数
            # 实现自动爬取
            yield scrapy.Request(url, callback = self.parse)
