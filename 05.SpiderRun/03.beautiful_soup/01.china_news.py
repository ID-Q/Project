#!/usr/bin/env python
# coding=utf-8
import re
import sys
import json
import urllib.request
from bs4 import BeautifulSoup

url = 'http://www.chinanews.com/rss/rss_2.html'

# 得到链接
def get_links(url):
    html = urllib.request.urlopen(url)
    bs = BeautifulSoup(html.read(), 'html.parser')
    links = set([item['href'] for item in bs.find_all('a')])
    return links

def get_texts(url):
    html = urllib.request.urlopen(url)
    bs = BeautifulSoup(html.read(), "html.parser")
    bs = bs.find_all('div')
    print (bs)

# 得到信息
def get_rss(url):
    html = urllib.request.urlopen(url)
    bs = BeautifulSoup(html.read(), "lxml")
    items = []
    for item in bs.find_all('item'):
        try:
            feed_item = {
                'title' : item.title.text,
                'description' : item.description.text,
                'link' : item.contents[2],
                'pub_date' : item.pubdate.text,
            }
            items.append(feed_item)
        except Exception as e:
            print (e.message)
    return items

if __name__ == "__main__":
    links = get_links(url)
    feed_items = []
    for link in links:
        feed_items += get_rss(link)
    with open('./news.json', 'w', encoding = 'utf-8') as f:
        for key in feed_items:
            json.dump(key, f, ensure_ascii=False)
   
