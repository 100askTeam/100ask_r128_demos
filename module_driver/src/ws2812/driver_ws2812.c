// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */
 
/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 * 免责声明: 百问网编写的文档, 仅供学员学习使用, 可以转发或引用(请保留作者信息),禁止用于商业用途！
 * 免责声明: 百问网编写的程序, 可以用于商业用途, 但百问网不承担任何后果！
 * 
 * 文件名称：driver_ws2812.c
 * 本程序遵循GPL V3协议, 请遵循协议
 * 百问网学习平台   : https://www.100ask.net
 * 百问网交流社区   : https://forums.100ask.net
 * 百问网官方B站    : https://space.bilibili.com/275908810
 * 本程序所用开发板 : DShanMCU-F103
 * 百问网官方淘宝   : https://100ask.taobao.com
 * 联系我们(E-mail): weidongshan@qq.com
 *
 *          版权所有，盗版必究。
 *  
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.12.7   v0.1         zhouyuebiao   创建文件
 *-----------------------------------------------------
 */

#include <console.h>
#include <unistd.h>
#include <hal_log.h>
#include <hal_time.h>
#include <hal_timer.h>
#include <sunxi_hal_ledc.h>


// 使用RGB 分量合成颜色值
#define MERAGECOLOR(G, R, B) (((uint32_t)G << 16) | ((uint16_t)R << 8) | B)
#define PIXEL_NUM 4

// 生成颜色
uint32_t WS281x_Wheel(uint8_t wheelPos) {
  wheelPos = 255 - wheelPos;
  if (wheelPos < 85) {
    return MERAGECOLOR(255 - wheelPos * 3, 0, wheelPos * 3);
  }
  if (wheelPos < 170) {
    wheelPos -= 85;
    return MERAGECOLOR(0, wheelPos * 3, 255 - wheelPos * 3);
  }
  wheelPos -= 170;
  return MERAGECOLOR(wheelPos * 3, 255 - wheelPos * 3, 0);
}

// 亮度设置
uint32_t WS281xLSet(uint32_t rgb, float k) {
    uint8_t r, g, b;
    float h, s, v;
    uint8_t cmax, cmin, cdes;

    r = (uint8_t) (rgb >> 16);
    g = (uint8_t) (rgb >> 8);
    b = (uint8_t) (rgb);

    cmax = r > g ? r : g;
    if (b > cmax)
        cmax = b;
    cmin = r < g ? r : g;
    if (b < cmin)
        cmin = b;
    cdes = cmax - cmin;

    v = cmax / 255.0f;
    s = cmax == 0 ? 0 : cdes / (float) cmax;
    h = 0;

    if (cmax == r && g >= b)
        h = ((g - b) * 60.0f / cdes) + 0;
    else if (cmax == r && g < b)
        h = ((g - b) * 60.0f / cdes) + 360;
    else if (cmax == g)
        h = ((b - r) * 60.0f / cdes) + 120;
    else
        h = ((r - g) * 60.0f / cdes) + 240;

    v *= k;

    float f, p, q, t;
    float rf, gf, bf;
    int i = ((int) (h / 60) % 6);
    f = (h / 60) - i;
    p = v * (1 - s);
    q = v * (1 - f * s);
    t = v * (1 - (1 - f) * s);
    switch (i) {
    case 0:
        rf = v;
        gf = t;
        bf = p;
        break;
    case 1:
        rf = q;
        gf = v;
        bf = p;
        break;
    case 2:
        rf = p;
        gf = v;
        bf = t;
        break;
    case 3:
        rf = p;
        gf = q;
        bf = v;
        break;
    case 4:
        rf = t;
        gf = p;
        bf = v;
        break;
    case 5:
        rf = v;
        gf = p;
        bf = q;
        break;
    default:
        break;
    }

    r = (uint8_t) (rf * 255.0);
    g = (uint8_t) (gf * 255.0);
    b = (uint8_t) (bf * 255.0);

    return ((uint32_t) r << 16) | ((uint32_t) g << 8) | b;
}

// 延时函数
static inline int msleep(int ms) {
    vTaskDelay(ms / portTICK_RATE_MS); 
}


/**********************************************************************
 * 函数名称： ws2812_test
 * 功能描述： 测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
int ws2812_test(int argc, char *argv[])
{
  int i = 0, j = 0, err;
  int mode = 0;
  uint8_t R = 0, G = 0, B = 0;

  err = hal_ledc_init();
  if (err) {
    printf("ledc init error\n");
    return -1;
  }

  while (1) {
    for (j = 0; j < 256; j++) {
      for (i = 0; i < PIXEL_NUM; i++) {
        sunxi_set_led_brightness(
            i + 1, WS281xLSet(WS281x_Wheel(((i * 256 / PIXEL_NUM) + j) & 255), 0.2));
        msleep(1);
      }
      msleep(10);
    }
  }
  return 0;

}

FINSH_FUNCTION_EXPORT_CMD(ws2812_test, test_100ask_ws2812, module driver 100ask test ws2812);
