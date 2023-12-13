# R128使用SPI LCD

这里的示例以百问网的SPI LCD为例。

## 接线

| LCD Pin | R128 Pin |
| :---- | :----- |
| GND | GND |
| PWM | PA26 |
| RESET | PA05 |
| MOSI | PA18 |
| SCK | PA13 |
| RS | PA04 |
| CS | PA12 |
| MISO | PA21 |
| 3.3V | 3.3V |
| GND | GND |

## 覆盖spilcd目录

将此目录下的名为`spilcd`的文件夹，复制覆盖到SDK中位于`R128-FreeRTOS/lichee/rtos-hal/hal/source/spilcd`的目录。

## 配置menuconfig

初始化环境变量，选择方案，进入到menuconfig：

```shell
100ask@100ask:~/R128-FreeRTOS/SDK$ source envsetup.sh 
Setup env done!
Run lunch_rtos to select project
100ask@100ask:~/R128-FreeRTOS/SDK$ lunch_rtos
last=r128s2_devkit_c906

You're building on Linux

Lunch menu... pick a combo:
     1. r128s2_devkit_c906
     2. r128s2_devkit_m33
     3. r128s2_devkit_rgb_c906
     4. r128s2_devkit_rgb_m33
     5. r128s2_evt_c906
     6. r128s2_evt_m33

Which would you like? [Default r128s2_devkit_c906]: 1
select=1...
r128s2/devkit_c906
'/home/100ask/R128-FreeRTOS/SDK/lichee/rtos/projects/r128s2/devkit_c906/defconfig' -> '/home/100ask/R128-FreeRTOS/SDK/lichee/rtos/.config'
============================================
RTOS_BUILD_TOP=/home/100ask/R128-FreeRTOS/SDK
RTOS_TARGET_ARCH=riscv
RTOS_TARGET_CHIP=sun20iw2p1
RTOS_TARGET_DEVICE=r128s2
RTOS_PROJECT_NAME=r128s2_devkit_c906
============================================
Run mrtos_menuconfig to config rtos
Run m or mrtos to build rtos
100ask@100ask:~/R128-FreeRTOS/SDK$
100ask@100ask:~/R128-FreeRTOS/SDK$
100ask@100ask:~/R128-FreeRTOS/SDK$ mrtos_menuconfig
```

menuconfig具体配置：

```shell

# 1.打开SPI
Drivers Options  --->
    soc related device drivers  --->
        SPI Devices   --->
            [*] enable spi driver
            [*]   support sp transfer if crash
            [ ]   enable spi hal APIs test command

# 2. 打开SPILCD，并打开100ASK相关的配置
Drivers Options  --->
    soc related device drivers  --->
        SPILCD Devices   --->
            [*] DISP Driver Support(spi_lcd)
            [*]   spilcd hal APIs test
            LCD_FB panels select  --->
                [ ] LCD support kld2844B panel
                [ ] LCD support kld35512 panel
                [ ] LCD support kld39501 panel
                [ ] LCD support nv3029s panel
                [ ] LCD support JLT35031C panel
                100ASK LCD configure  --->
                    [*] LCD support 100ask panel
                    Select the model of the display (320X480)  --->
                    (0) Set display rotation angle (90, 180, 270)
            Board select  --->
                [ ] board kld2844b support
                [ ] board kld35512 support
                [*] board lcd 100ask support

# 3. 打开LVGL相关配置
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


## 修改调试等级
修改文件：`R128-FreeRTOS/lichee/rtos-hal/hal/source/spi/hal_spi.c`

SPI 驱动中预留了 3 个调试宏开关，可以根据需求相应开启。

```c
	#define SPI_INFO_LEVEL    # 开启后驱动会将一些调试信息打印到串口终端上。
	#define SPI_DATA_LEVEL    # 开启后驱动会将 RX/TX buffer 中的数据打印到串口终端上方便观察。
	#define SPI_DUMPREG_LEVEL # 开启后驱动会在 RX/TX 时分别 dump 一次 SPI 的寄存器信息并打印到串口终端上方便观察。
```

## 修改默认ALIGN_DMA_BUF_SIZE
修改文件：`R128-FreeRTOS/lichee/rtos-hal/include/hal/sunxi_hal_spi.h`

```c
#ifdef CONFIG_USE_100ASK_DISPLAY_SCREEN_240X240
	#define ALIGN_DMA_BUF_SIZE ((240*240*4) + 64)
#elif CONFIG_USE_100ASK_DISPLAY_SCREEN_320X480
	#define ALIGN_DMA_BUF_SIZE ((320*480*4) + 64)
#else
	#define ALIGN_DMA_BUF_SIZE (4096 + 64)
#endif
```

## 注释SUPPORT_DBI_IF

修改文件：`R128-FreeRTOS/lichee/rtos-hal/hal/source/spilcd/lcd_fb/lcd_fb_feature.h`

```c
#if defined (CONFIG_ARCH_SUN50IW11) || defined(CONFIG_ARCH_SUN8IW20) || defined(CONFIG_ARCH_SUN20IW1) \
        || defined(CONFIG_ARCH_SUN20IW2)
//#define SUPPORT_DBI_IF
#endif
```


## sys_config.fex配置

修改文件：`R128-FreeRTOS/board/r128s2/pro/configs/sys_config.fex`

```shell

;----------------------------------------------------------------------------------
;SPI controller configuration
;----------------------------------------------------------------------------------
;Please config spi in dts
[spi1]
spi1_used       = 1
spi1_cs_number  = 1
spi1_cs_bitmap  = 1
spi1_cs0        = port:PA12<6><0><2><default>
spi1_sclk       = port:PA13<6><0><2><default>
spi1_mosi       = port:PA18<6><0><2><default>
spi1_miso       = port:PA21<6><0><2><default>
spi1_hold       = port:PA19<6><0><2><default>
spi1_wp         = port:PA20<6><0><2><default>

[pwm6]
pwm_used        = 1
pwm_positive    = port:PA26<4><0><2><default>

;----------------------------------------------------------------------------------
;lcd_fb0 configuration
;----------------------------------------------------------------------------------

[lcd_fb0]
lcd_used            = 1
lcd_model_name      = "spilcd"
lcd_driver_name     = "lcd_100ask_spi"
lcd_x               = 320
lcd_y               = 480
lcd_width           = 49
lcd_height          = 73
lcd_data_speed      = 65
lcd_pwm_used        = 1
lcd_pwm_ch          = 6
lcd_pwm_freq        = 5000
lcd_pwm_pol         = 0
lcd_if              = 0
lcd_pixel_fmt       = 11
lcd_dbi_fmt         = 2
lcd_dbi_clk_mode    = 1
lcd_dbi_te          = 1
fb_buffer_num       = 2
lcd_dbi_if          = 4
lcd_rgb_order       = 0
lcd_fps             = 10
lcd_spi_bus_num     = 1
lcd_frm             = 2
lcd_gamma_en        = 1
lcd_backlight       = 100

lcd_power_num       = 0
lcd_gpio_regu_num   = 0
lcd_bl_percent_num  = 0

;;lcd_spi_dc_pin      = port:PA19<1><0><3><0>
lcd_spi_dc_pin      = port:PA04<1><0><3><0>
;lcd_gpio_0          = port:PA12<1><0><2><0>



;----------------------------------------------------------------------------------
;lcd0 configuration

;lcd_if:               0:hv(sync+de); 1:8080; 2:ttl; 3:lvds; 4:dsi; 5:edp; 6:extend dsi
;lcd_x:                lcd horizontal resolution
;lcd_y:                lcd vertical resolution
;lcd_width:            width of lcd in mm
;lcd_height:           height of lcd in mm
;lcd_dclk_freq:        in MHZ unit
;lcd_pwm_freq:         in HZ unit
;lcd_pwm_pol:          lcd backlight PWM polarity
;lcd_pwm_max_limit     lcd backlight PWM max limit(<=255)
;lcd_hbp:              hsync back porch
;lcd_ht:               hsync total cycle
;lcd_vbp:              vsync back porch
;lcd_vt:               vysnc total cycle
;lcd_hspw:             hsync plus width
;lcd_vspw:             vysnc plus width
;lcd_lvds_if:          0:single link;  1:dual link
;lcd_lvds_colordepth:  0:8bit; 1:6bit
;lcd_lvds_mode:        0:NS mode; 1:JEIDA mode
;lcd_frm:              0:disable; 1:enable rgb666 dither; 2:enable rgb656 dither
;lcd_io_phase:         0:noraml; 1:intert phase(0~3bit: vsync phase; 4~7bit:hsync phase;
;                      8~11bit:dclk phase; 12~15bit:de phase)
;lcd_gamma_en          lcd gamma correction enable
;lcd_bright_curve_en   lcd bright curve correction enable
;lcd_cmap_en           lcd color map function enable
;deu_mode              0:smoll lcd screen; 1:large lcd screen(larger than 10inch)
;lcdgamma4iep:         Smart Backlight parameter, lcd gamma vale * 10;
;                      decrease it while lcd is not bright enough; increase while lcd is too bright
;smart_color           90:normal lcd screen 65:retina lcd screen(9.7inch)
;----------------------------------------------------------------------------------
;[lcd0]
;lcd_used            = 1
;
;lcd_driver_name     = "default_lcd"
;lcd_backlight       = 150
;lcd_if              = 0
;lcd_x               = 800
;lcd_y               = 480
;lcd_width           = 150
;lcd_height          = 94
;lcd_rb_swap         = 0
;lcd_dclk_freq       = 33
;lcd_pwm_used        = 1
;lcd_pwm_ch          = 6
;lcd_pwm_freq        = 5000
;lcd_pwm_pol         = 1
;lcd_hbp             = 46
;lcd_ht              = 1055
;lcd_hspw            = 0
;lcd_vbp             = 23
;lcd_vt              = 525
;lcd_vspw            = 0
;lcd_lvds_if         = 0
;lcd_lvds_colordepth = 1
;lcd_lvds_mode       = 0
;lcd_frm             = 0
;lcd_io_phase        = 0x0000
;lcd_gamma_en        = 0
;lcd_bright_curve_en = 0
;lcd_cmap_en         = 0
;
;deu_mode            = 0
;lcdgamma4iep        = 22
;smart_color         = 90
;
;;LCD_D2-LCD_D7
;lcd_gpio_0               = port:PA00<8><0><3><0>
;lcd_gpio_1               = port:PA01<8><0><3><0>
;lcd_gpio_2               = port:PA02<8><0><3><0>
;lcd_gpio_3               = port:PA03<8><0><3><0>
;lcd_gpio_4               = port:PA04<8><0><3><0>
;lcd_gpio_5               = port:PA05<8><0><3><0>
;
;;LCD_D10-LCD_D15
;lcd_gpio_6               = port:PA11<8><0><3><0>
;lcd_gpio_7               = port:PA10<8><0><3><0>
;lcd_gpio_8               = port:PA08<8><0><3><0>
;lcd_gpio_9               = port:PA07<8><0><3><0>
;lcd_gpio_10              = port:PA06<8><0><3><0>
;lcd_gpio_11              = port:PA09<8><0><3><0>
;
;;LCD_D18-LCD_D23
;lcd_gpio_12              = port:PA12<8><0><3><0>
;lcd_gpio_13              = port:PA13<8><0><3><0>
;lcd_gpio_14              = port:PA14<8><0><3><0>
;lcd_gpio_15              = port:PA15<8><0><3><0>
;lcd_gpio_16              = port:PB03<8><0><3><0>
;lcd_gpio_17              = port:PB02<8><0><3><0>
;
;;LCD_VSYNC, LCD_HSYNC, LCD_DCLK, LCD_DE
;lcd_gpio_18              = port:PA18<8><0><3><0>
;lcd_gpio_19              = port:PA19<8><0><3><0>
;lcd_gpio_20              = port:PA20<8><0><3><0>
;lcd_gpio_21              = port:PA21<8><0><3><0>
```

## lvgl配置

### 修改lv_conf.h

修改文件：`lichee/rtos-components/thirdparty/littlevgl-8/lv_examples/src/lv_conf.h`

```c
/*====================
   COLOR SETTINGS
 *====================*/

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI)*/
#define LV_COLOR_16_SWAP 1
```

### 修改lv_main.c

修改文件：`lichee/rtos-components/thirdparty/littlevgl-8/lv_examples/src/lv_main.c`

添加屏幕旋转处理：

```c
/*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = sunxifb_flush;
#if ((CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 0) || (CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 180))
    disp_drv.hor_res    = width;
    disp_drv.ver_res    = height;
#elif ((CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 90) || (CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 270))
    disp_drv.hor_res    = height;
    disp_drv.ver_res    = width;
#else
    disp_drv.hor_res    = width;
    disp_drv.ver_res    = height;
#endif

    disp_drv.rotated    = rotated;
#ifndef USE_SUNXIFB_G2D_ROTATE
    if (rotated != LV_DISP_ROT_NONE)
        disp_drv.sw_rotate = 1;
#endif
    lv_disp_drv_register(&disp_drv);
```

