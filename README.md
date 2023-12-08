# 价签改温湿度计

#### 介绍
2.1寸汉朔的Stellar-M价签改造成温湿度计
#### 软件架构
软件架构说明


#### 安装教程

1.  xxxx
2.  xxxx
3.  xxxx

#### 使用说明
功能说明
价签原板飞线接温湿度模块。AHT20和SHT30都支持，上电自动检测。定时10秒查询温湿度。温度差0.5或者湿度差1就刷新，6000秒后强制刷新，局刷30次全刷一次。SDA接价签原板的data，SCL接SCK

详细看我立创开源 [2.1价签无损改温湿度计](https://oshwhub.com/article/2.1jia-qian-wu-sun-gai-wen-shi-du-ji)
#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)

这次无损改造需要有编程器，不能用BSL下载，背后没有BSL下载脚。劝退没有编程器的同学。

 

改造步骤如下：

1.选择价签

注意：价签必须使用汉朔的Stellar-M价签才能使用本固件

如下图：

Ph4P2bXSnocekVDA3kpzbbT1sPskz65Iqa2BGXUQ.png

9I4Wjix19xjLxvq5opGlnU1fQqBr9x1mzUyaIzWe.png

 

2.打开电池盖

 

o2WvGupocqcR5Zdp7TrnZaf9VqT0N2TIiH9lsbGF.png

拿一字螺丝刀捅下两个卡口往上顶就能把电池仓拿出来

打开的图如下

uKGh6E9HhagGPCEYQ2mRlfbv7q0mfCQyUfdtAaLA.png

 

接线说明

丝印data接温湿度模块的SDA  CLK接温湿度的SCL    模块电源接VCC和GND

mujqu9DvZw94qmUuMI6sva96FFNFpM2nHD7iAz14.png

 

3.下载固件

详细可以看我之前的开源MSP430 2.13寸墨水屏时钟

 

4.飞线温湿度模块

下面是我的接线。没买模块，下面是我的灵魂飞线芯片，AHT21焊盘比较大好飞线。

J2Qu00x1jzzr3e9mnSgkBk7TDR7i8XctX8lZc3ek.png

 

固件功能说明：

价签原板飞线接温湿度模块。AHT20和SHT30都支持，上电自动检测。定时10秒查询温湿度。温度差0.5或者湿度差1就刷新，6000秒后强制刷新，局刷30次全刷一次。SDA接价签原板的data，SCL接SCK

固件&&开源下载地址：

https://gitee.com/stw123456/msp403_RH.git

 

转接板&&支架看  2.1寸价签温湿度转接板

eIeVJ9MY5szijvGcVMeCpdQhzaKpNYbDYxk5kncf.png

 

 

群友改造完的图

mefcIiHe3tifLf2jgXjNMjp0z6CgckabxHujtH28.png

 

 
