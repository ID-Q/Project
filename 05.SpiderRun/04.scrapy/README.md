# Python爬虫基于（Scrapy框架）

Scrapy是一个高级的Python爬虫框架，它不仅包含了爬虫的特性，还可以方便的将爬虫数据保存到csv、json等文件中。它能我们更好的完成爬虫任务，自己写Python爬虫程序好比孤军奋战，而使用了Scrapy就好比手底下有了千军万马。

![img](https:////upload-images.jianshu.io/upload_images/2255795-bedae096783b6ba7.png?imageMogr2/auto-orient/strip|imageView2/2/w/597)

scrapy.png

- Scrapy Engine(Scrapy核心) 负责数据流在各个组件之间的流。
- Spiders(爬虫)发出Requests请求，经由Scrapy Engine(Scrapy核心) 交给Scheduler(调度器)，Downloader(下载器)Scheduler(调度器) 获得Requests请求，然后根据Requests请求，从网络下载数据。
- Downloader(下载器)的Responses响应再传递给Spiders进行分析。根据需求提取出Items，交给Item Pipeline进行下载。Spiders和Item Pipeline是需要用户根据响应的需求进行编写的。
- 除此之外，还有两个中间件，Downloaders Mddlewares和Spider Middlewares，这两个中间件为用户提供方面，通过插入自定义代码扩展Scrapy的功能，例如去重等。