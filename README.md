# http-listen-win32
用于单纯的查看客户端提交过来的HTTP请求内容，win32版本

请看这里：[http://blog.kijk.top/?p=16](http://blog.kijk.top/?p=16)

做这个小工具的初衷是因为在开发第三方支付平台的时候，调试通知有没有到达，和回调带具体参数时候比较麻烦，所以就写了这个小工具来监听端口，方便查看数据。

注：暂不支持https，请先使用http，准备再写个Linux版本的。


使用方法：

1、git clone 本项目 或者直接下载项目中编译好的http-listen.exe 到你的Windows计算机上面。

2、在Windows主机或者服务器的命令行模式下直接运行 ```http-listen.exe -l 10082```，这里的“-l”参数表示你需要监听的端口号，不加的话默认是10082，出现了http-listen的logo后表示监听成功。

 ![image](http://blog.kijk.top/wp-content/uploads/2020/09/cut001.jpg)

3、通过任意HTTP客户端（如Postman,Chrome浏览器等）向你的主机或者域名发送请求，格式如"http://xxx.xxx.xxx.xxx:10082/xxxx" ，http-listen会打印出HTTP客户端的请求信息，内容如下：

    ===========================================
    A new connection xxx.xxx.xxx.xxx:6936
    ===========================================
    POST /pay/notify HTTP/1.1
    User-Agent: PostmanRuntime/7.26.3
    Accept: */*
    Cache-Control: no-cache
    Postman-Token: fde41b9c-74b9-4772-9cfa-3f6521f29eac
    Host: xxx.xxx.xxx.xxx:10082
    Accept-Encoding: gzip, deflate, br
    Connection: keep-alive
    Content-Type: application/x-www-form-urlencoded
    Content-Length: 25
    
    sn=a251131515&uid=7755862

