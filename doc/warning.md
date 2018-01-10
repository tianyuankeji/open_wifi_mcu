## 注意

> esp8266固件请勿私自烧录改动，如损坏固件则无法恢复

> esp8266的wifi状态指示灯开机正常运行状态为闪烁，入网成功后会停止闪烁并常亮

> stm32的demo示例中必须将PB15设置成开漏输出，否则esp8266模块会无法正常启动

> MCU与esp8266对接，联网过程中esp8266模块会通过串口向MCU请求消息，包括**chipid**、**vcode**、**pcode**、**hardwareversion**、**softwareversion**等必要的注册信息
