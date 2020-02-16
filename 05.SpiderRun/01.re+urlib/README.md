# Python爬虫基于（re正则表达式和urlib）

## urllib2库的基本使用

所谓网页抓取，就是把URL地址中指定的网络资源从网络流中读取出来，保存到本地。 在Python中有很多库可以用来抓取网页，我们先学习urllib2。

urllib2 是 Python2.7 自带的模块(不需要下载，导入即可使用)

urllib2 官方文档：https://docs.python.org/2/library/urllib2.html

urllib2 源码：https://hg.python.org/cpython/file/2.7/Lib/urllib2.py

urllib2 在 python3.x 中被改为urllib.request

## urlopen

我们先来段代码：

```python
#!/usr/bin/env python
# coding=utf-8
# urllib2_urlopen.py

# 导入urllib2 库
import urllib2

# 向指定的url发送请求，并返回服务器响应的类文件对象
response = urllib2.urlopen("http://www.baidu.com")

# 类文件对象支持 文件对象的操作方法，如read()方法读取文件全部内容，返回字符串
html = response.read()

# 打印字符串
print html
执行写的python代码，将打印结果
```

实际上，如果我们在浏览器上打开百度主页， 右键选择“查看源代码”，你会发现，跟我们刚才打印出来的是一模一样。也就是说，上面的4行代码就已经帮我们把百度的首页的全部代码爬了下来。

一个基本的url请求对应的python代码真的非常简单。

## Request

在我们第一个例子里，urlopen()的参数就是一个url地址；

但是如果需要执行更复杂的操作，比如增加HTTP报头，必须创建一个 Request 实例来作为urlopen()的参数；而需要访问的url地址则作为 Request 实例的参数。

我们编辑urllib2_request.py

```python
#!/usr/bin/env python
# coding=utf-8
# urllib2_request.py

import urllib2

# url 作为Request()方法的参数，构造并返回一个Request对象
request = urllib2.Request("http://www.baidu.com")

# Request对象作为urlopen()方法的参数，发送给服务器并接收响应
response = urllib2.urlopen(request)

html = response.read()
print html
```


运行结果是完全一样的：

新建Request实例，除了必须要有 url 参数之外，还可以设置另外两个参数：

data（默认空）：是伴随 url 提交的数据（比如要post的数据），同时 HTTP 请求将从 "GET"方式 改为 "POST"方式。

headers（默认空）：是一个字典，包含了需要发送的HTTP报头的键值对。

这两个参数下面会说到。

## User-Agent

但是这样直接用urllib2给一个网站发送请求的话，确实略有些唐突了，就好比，人家每家都有门，你以一个路人的身份直接闯进去显然不是很礼貌。而且有一些站点不喜欢被程序（非人为访问）访问，有可能会拒绝你的访问请求。

但是如果我们用一个合法的身份去请求别人网站，显然人家就是欢迎的，所以我们就应该给我们的这个代码加上一个身份，就是所谓的User-Agent头。

浏览器 就是互联网世界上公认被允许的身份，如果我们希望我们的爬虫程序更像一个真实用户，那我们第一步，就是需要伪装成一个被公认的浏览器。用不同的浏览器在发送请求的时候，会有不同的User-Agent头。 urllib2默认的User-Agent头为：Python-urllib/x.y（x和y是Python主版本和次版本号,例如 Python-urllib/2.7）

```python
#!/usr/bin/env python
# coding=utf-8
# urllib2_useragent.py

import urllib2

url = "http://www.itcast.cn"

# IE 9.0 的 User-Agent，包含在 ua_header里
ua_header = {"User-Agent" : "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0;"} 

# url 连同 headers，一起构造Request请求，这个请求将附带 IE9.0 浏览器的User-Agent
request = urllib2.Request(url, headers = ua_header)

# 向服务器发送这个请求
response = urllib2.urlopen(request)

html = response.read()
print html
```

## 添加更多的Header信息

在 HTTP Request 中加入特定的 Header，来构造一个完整的HTTP请求消息。

可以通过调用Request.add_header() 添加/修改一个特定的header 也可以通过调用Request.get_header()来查看已有的header。

添加一个特定的header

```python
#!/usr/bin/env python
# coding=utf-8
# urllib2_headers.py

import urllib2

url = "http://www.itcast.cn"

# IE 9.0 的 User-Agent
header = {"User-Agent" : "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0;"} 
request = urllib2.Request(url, headers = header)

# 也可以通过调用Request.add_header() 添加/修改一个特定的header
request.add_header("Connection", "keep-alive")

# 也可以通过调用Request.get_header()来查看header信息
# request.get_header(header_name="Connection")
response = urllib2.urlopen(req)

print response.code     #可以查看响应状态码
html = response.read()

print html
```

## 随机添加/修改User-Agent

```python
#!/usr/bin/env python
# coding=utf-8
# urllib2_add_headers.py

import urllib2
import random

url = "http://www.itcast.cn"

ua_list = [
    "Mozilla/5.0 (Windows NT 6.1; ) Apple.... ",
    "Mozilla/5.0 (X11; CrOS i686 2268.111.0)... ",
    "Mozilla/5.0 (Macintosh; U; PPC Mac OS X.... ",
    "Mozilla/5.0 (Macintosh; Intel Mac OS... "
]

user_agent = random.choice(ua_list)
request = urllib2.Request(url)

# 也可以通过调用Request.add_header() 添加/修改一个特定的header
request.add_header("User-Agent", user_agent)

# 第一个字母大写，后面的全部小写
request.get_header("User-agent")
response = urllib2.urlopen(req)

html = response.read()

print html
```

# novel_spider爬小说

> 首先引入库

```python
#!/usr/bin/env python
# coding=utf-8
import re
import urllib2
```

> 爬取的网站是：[http://www.quanshuwang.com/book/44/44683](http://www.quanshuwang.com/book/44/44683)

```python
#!/usr/bin/env python
# coding=utf-8
import re
import urllib.request
 
def get_novel_content():
    # 获取页面源代码 
    url = 'http://www.quanshuwang.com/book/44/44683'
    html = urllib.request.urlopen(url).read()
```

最后一行我们调用了urllib2库的方法，urlopen()方法中我们传进一个网址作为参数表示我们需要爬取的网站，read()方法表示获取源代码。那我们现在打印html是否能成功在控制台把页面的代码给输出了呢？答案是否定的，现在获取的源码是一个乱码，我们还需要对该代码进行转码，于是要在下面加多一行转码的。

由上面我们可知代码已经转成了‘gbk’格式，并且也已经将它存在html这个变量上了，那我们怎么知道转成什么格式呢？通过获取网页源代码查看格式

![GBK](/Users/zhengdongqi/X.Nick撸代码/Note/Python/pic/GBK.png)

```python
#!/usr/bin/env python
# coding=utf-8
import re
import urllib.request
 
def get_novel_content():
    # 获取页面源代码 
    url = 'http://www.quanshuwang.com/book/44/44683'
    html = urllib.request.urlopen(url).read()
    # 指定编码
    html = html.decode('gbk')
    print (html)
    
if __name__ == "__main__":
    get_novel_content()
```

因为我们要获取整本小说，所以让我们先获取章节目录吧，把鼠标指向其中一章并选中，下面就自动定位到该章节标签位置了

![参考](/Users/zhengdongqi/X.Nick撸代码/Note/Python/pic/参考.png)

回到编辑器这边把刚才的代码粘贴过来并打上注释，作为一个参考的模板

```python
#!/usr/bin/env python
# coding=utf-8
import re
import urllib.request
 
def get_novel_content():
    # 获取页面源代码 
    url = 'http://www.quanshuwang.com/book/44/44683'
    html = urllib.request.urlopen(url).read()
    # 指定编码
    html = html.decode('gbk')
    #<li><a href="http://www.quanshuwang.com/book/44/44683/15379609.html" title="引子 穿越的唐家三少，共2744字">引子 穿越的唐家三少</a></li> #参考
    print (html)
    
if __name__ == "__main__":
    get_novel_content()
```

因为我们需要抓取的是全部章节而不仅仅只是这一个章节，所以我们要用到正则表达式来进行匹配，先把通用的部分用(.*?)替代，(.*?)可以匹配所有东西

```python
#!/usr/bin/env python
# coding=utf-8
import re
import urllib.request
 
def get_novel_content():
    # 获取页面源代码 
    url = 'http://www.quanshuwang.com/book/44/44683'
    html = urllib.request.urlopen(url).read()
    # 指定编码
    html = html.decode('gbk')
    #<li><a href="http://www.quanshuwang.com/book/44/44683/15379609.html" title="引子 穿越的唐家三少，共2744字">引子 穿越的唐家三少</a></li> #参考
    # 获取章节源代码
    # 正则表达式
    reg = r'<li><a href="(.*?)" title=".*?">(.*?)</a></li>'
    reg = re.compile(reg)
    urls = re.findall(reg, html)
    for url in urls:
        print(url)

if __name__ == "__main__":
    get_novel_content()
```

仔细的小伙伴就发现有些地方的.*?加括号，有些地方又不加，这是因为加了括号的都是我们要匹配的，不加括号是我们不需要匹配的。接下来一行调用re.compiled()方法是增加匹配的效率，建议习惯加上，最后一行开始与我们一开始获取的整个网页的源代码进行匹配。到这步我们已经能把代码所有章节以及章节链接的代码都获取了，打印在控制台上看一下

![目录](/Users/zhengdongqi/X.Nick撸代码/Note/Python/pic/目录.png)

接下来获取章节页面源代码

```python
#!/usr/bin/env python
# coding=utf-8
import re
import urllib.request
 
def get_novel_content():
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
        novel_url,novel_title = url
        # 获取章节页面源代码
        chapter = urllib.request.urlopen(novel_url).read()
        chapter_html = chapter.decode('gbk')

        print(chapter_html)
        exit()

if __name__ == "__main__":
    get_novel_content()
```

![章节页面代码](/Users/zhengdongqi/X.Nick撸代码/Note/Python/pic/章节页面代码.png)

接下来继续用正则表达式进行数据清洗

```python
#!/usr/bin/env python
# coding=utf-8
import re
import urllib.request
 
def get_novel_content():
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
        novel_url,novel_title = url
        # 获取章节页面源代码
        chapter = urllib.request.urlopen(novel_url).read()
        chapter_html = chapter.decode('gbk')
        chapter_reg = r'</script>&nbsp;&nbsp;&nbsp;&nbsp;(.*?)<script type="text/javascript">'
        # 多行匹配
        chapter_reg = re.compile(chapter_reg, re.S)
        chapter_content = re.findall(chapter_reg, chapter_html)
        # 数据清洗
        chapter_content = chapter_content[0].replace('&nbsp;', '')
        chapter_content = chapter_content.replace('<br />', '')
        print(chapter_content)
        
if __name__ == "__main__":
    get_novel_content()
```

![完整页面](/Users/zhengdongqi/X.Nick撸代码/Note/Python/pic/完整页面.png)

最后一步就是下载了

![下载](/Users/zhengdongqi/X.Nick撸代码/Note/Python/pic/下载.png)

附上完整代码

```python
#!/usr/bin/env python
# coding=utf-8
import re
import urllib.request
 
def get_novel_content():
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
        exit()


if __name__ == "__main__":
    get_novel_content()
```



正则表达式

```python
一个正则表达式（或RE）指定了一集与之匹配的字符串；模块内的函数可以让你检查某个字符串是否跟给定的正则表达式匹配（或者一个正则表达式是否匹配到一个字符串，这两种说法含义相同）。

正则表达式可以拼接； 如果 A 和 B 都是正则表达式， 那么 AB 也是正则表达式。 通常， 如果字符串 p 匹配 A 并且另一个字符串 q 匹配 B, 那么 pq 可以匹配 AB。除非 A 或者 B 包含低优先级操作，A 和 B 存在边界条件；或者命名组引用。所以，复杂表达式可以很容易的从这里描述的简单源语表达式构建。 了解更多正则表达式理论和实现，参考the Friedl book [Frie09] ，或者其他编译器构建的书籍。

以下是正则表达式格式的简要说明。更详细的信息和演示，参考 正则表达式HOWTO。

正则表达式可以包含普通或者特殊字符。绝大部分普通字符，比如 'A', 'a', 或者 '0'，都是最简单的正则表达式。它们就匹配自身。你可以拼接普通字符，所以 last 匹配字符串 'last'. （在这一节的其他部分，我们将用 this special style 这种方式表示正则表达式，通常不带引号，要匹配的字符串用 'in single quotes' ，单引号形式。）

有些字符，比如 '|' 或者 '('，属于特殊字符。 特殊字符既可以表示它的普通含义， 也可以影响它旁边的正则表达式的解释。

重复修饰符 (*, +, ?, {m,n}, 等) 不能直接嵌套。这样避免了非贪婪后缀 ? 修饰符，和其他实现中的修饰符产生的多义性。要应用一个内层重复嵌套，可以使用括号。 比如，表达式 (?:a{6})* 匹配6个 'a' 字符重复任意次数。

特殊字符是：

.
(点) 在默认模式，匹配除了换行的任意字符。如果指定了标签 DOTALL ，它将匹配包括换行符的任意字符。

^
(插入符号) 匹配字符串的开头， 并且在 MULTILINE 模式也匹配换行后的首个符号。

$
匹配字符串尾或者换行符的前一个字符，在 MULTILINE 模式匹配换行符的前一个字符。 foo 匹配 'foo' 和 'foobar' , 但正则 foo$ 只匹配 'foo'。更有趣的是， 在 'foo1\nfoo2\n' 搜索 foo.$ ，通常匹配 'foo2' ，但在 MULTILINE 模式 ，可以匹配到 'foo1' ；在 'foo\n' 搜索 $ 会找到两个空串：一个在换行前，一个在字符串最后。

*
对它前面的正则式匹配0到任意次重复， 尽量多的匹配字符串。 ab* 会匹配 'a'， 'ab'， 或者 'a'``后面跟随任意个 ``'b'。

+
对它前面的正则式匹配1到任意次重复。 ab+ 会匹配 'a' 后面跟随1个以上到任意个 'b'，它不会匹配 'a'。

?
对它前面的正则式匹配0到1次重复。 ab? 会匹配 'a' 或者 'ab'。

*?, +?, ??
'*', '+'，和 '?' 修饰符都是 贪婪的；它们在字符串进行尽可能多的匹配。有时候并不需要这种行为。如果正则式 <.*> 希望找到 '<a> b <c>'，它将会匹配整个字符串，而不仅是 '<a>'。在修饰符之后添加 ? 将使样式以 非贪婪`方式或者 :dfn:`最小 方式进行匹配； 尽量 少 的字符将会被匹配。 使用正则式 <.*?> 将会仅仅匹配 '<a>'。

{m}
对其之前的正则式指定匹配 m 个重复；少于 m 的话就会导致匹配失败。比如， a{6} 将匹配6个 'a' , 但是不能是5个。

{m,n}
对正则式进行 m 到 n 次匹配，在 m 和 n 之间取尽量多。 比如，a{3,5} 将匹配 3 到 5个 'a'。忽略 m 意为指定下界为0，忽略 n 指定上界为无限次。 比如 a{4,}b 将匹配 'aaaab' 或者1000个 'a' 尾随一个 'b'，但不能匹配 'aaab'。逗号不能省略，否则无法辨别修饰符应该忽略哪个边界。

{m,n}?
前一个修饰符的非贪婪模式，只匹配尽量少的字符次数。比如，对于 'aaaaaa'， a{3,5} 匹配 5个 'a' ，而 a{3,5}? 只匹配3个 'a'。

\
转义特殊字符（允许你匹配 '*', '?', 或者此类其他），或者表示一个特殊序列；特殊序列之后进行讨论。

如果你没有使用原始字符串（ r'raw' ）来表达样式，要牢记Python也使用反斜杠作为转义序列；如果转义序列不被Python的分析器识别，反斜杠和字符才能出现在字符串中。如果Python可以识别这个序列，那么反斜杠就应该重复两次。这将导致理解障碍，所以高度推荐，就算是最简单的表达式，也要使用原始字符串。

[]
用于表示一个字符集合。在一个集合中：

字符可以单独列出，比如 [amk] 匹配 'a'， 'm'， 或者 'k'。
可以表示字符范围，通过用 '-' 将两个字符连起来。比如 [a-z] 将匹配任何小写ASCII字符， [0-5][0-9] 将匹配从 00 到 59 的两位数字， [0-9A-Fa-f] 将匹配任何十六进制数位。 如果 - 进行了转义 （比如 [a\-z]）或者它的位置在首位或者末尾（如 [-a] 或 [a-]），它就只表示普通字符 '-'。
特殊字符在集合中，失去它的特殊含义。比如 [(+*)] 只会匹配这几个文法字符 '(', '+', '*', or ')'。
字符类如 \w 或者 \S (如下定义) 在集合内可以接受，它们可以匹配的字符由 ASCII 或者 LOCALE 模式决定。
不在集合范围内的字符可以通过 取反 来进行匹配。如果集合首字符是 '^' ，所有 不 在集合内的字符将会被匹配，比如 [^5] 将匹配所有字符，除了 '5'， [^^] 将匹配所有字符，除了 '^'. ^ 如果不在集合首位，就没有特殊含义。
在集合内要匹配一个字符 ']'，有两种方法，要么就在它之前加上反斜杠，要么就把它放到集合首位。比如， [()[\]{}] 和 []()[{}] 都可以匹配括号。
Unicode Technical Standard #18 里的嵌套集合和集合操作支持可能在未来添加。这将会改变语法，所以为了帮助这个改变，一个 FutureWarning 将会在有多义的情况里被 raise，包含以下几种情况，集合由 '[' 开始，或者包含下列字符序列 '--', '&&', '~~', 和 '||'。为了避免警告，需要将它们用反斜杠转义。
在 3.7 版更改: 如果一个字符串构建的语义在未来会改变的话，一个 FutureWarning 会 raise 。

|
A|B， A 和 B 可以是任意正则表达式，创建一个正则表达式，匹配 A 或者 B. 任意个正则表达式可以用 '|' 连接。它也可以在组合（见下列）内使用。扫描目标字符串时， '|' 分隔开的正则样式从左到右进行匹配。当一个样式完全匹配时，这个分支就被接受。意思就是，一旦 A 匹配成功， B 就不再进行匹配，即便它能产生一个更好的匹配。或者说，'|' 操作符绝不贪婪。 如果要匹配 '|' 字符，使用 \|， 或者把它包含在字符集里，比如 [|].

(...)
（组合），匹配括号内的任意正则表达式，并标识出组合的开始和结尾。匹配完成后，组合的内容可以被获取，并可以在之后用 \number 转义序列进行再次匹配，之后进行详细说明。要匹配字符 '(' 或者 ')', 用 \( 或 \), 或者把它们包含在字符集合里: [(], [)].

(?…)
这是个扩展标记法 （一个 '?' 跟随 '(' 并无含义）。 '?' 后面的第一个字符决定了这个构建采用什么样的语法。这种扩展通常并不创建新的组合； (?P<name>...) 是唯一的例外。 以下是目前支持的扩展。

(?aiLmsux)
( 'a', 'i', 'L', 'm', 's', 'u', 'x' 中的一个或多个) 这个组合匹配一个空字符串；这些字符对正则表达式设置以下标记 re.A (只匹配ASCII字符), re.I (忽略大小写), re.L (语言依赖), re.M (多行模式), re.S (点dot匹配全部字符), re.U (Unicode匹配), and re.X (冗长模式)。 (这些标记在 模块内容 中描述) 如果你想将这些标记包含在正则表达式中，这个方法就很有用，免去了在 re.compile() 中传递 flag 参数。标记应该在表达式字符串首位表示。

(?:…)
正则括号的非捕获版本。 匹配在括号内的任何正则表达式，但该分组所匹配的子字符串 不能 在执行匹配后被获取或是之后在模式中被引用。

(?aiLmsux-imsx:…)
('a', 'i', 'L', 'm', 's', 'u', 'x' 中的0或者多个， 之后可选跟随 '-' 在后面跟随 'i' , 'm' , 's' , 'x' 中的一到多个 .) 这些字符为表达式的其中一部分 设置 或者 去除 相应标记 re.A (只匹配ASCII), re.I (忽略大小写), re.L (语言依赖), re.M (多行), re.S (点匹配所有字符), re.U (Unicode匹配), and re.X (冗长模式)。(标记描述在 模块内容 .)

'a', 'L' and 'u' 作为内联标记是相互排斥的， 所以它们不能结合在一起，或者跟随 '-' 。 当他们中的某个出现在内联组中，它就覆盖了括号组内的匹配模式。在Unicode样式中， (?a:...) 切换为 只匹配ASCII， (?u:...) 切换为Unicode匹配 (默认). 在byte样式中 (?L:...) 切换为语言依赖模式， (?a:...) 切换为 只匹配ASCII (默认)。这种方式只覆盖组合内匹配，括号外的匹配模式不受影响。

3.6 新版功能.

在 3.7 版更改: 符号 'a', 'L' 和 'u' 同样可以用在一个组合内。

(?P<name>…)
（命名组合）类似正则组合，但是匹配到的子串组在外部是通过定义的 name 来获取的。组合名必须是有效的Python标识符，并且每个组合名只能用一个正则表达式定义，只能定义一次。一个符号组合同样是一个数字组合，就像这个组合没有被命名一样。

命名组合可以在三种上下文中引用。如果样式是 (?P<quote>['"]).*?(?P=quote) （也就是说，匹配单引号或者双引号括起来的字符串)：

引用组合 "quote" 的上下文
引用方法
在正则式自身内
(?P=quote) (如示)
\1
处理匹配对象 m
m.group('quote')
m.end('quote') (等)
传递到 re.sub() 里的 repl 参数中
\g<quote>
\g<1>
\1
(?P=name)
反向引用一个命名组合；它匹配前面那个叫 name 的命名组中匹配到的串同样的字串。

(?#…)
注释；里面的内容会被忽略。

(?=…)
匹配 … 的内容，但是并不消费样式的内容。这个叫做 lookahead assertion。比如， Isaac (?=Asimov) 匹配 'Isaac ' 只有在后面是 'Asimov' 的时候。

(?!…)
匹配 … 不符合的情况。这个叫 negative lookahead assertion （前视取反）。比如说， Isaac (?!Asimov) 只有后面 不 是 'Asimov' 的时候才匹配 'Isaac ' 。

(?<=…)
匹配字符串的当前位置，它的前面匹配 … 的内容到当前位置。这叫:dfn:positive lookbehind assertion （正向后视断定）。 (?<=abc)def 会在 'abcdef' 中找到一个匹配，因为后视会往后看3个字符并检查是否包含匹配的样式。包含的匹配样式必须是定长的，意思就是 abc 或 a|b 是允许的，但是 a* 和 a{3,4} 不可以。注意以 positive lookbehind assertions 开始的样式，如 (?<=abc)def ，并不是从 a 开始搜索，而是从 d 往回看的。你可能更加愿意使用 search() 函数，而不是 match() 函数：

>>>
>>> import re
>>> m = re.search('(?<=abc)def', 'abcdef')
>>> m.group(0)
'def'
这个例子搜索一个跟随在连字符后的单词：

>>>
>>> m = re.search(r'(?<=-)\w+', 'spam-egg')
>>> m.group(0)
'egg'
在 3.5 版更改: 添加定长组合引用的支持。

(?<!…)
匹配当前位置之前不是 ... 的样式。这个叫 negative lookbehind assertion （后视断定取非）。类似正向后视断定，包含的样式匹配必须是定长的。由 negative lookbehind assertion 开始的样式可以从字符串搜索开始的位置进行匹配。

(?(id/name)yes-pattern|no-pattern)
如果给定的 id 或 name 存在，将会尝试匹配 yes-pattern ，否则就尝试匹配 no-pattern，no-pattern 可选，也可以被忽略。比如， (<)?(\w+@\w+(?:\.\w+)+)(?(1)>|$) 是一个email样式匹配，将匹配 '<user@host.com>' 或 'user@host.com' ，但不会匹配 '<user@host.com' ，也不会匹配 'user@host.com>'。

由 '\' 和一个字符组成的特殊序列在以下列出。 如果普通字符不是ASCII数位或者ASCII字母，那么正则样式将匹配第二个字符。比如，\$ 匹配字符 '$'.

\number
匹配数字代表的组合。每个括号是一个组合，组合从1开始编号。比如 (.+) \1 匹配 'the the' 或者 '55 55', 但不会匹配 'thethe' (注意组合后面的空格)。这个特殊序列只能用于匹配前面99个组合。如果 number 的第一个数位是0， 或者 number 是三个八进制数，它将不会被看作是一个组合，而是八进制的数字值。在 '[' 和 ']' 字符集合内，任何数字转义都被看作是字符。

\A
只匹配字符串开始。

\b
匹配空字符串，但只在单词开始或结尾的位置。一个单词被定义为一个单词字符的序列。注意，通常 \b 定义为 \w 和 \W 字符之间，或者 \w 和字符串开始/结尾的边界， 意思就是 r'\bfoo\b' 匹配 'foo', 'foo.', '(foo)', 'bar foo baz' 但不匹配 'foobar' 或者 'foo3'。

默认情况下，Unicode字母和数字是在Unicode样式中使用的，但是可以用 ASCII 标记来更改。如果 LOCALE 标记被设置的话，词的边界是由当前语言区域设置决定的，\b 表示退格字符，以便与Python字符串文本兼容。

\B
匹配空字符串，但 不 能在词的开头或者结尾。意思就是 r'py\B' 匹配 'python', 'py3', 'py2', 但不匹配 'py', 'py.', 或者 'py!'. \B 是 \b 的取非，所以Unicode样式的词语是由Unicode字母，数字或下划线构成的，虽然可以用 ASCII 标志来改变。如果使用了 LOCALE 标志，则词的边界由当前语言区域设置。

\d
对于 Unicode (str) 样式：
匹配任何Unicode十进制数（就是在Unicode字符目录[Nd]里的字符）。这包括了 [0-9] ，和很多其他的数字字符。如果设置了 ASCII 标志，就只匹配 [0-9] 。

对于8位(bytes)样式：
匹配任何十进制数，就是 [0-9]。

\D
匹配任何非十进制数字的字符。就是 \d 取非。 如果设置了 ASCII 标志，就相当于 [^0-9] 。

\s
对于 Unicode (str) 样式：
匹配任何Unicode空白字符（包括 [ \t\n\r\f\v] ，还有很多其他字符，比如不同语言排版规则约定的不换行空格）。如果 ASCII 被设置，就只匹配 [ \t\n\r\f\v] 。

对于8位(bytes)样式：
匹配ASCII中的空白字符，就是 [ \t\n\r\f\v] 。

\S
匹配任何非空白字符。就是 \s 取非。如果设置了 ASCII 标志，就相当于 [^ \t\n\r\f\v] 。

\w
对于 Unicode (str) 样式：
匹配Unicode词语的字符，包含了可以构成词语的绝大部分字符，也包括数字和下划线。如果设置了 ASCII 标志，就只匹配 [a-zA-Z0-9_] 。

对于8位(bytes)样式：
匹配ASCII字符中的数字和字母和下划线，就是 [a-zA-Z0-9_] 。如果设置了 LOCALE 标记，就匹配当前语言区域的数字和字母和下划线。

\W
匹配任何不是单词字符的字符。 这与 \w 正相反。 如果使用了 ASCII 旗标，这就等价于 [^a-zA-Z0-9_]。 如果使用了 LOCALE 旗标，则会匹配在当前区域设置中不是字母数字又不是下划线的字符。

\Z
只匹配字符串尾。

绝大部分Python的标准转义字符也被正则表达式分析器支持。:

\a      \b      \f      \n
\N      \r      \t      \u
\U      \v      \x      \\
（注意 \b 被用于表示词语的边界，它只在字符集合内表示退格，比如 [\b] 。）

'\u', '\U' 和 '\N' 转义序列只在 Unicode 模式中可被识别。 在 bytes 模式中它们会导致错误。 未知的 ASCII 字母转义序列保留在未来使用，会被当作错误来处理。

八进制转义包含为一个有限形式。如果首位数字是 0， 或者有三个八进制数位，那么就认为它是八进制转义。其他的情况，就看作是组引用。对于字符串文本，八进制转义最多有三个数位长。

在 3.3 版更改: 增加了 '\u' 和 '\U' 转义序列。

在 3.6 版更改: 由 '\' 和一个ASCII字符组成的未知转义会被看成错误。

在 3.8 版更改: 添加了 '\N{name}' 转义序列。 与在字符串字面值中一样，它扩展了命名 Unicode 字符 (例如 '\N{EM DASH}')。
```

