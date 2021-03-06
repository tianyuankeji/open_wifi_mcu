# smart_dht12_sensor_demo：模拟智能温湿度传感器
------------------------------------------

## 开发演示视频

[模拟智能温湿度传感器-演示视频](https://smart-img.5itianyuan.com/open/smart_dht12_sensor_demo.mp4)

## 功能说明

> 基于甜园智能云平台开发板，驱动DHT12温湿度传感器采集数据模拟智能温湿度传感器

## 硬件操作说明

> KEY2长按入网（3S），当长按**KEY2**按键，**LED4**进入闪烁状态开发板开始入网，通过**甜园app[添加设备步骤](app_add_wifidevelopboard.md)**,入网添加开发板设备，当wifi模块入网成功LED4指示灯常亮，并且wifi状态指示灯常亮

> 通过甜园APP添加开发板成功后，在app打开设备会接收到开发板采集的温湿度数据，点击温度数据显示栏，会跳转到采集的历史数据曲线图，方便查看历史数据

## Demo程序结构介绍

#### 系统时钟、定时器以及外围接口的初始化等

> RCC时钟配置，NVIC配置，初始化Time2定时器用于计时，初始化按键，初始化LED灯等外围设备

#### 串口透传数据解析

> 初始化uart3，波特率为115200，根据甜园智能云平台的wifi串口协议，进行数据的传输以及解析。

#### IIC接口初始化

> 将[DHT12传感器](DHT12_data_sheet.pdf)的SDA引脚接口与PA4连接，SCL引脚与PA5连接，并将电源引脚接入开发板5V供电将GND接地，初始化PA4、PA5作为IIC的SDA以及SCL接口，用于IIC数据传输

#### 数据上报初始化

> 初始化Timer3用于定时采集上报温湿度数值

#### 按键控制功能

> 初始化按键KEY2，并初始化LED4，其中LED4用于wifi入网状态显示操作，PB15开漏输出用于RSTwifi模块

> 采用轮询方式对按键监控处理，通过按键的动作执行入网等操作，并将该动作通过协议帧格式发送给wifi并上报到甜园云服务器，实现智能化控制。

## 注意

> [甜园云平台开发板开发使用注意事项](warning.md)
