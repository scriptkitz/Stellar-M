# 价签改温湿度计

#### 介绍
2.1寸汉朔的Stellar-M价签改造成温湿度计

#### 使用说明
功能说明
价签原板飞线接温湿度模块。AHT20和SHT30都支持，上电自动检测。定时10秒查询温湿度。温度差0.5或者湿度差1就刷新，6000秒后强制刷新，局刷30次全刷一次。SDA接价签原板的data，SCL接SCK

详细看我立创开源 [2.1价签无损改温湿度计](https://oshwhub.com/article/2.1jia-qian-wu-sun-gai-wen-shi-du-ji)


这次无损改造需要有编程器，不能用BSL下载，背后没有BSL下载脚。劝退没有编程器的同学。



 自从上次价签改时钟。改造时钟焊板子太麻烦。有没有简单的价签改造呢。想法来源群友有提了个时钟做个纯温湿度计固件
 ![](/img/SzJ0ex4aVH5GFPRmfOqg4mmK8UunhUyEKSqnvMa9.png)
有了想法，昨天有时间就赶紧写了下代码，显示如下，flash空间充足就写了自动检测支持AHT20和SHT30的温湿度驱动。建议用AHT20便宜好用
![](/img/RTEuSl64pivBESl7vwZ9IaONUbARZWaG9xVZyNpr.png)

这次无损改造需要有编程器，不能用BSL下载，背后没有BSL下载脚。劝退没有编程器的同学。

改造步骤如下：

1.选择价签

注意：价签必须使用汉朔的Stellar-M价签才能使用本固件

如下图：

![](/img/Ph4P2bXSnocekVDA3kpzbbT1sPskz65Iqa2BGXUQ.png)

![](/img/9I4Wjix19xjLxvq5opGlnU1fQqBr9x1mzUyaIzWe.png)

 

2.打开电池盖

 

![](/img/o2WvGupocqcR5Zdp7TrnZaf9VqT0N2TIiH9lsbGF.png)

拿一字螺丝刀捅下两个卡口往上顶就能把电池仓拿出来

打开的图如下

![](/img/uKGh6E9HhagGPCEYQ2mRlfbv7q0mfCQyUfdtAaLA.png)

 

接线说明

丝印data接温湿度模块的SDA  CLK接温湿度的SCL    模块电源接VCC和GND

![](/img/mujqu9DvZw94qmUuMI6sva96FFNFpM2nHD7iAz14.png)

 

3.下载固件

详细可以看我之前的开源MSP430 2.13寸墨水屏时钟

 

4.飞线温湿度模块

下面是我的接线。没买模块，下面是我的灵魂飞线芯片，AHT21焊盘比较大好飞线。

![](/img/J2Qu00x1jzzr3e9mnSgkBk7TDR7i8XctX8lZc3ek.png)

 

固件功能说明：

价签原板飞线接温湿度模块。AHT20和SHT30都支持，上电自动检测。定时10秒查询温湿度。温度差0.5或者湿度差1就刷新，6000秒后强制刷新，局刷30次全刷一次。SDA接价签原板的data，SCL接SCK

固件&&开源下载地址：

https://gitee.com/stw123456/msp403_RH.git

 

转接板&&支架看  2.1寸价签温湿度转接板

![](/img/eIeVJ9MY5szijvGcVMeCpdQhzaKpNYbDYxk5kncf.png)

 

 

群友改造完的图

![](/img/mefcIiHe3tifLf2jgXjNMjp0z6CgckabxHujtH28.png)

 

 
