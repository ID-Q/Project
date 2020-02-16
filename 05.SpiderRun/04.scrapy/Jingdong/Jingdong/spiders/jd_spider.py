# -*- coding: utf-8 -*-
import scrapy
import re 
from scrapy.http import Request
from scrapy_splash import SplashRequest
from Jingdong.items import JingdongItem

class JdSpiderSpider(scrapy.Spider):
    name = 'jd_spider'
    allowed_domains = ['jd.com']
    start_urls = (
        'https://list.jd.com/list.html?cat=652,654,831',
    )

    def start_requests(self):
        for url in self.start_urls:
            yield SplashRequest(url = url, callback = self.parse, args={'wait': 1}, endpoint='render.html')

    def parse(self, response):
        for sel in response.xpath('//*[@id="plist"]/ul/li/div[@class="gl-i-wrap j-sku-item"]'):
            item = JingdongItem()
            item["link"] = "http:" + str(sel.xpath('div[1]/a/@href').extract())[2:-2]
            item["price"] = sel.xpath('div[2]/strong[1]/i/text()').extract()
            temp = str(sel.xpath('div[3]/a/em/text()').extract())
            pattern = re.compile("[\u4e00-\u9fa5]+.+\w")  # 从第一个汉字起 匹配商品名称
            good_name = re.search(pattern, temp)
            item["name"] = good_name.group()
            item["comment"] = sel.xpath('div[4]/strong/a/text()').extract()
            item["owner"] = sel.xpath('div[5]/span/a/text()').extract()
            yield item
