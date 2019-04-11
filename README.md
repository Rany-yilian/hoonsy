#http_server

使用C来实现自己的http服务器，目前只实现了对于 请求方式 (get、post等)、还有url的解析

请求行存放在 Http_format 结构体中
请求头存放在链表 Node 中

其中还增加了与本地接口80端口(端口可自行修改)的代理转发
由于我是采用 apache监听端口，我则可以返回apache的首页，访问其他路径也可以返回apache的其他访问页面

后续还有持续更新，敬请期待



使用方法

进入项目根目录 


make

./http_server


访问方式  
浏览器打开  ip:8080 端口


