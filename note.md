# 关于Remix_OS的若干说明

标签： RTOS 开发文档

[TOC]

---

##1 优先级
| Priority   | G1/Byets  |  G2/Bytes  |  G3/Bytes | G4/Bytes  |  RAM/Bytes  | Type  |
| --------   | -----     |   ----     |     ----  | ----      |  ----       | ----  |
| 8          |   1       | 0          | 0         | 0         |  65         | U8    |
| 16         |   2       | 1          | 0         | 0         |  131        | U8    |
| 32         |   4       | 1          | 0         | 0         |  261        | U8    |
| 64         |   8       | 1          | 0         | 0         |  521        | U8    |
| 128        |   16      | 2          | 1         | 0         |  1043       | U8    |
| 256        |   32      | 4          | 1         | 0         |  2085       | U8    |
| 512        |   64      | 8          | 1         | 0         |  4241       | U16   |
| 1024       |   128     | 16         | 2         | 1         |  8339       | U16   |
| 2048       |   256     | 32         | 4         | 1         |  16677      | U16   |

##2 节点插入Delay表的比较
Delay表是按照延时时间递增顺序排列的双向链表。
需要根绝guiTick(系统当前TIck)、uiStillTick(待插入Task的延时时间)和uiTempStillTick(当前比较节点的延时时间)三者关系来进行比较确定。根据排列组合，一共有如下六种可能。

 - *guiTick < uiStillTick < uiTempStillTick*
> **uiStillTick插入到uiTempStillTick的前面。**待插入节点比当前比较节点的醒来时刻要早。
 
 - uiStillTick < guiTick < uiTempStillTick
> 继续与下一个uiTempStillTick比较。uiStillTick比当前TIck值guiTick小说明待插入节点的醒来时刻在下一个Tick计数周期中。Tick计数值从0x00000000到0xFFFFFFFF。
 
 - *uiStillTick < uiTempStillTick < guiTick*
> **uiStillTick插入到uiTempStillTick的前面。**待插入节点比当前比较节点的醒来时刻要早。尽管它们的醒来时刻都在下一个Tick计数周期中。
 
 - guiTick < uiTempStillTick < uiStillTick
> 继续与下一个uiTempStillTick比较。待插入节点比当前比较节点的醒来时刻要晚。
 
 - *uiTempStillTick < guiTick < uiStillTick*
> **uiStillTick插入到uiTempStillTick的前面。**uiTempStillTick是在Tick下个计数周期醒来，而uiStillTick是在当前计数周期醒来。

 - uiTempStillTick < uiStillTick < guiTick
> 继续与下一个uiTempStillTick比较。uiStillTick和uiTempStillTick都在下一个Tick计数周期醒来，并且uiStillTick醒的要晚。