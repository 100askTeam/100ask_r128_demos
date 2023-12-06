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
 * 文件名称： blink.c
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
 * 2023.12.06   v0.1         zhouyuebiao   创建文件
 *-----------------------------------------------------
 */
#include <stdio.h>
#include <console.h>
#include <unistd.h>

#include <hal_time.h>
#include <hal_gpio.h>

#define LED_TESET_PIN   GPIOA(18)

int blink_test(int argc, char *argv[])
{
    uint16_t count = 0;

    hal_gpio_set_pull(LED_TESET_PIN, GPIO_PULL_UP);                 // 配置 GPIO 的上下拉状态
    hal_gpio_set_direction(LED_TESET_PIN, GPIO_DIRECTION_OUTPUT);   // 配置 GPIO 输入输出模式
    hal_gpio_pinmux_set_function(LED_TESET_PIN, GPIO_MUXSEL_OUT);   // 配置 GPIO 的 MUX 功能
    hal_gpio_set_driving_level(LED_TESET_PIN, GPIO_DRIVING_LEVEL2); // 配置 GPIO 的驱动能力

    while(1){

        hal_gpio_set_data(LED_TESET_PIN, GPIO_DATA_HIGH);  // 配置 GPIO 的电平
        printf("LED ON %d\n", count);
        hal_mdelay(1000);
        hal_gpio_set_data(LED_TESET_PIN, GPIO_DATA_LOW);  // 配置 GPIO 的电平
        printf("LED OFF %d\n", count);
        hal_mdelay(1000);

        hal_gpio_set_data(LED_TESET_PIN, GPIO_DATA_HIGH);  // 配置 GPIO 的电平
        printf("LED ON %d\n", count);
        vTaskDelay(1000 / portTICK_RATE_MS);
        hal_gpio_set_data(LED_TESET_PIN, GPIO_DATA_LOW);  // 配置 GPIO 的电平
        printf("LED OFF %d\n", count);
        vTaskDelay(1000 / portTICK_RATE_MS);

        hal_gpio_set_data(LED_TESET_PIN, GPIO_DATA_HIGH);  // 配置 GPIO 的电平
        printf("LED ON %d\n", count);
        hal_msleep(1000);
        hal_gpio_set_data(LED_TESET_PIN, GPIO_DATA_LOW);  // 配置 GPIO 的电平
        printf("LED OFF %d\n", count);
        hal_msleep(1000);

        hal_gpio_set_data(LED_TESET_PIN, GPIO_DATA_HIGH);  // 配置 GPIO 的电平
        printf("LED ON %d\n", count);
        sleep(1);
        hal_gpio_set_data(LED_TESET_PIN, GPIO_DATA_LOW);  // 配置 GPIO 的电平
        printf("LED OFF %d\n", count);
        sleep(1);

        count++;
    }

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(blink_test, test_100ask_blink, get started: blink);
