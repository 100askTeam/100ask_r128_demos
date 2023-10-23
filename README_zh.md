| Supported Targets | [R128 DevKit](https://item.taobao.com/item.htm?id=736154682975) |
| ----------------- | ------------ |

---
<p align="right">
  <a href="./README.md">English</a>  |  <b>中文</b></a>
</p>

# 介绍

100ask_r128_demos 的定位是作为R128的第三方库使用，因此其是基于R128SDK提供的底层接口而编写的应用示例，包括下表所示的几大部分：

| | |
| :--- | :--- |
|lv_100ask_desktop | [./lv_100ask_desktop/README.md](./lv_100ask_desktop/README.md) |
|module_driver | [./module_driver/README.md](./module_driver/README.md) |
|nwatch | [./nwatch/README.md](./nwatch/README.md) |
|More todo... | More todo... |


# 用法

作为R128的第三方库， 100ask_r128_demos 使用起来非常简单：

1. 将 100ask_r128_demos 目录复制到sdk的此目录下 `R128-S2-SDK/lichee/rtos-components/thirdparty`
2. 打开文件 `R128-S2-SDK/lichee/rtos-components/thirdparty/Makefile` 进行编辑，在文件的最后/最前加入下面的内容：

```shell
obj-$(CONFIG_COMPONENTS_100ASK_R128_DEMOS) += 100ask_r128_demos/
```

3. 打开文件 `R128-S2-SDK/lichee/rtos-components/thirdparty/Kconfig` 进行编辑，在文件的最后/最前加入下面的内容：

```shell
source components/common/thirdparty/100ask_r128_demos/Kconfig
```

4. 执行 **mrtos_menuconfig** 进入到下面所示的位置进行配置：

```shell
System components  --->
    thirdparty components  --->
        USE 100ask_r128_demos  --->
            --- USE 100ask_r128_demos
            [*]   USE USE_100ASK_MODULE_DRIVER  --->
            [ ]   100ask lvgl desktop
            [*]   NWatch demo
            ......
```

5. 编译、打包： **mrtos && pack**


# 技术交流

- Allwinne: [https://bbs.aw-ol.com](https://bbs.aw-ol.com/recent?cid[]=2)
- 100ASK: [https://forums.100ask.net](https://forums.100ask.net)
