| Supported Targets | [R128 DevKit](https://www.aliexpress.us/item/3256805799420975.html) |
| ----------------- | ------------ |

---
<p align="right">
  <b>English</b> | <a href="./README_zh.md">中文</a></a>
</p>

# Introduction

100ask_r128_demos is positioned as a third-party library for R128, so it is an application example written based on the underlying interface provided by the R128 SDK, including the following sections:

| | |
| :--- | :--- |
|get_started | [./get_started/README.md](./get_started/README.md) |
|lv_100ask_desktop | [./lv_100ask_desktop/README.md](./lv_100ask_desktop/README.md) |
|module_driver | [./module_driver/README.md](./module_driver/README.md) |
|nwatch | [./nwatch/README.md](./nwatch/README.md) |
|other | [./other/README.md](./other/README.md) 
|More todo... | More todo... |

# Usage

As a third-party library for R128, 100ask_r128_demos is very simple to use:

1. Copy the 100ask_r128_demos directory to this directory in sdk: `R128-S2-SDK/lichee/rtos-components/thirdparty`
2. Open the file `R128-S2-SDK/lichee/rtos-components/thirdparty/Makefile` for editing and add the following content at the end/beginning of the file:

```shell
obj-$(CONFIG_COMPONENTS_100ASK_R128_DEMOS) += 100ask_r128_demos/
```

3. Open the file `R128-S2-SDK/lichee/rtos-components/thirdparty/Kconfig` for editing and add the following content at the end/beginning of the file:

```shell
source components/common/thirdparty/100ask_r128_demos/Kconfig
```

4. Execute  **mrtos_menuconfig** to enter the location shown below for configuration:

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

5. Compile and package： **mrtos && pack**

# Forum

- Allwinne: [https://bbs.aw-ol.com](https://bbs.aw-ol.com/recent?cid[]=2)
- 100ASK: [https://forums.100ask.net](https://forums.100ask.net)
