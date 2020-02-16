# -*- coding: utf-8 -*-
import scrapy
from Tencent.items import TencentItem

class TencentSpider(scrapy.Spider):
    name = 'tencent'
    allowed_domains = ['tencent.com']
    base_url = 'https://careers.tencent.com/search.html?&start='
    offset = 0
    start_urls = [base_url + str(offset)]
    
    def parse(self, response):
        item = TencentItem()
        node_list = response.xpath("//div[@class='recruit-list']") 
        print(node_list)
        for node in node_list:
            item['positionName'] = node.xpath(".//div[@class='recruit-list']/a/h4/text()").extract()[0].encode("utf-8")
            item['businessGroup'] = node.xpath(".//div/a//p[@class='recruit-tips']/span[1]").extract()[0].encode("utf-8")
            item['workLocation'] = node.xpath(".//div/a//p[@class='recruit-tips']/span[2]").extract()[0].encode("utf-8")
            item['positionType'] = node.xpath(".//div/a//p[@class='recruit-tips']/span[3]").extract()[0].encode("utf-8")
            item['publishTime'] = node.xpath(".//div/a//p[@class='recruit-tips']/span[4]").extract()[0].encode("utf-8")
            item['positionDetail'] = node.xpath(".//div/a//p[2][@class='recruit-text']").extract()[0].encode("utf-8")

            yield item

        if self.offset < 100:
            self.offset += 10
            url = self.base_url + str(self.offset)
            yield scrapy.Request(url, callback = self.parse)
