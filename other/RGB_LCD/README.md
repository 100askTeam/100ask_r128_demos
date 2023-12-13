# R128使用RGB LCD

这里的示例以百问网的7寸（1024*600）、4寸方屏（480*480）、4寸圆屏RGB（480*480） LCD为例。

硬件列表：

- 屏幕+驱动板：[https://item.taobao.com/item.htm?id=736154682975](https://item.taobao.com/item.htm?id=736154682975)
- DShanMCU-R128 RGBDevkit 显示开发学习板：[https://item.taobao.com/item.htm?id=736154682975](https://item.taobao.com/item.htm?id=736154682975)


## 选择devkit_rgb方案

初始化环境变量，选择方案：

```shell
100ask@100ask:~/R128-FreeRTOS/SDK$ source envsetup.sh 
Setup env done!
Run lunch_rtos to select project
100ask@100ask:~/R128-FreeRTOS/SDK$ lunch_rtos 
last=r128s2_devkit_rgb_c906

You're building on Linux

Lunch menu... pick a combo:
     1. r128s2_devkit_c906
     2. r128s2_devkit_m33
     3. r128s2_devkit_rgb_c906
     4. r128s2_devkit_rgb_m33
     5. r128s2_evt_c906
     6. r128s2_evt_m33

Which would you like? [Default r128s2_devkit_c906]: 3
select=3...
r128s2/devkit_rgb_c906
'/home/100ask/R128-FreeRTOS/SDK/lichee/rtos/projects/r128s2/devkit_rgb_c906/defconfig' -> '/home/100ask/R128-FreeRTOS/SDK/lichee/rtos/.config'
============================================
RTOS_BUILD_TOP=/home/100ask/R128-FreeRTOS/SDK
RTOS_TARGET_ARCH=riscv
RTOS_TARGET_CHIP=sun20iw2p1
RTOS_TARGET_DEVICE=r128s2
RTOS_PROJECT_NAME=r128s2_devkit_rgb_c906
============================================
Run mrtos_menuconfig to config rtos
Run m or mrtos to build rtos
```

## 配置menuconfig

menuconfig具体配置：

```shell

# 1.打开下面的 Drivers
Drivers Options  --->
    soc related device drivers  --->
        PWM devices --->
            [*] enable pwm driver
            [ ]   enable pwm hal APIs test command
        TWI Devices  --->
            -*- enable twi driver
            [ ]   enable twi hal APIs test command
        Video support for sunxi  --->
        	[*] DISP Driver Support(sunxi-disp2)


# 2. 打开LVGL相关配置
System components  --->
    thirdparty components  --->
        [*] Littlevgl-8  --->
            --- Littlevgl-8
            [*]   lvgl examples
            [ ]   lvgl g2d test
            [*]   lvgl-8.1.0 use sunxifb double buffer
            [*]   lvgl-8.1.0 use sunxifb cache
            [ ]   lvgl-8.1.0 use sunxifb g2d
            [ ]   lvgl-8.1.0 use sunxifb g2d rotate
            [ ]   lvgl-8.1.0 use freetype
            [ ]   lvgl-8.1.0 use direct mode
```

## 显示

### sys_config.fex配置

修改文件：`R128-FreeRTOS/board/r128s2/devkit_rgb/configs/sys_config.fex`

```shell
[lcd0]
lcd_used            = 1
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;; 7寸屏（1024x600）
lcd_driver_name     = "default_lcd"
lcd_backlight       = 150
lcd_if              = 0
lcd_x               = 1024
lcd_y               = 600
lcd_width           = 150
lcd_height          = 94
lcd_rb_swap         = 0
lcd_dclk_freq       = 48
lcd_pwm_used        = 1
lcd_pwm_ch          = 7
lcd_pwm_freq        = 500000
lcd_pwm_pol         = 1
lcd_hbp             = 150
lcd_ht              = 1344
lcd_hspw            = 20
lcd_vbp             = 20
lcd_vt              = 635
lcd_vspw            = 3
lcd_lvds_if         = 0
lcd_lvds_colordepth = 1
lcd_lvds_mode       = 0
lcd_frm             = 0
lcd_io_phase        = 0x0000
lcd_gamma_en        = 0
lcd_bright_curve_en = 0
lcd_cmap_en         = 0
;;;;;; 4寸方屏（480x480）
;lcd_driver_name     = "default_lcd"
;lcd_backlight       = 150
;lcd_if              = 0
;lcd_x               = 480
;lcd_y               = 480
;lcd_width           = 70
;lcd_height          = 70
;lcd_rb_swap         = 1
;lcd_dclk_freq       = 9
;lcd_pwm_used        = 1
;lcd_pwm_ch          = 7
;lcd_pwm_freq        = 500000
;lcd_pwm_pol         = 1
;lcd_hbp             = 20
;lcd_ht              = 526
;lcd_hspw            = 6
;lcd_vbp             = 10
;lcd_vt              = 510
;lcd_vspw            = 4
;lcd_lvds_if         = 0
;lcd_lvds_colordepth = 1
;lcd_lvds_mode       = 0
;lcd_frm             = 1
;lcd_io_phase        = 0x0000
;lcd_gamma_en        = 0
;lcd_bright_curve_en = 0
;lcd_cmap_en         = 0
;;;;;; 4寸圆屏RGB（480*480）
;lcd_driver_name     = "default_lcd"
;lcd_backlight       = 200
;lcd_if              = 0
;lcd_x               = 480
;lcd_y               = 480
;lcd_width           = 70
;lcd_height          = 70
;lcd_rb_swap         = 1
;lcd_dclk_freq       = 9
;lcd_pwm_used        = 1
;lcd_pwm_ch          = 7
;lcd_pwm_freq        = 2000
;lcd_pwm_pol         = 1
;lcd_hbp             = 20
;lcd_ht              = 526
;lcd_hspw            = 6
;lcd_vbp             = 10
;lcd_vt              = 510
;lcd_vspw            = 4
;lcd_lvds_if         = 0
;lcd_lvds_colordepth = 1
;lcd_lvds_mode       = 0
;lcd_frm             = 1
;lcd_io_phase        = 0x0000
;lcd_gamma_en        = 0
;lcd_bright_curve_en = 0
;lcd_cmap_en         = 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

deu_mode            = 0
lcdgamma4iep        = 0
smart_color         = 0

;LCD_D2-LCD_D7
lcd_gpio_0               = port:PA00<8><0><3><0>
lcd_gpio_1               = port:PA01<8><0><3><0>
lcd_gpio_2               = port:PA02<8><0><3><0>
lcd_gpio_3               = port:PA03<8><0><3><0>
lcd_gpio_4               = port:PA04<8><0><3><0>
lcd_gpio_5               = port:PA05<8><0><3><0>

;LCD_D10-LCD_D15
lcd_gpio_6               = port:PA11<8><0><3><0>
lcd_gpio_7               = port:PA10<8><0><3><0>
lcd_gpio_8               = port:PA08<8><0><3><0>
lcd_gpio_9               = port:PA07<8><0><3><0>
lcd_gpio_10              = port:PA06<8><0><3><0>
lcd_gpio_11              = port:PA09<8><0><3><0>

;LCD_D18-LCD_D23
lcd_gpio_12              = port:PA12<8><0><3><0>
lcd_gpio_13              = port:PA13<8><0><3><0>
lcd_gpio_14              = port:PA14<8><0><3><0>
lcd_gpio_15              = port:PA15<8><0><3><0>
lcd_gpio_16              = port:PB03<8><0><3><0>
lcd_gpio_17              = port:PB02<8><0><3><0>

;LCD_VSYNC, LCD_HSYNC, LCD_DCLK, LCD_DE
lcd_gpio_18              = port:PA18<8><0><3><0>
lcd_gpio_19              = port:PA19<8><0><3><0>
lcd_gpio_20              = port:PA20<8><0><3><0>
lcd_gpio_21              = port:PA21<8><0><3><0>

```

### LVGL配置

修改文件：`SDK/lichee/rtos-components/thirdparty/littlevgl-8/lv_examples/src/lv_conf.h`

```c
/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 32

/*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI)*/
#define LV_COLOR_16_SWAP 0
```


## 触摸

## 配置menuconfig

menuconfig具体配置：

```shell
Drivers Options  --->
    other drivers  --->
        [*] input driver  --->
        	[*]   keyboard driver  --->
        	[*]   touchscreen driver  --->
        		[ ]   enable tlsc6x touchscreen driver
        		[*]   enable gt911 touchscreen driver
        		[ ]   enable cst226se touchscreen driver
        		[ ]   enable touchscreen drivers APIs test command      
```
### sys_config.fex配置

修改文件：`R128-FreeRTOS/board/r128s2/devkit_rgb/configs/sys_config.fex`

```shell
[touchscreen]
tp_used                = 1
tp_int                 = port:PA24<1><0><default><default>
tp_reset               = port:PA25<1><0><default><default>
tp_revert_mode         = 0
tp_exchange_flag       = 0
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;; 7寸屏（1024x600） GT911
tp_max_x               = 1024
tp_max_y               = 600
tp_addr                = 0x14
;;;;;; 4寸方屏（480x480） GT911
;tp_max_x               = 480
;tp_max_y               = 480
;tp_addr                = 0x14
;;;;;; 4寸圆屏RGB（480*480）
;待续...
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
tp_twi_id              = 1

```

### LVGL配置

修改文件：`SDK/lichee/rtos-components/thirdparty/littlevgl-8/lv_examples/src/lv_drv_conf.h`

```c
/*-------------------------------------------------
 * Mouse or touchpad as evdev interface (for Linux based systems)
 *------------------------------------------------*/
#ifndef USE_EVDEV
#  define USE_EVDEV           1
#endif
```


## 结果

以上配置完成后，编译打包烧录，上电后屏幕背光亮起，屏幕为黑色。

并且可以在 LOG 中看到 `[LCD_FB] lcd_fb_probe,line:103:` 和 `spi_clk_init()1609 [spi1] clk rate auto adjust to 48000000` SPI 初始化的 LOG。

```shell

```

通过执行 **lv_examples** 命令运行LVGL测试demo。

`lv_examples` 代码位于： `SDK/lichee/rtos-components/thirdparty/littlevgl-8/lv_examples/src/lv_main.c`