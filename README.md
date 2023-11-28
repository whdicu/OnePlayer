# OnePlayer

#### 介绍
一款界面简洁美观的音乐播放器


#### 功能介绍
支持循环播放、随机播放、单曲循环
音量控制：软件可以单曲控制音量大小，独立于系统音量
播放记忆：软件启动时播放上次退出时正在播放的歌曲，且从退出时的位置开始播放
封面显示：提取歌曲文件中的图片，显示在界面上
歌曲搜索：按ctrl+F可以对歌曲名进行搜索


#### 软件架构
软件架构说明


#### 安装教程 & 使用说明
1.  QT版本：6.6.0 \(之前使用6.3.2的msvc编译器时，会发生播放音乐卡顿的现象，6.3.2的mingw编译器就没这个问题\)
2.  VS版本：2022 \(用QtCreater的话稍作修改应该也能开发，没试过\)
3.  本项目中还用到了我的另一个项目OneDer，主要用它的DList来代替QList使用。需拉取，并配置包含路径
4.  使用QMediaPlayer的metaData来获取音乐图片，很多时候会获取不到，所以改用FFmpeg来获取音乐图片


#### 分支
1.  dev 开发分支
2.  master 能用的稳定分支


#### 特技
1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
