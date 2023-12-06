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
 * 文件名称： hello_world.c
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

#include <hal_log.h>
#include <hal_time.h>

int hello_world_test(int argc, char *argv[])
{
    uint16_t count = 0;

    while(1){
        count = 0;
        printf("[100ask r128]: Hello World! %d\n", count++);
        hal_mdelay(1000);
        printf("[100ask r128]: Hello World! %d\n", count++);
        vTaskDelay(1000 / portTICK_RATE_MS);
        printf("[100ask r128]: Hello World! %d\n", count++);
        hal_msleep(1000);
        printf("[100ask r128]: Hello World! %d\n", count++);
        sleep(1);
    }

	return 0;
}

FINSH_FUNCTION_EXPORT_CMD(hello_world_test, test_100ask_hello_world, get started: hello world);
