#!/usr/bin/env python
# coding=utf-8
import re
import urllib.request

def get_novel_content(n):
    # 获取页面源代码 
    url = 'http://www.quanshuwang.com/book/44/44683'
    html = urllib.request.urlopen(url).read()
    # 指定编码
    html = html.decode('gbk')
    # 获取章节源代码
    # 正则表达式
    reg = r'<li><a href="(.*?)" title=".*?">(.*?)</a></li>'
    reg = re.compile(reg)
    urls = re.findall(reg, html)
    for url in urls:
        n = n - 1
        if (n == 0):
            exit()
        novel_url,novel_title = url
        # 获取章节页面源代码
        chapter = urllib.request.urlopen(novel_url).read()
        chapter_html = chapter.decode('gbk')
        chapter_reg = r'</script>&nbsp;&nbsp;&nbsp;&nbsp;(.*?)<script type="text/javascript">'
        # 多行匹配
        chapter_reg = re.compile(chapter_reg, re.S)
        chapter_content = re.findall(chapter_reg, chapter_html)
        # 数据清洗
        chapter_content = chapter_content[0].replace('&nbsp;','')
        chapter_content = chapter_content.replace('<br />','')
        # 下载到本地
        print('正在下载 %s' % novel_title)
        
        with open('{}.txt'.format(novel_title), 'w') as f:
            f.write(chapter_content)


if __name__ == "__main__":
    get_novel_content(12)
