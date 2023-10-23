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
 * 文件名称： driver_spiflash_w25q64.c
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
 * 2023.10.16   v01         zhouyuebiao   创建文件
 *-----------------------------------------------------
 */

#ifndef _LCD_100ASK_SPI_H
#define _LCD_100ASK_SPI_H

#include <stdint.h>

typedef enum
{
    LCD_DISPLAY_ROTATION_0 = 0,
    LCD_DISPLAY_ROTATION_90,
    LCD_DISPLAY_ROTATION_180,
    LCD_DISPLAY_ROTATION_270,
} lcd_display_rotation_t;


typedef struct {
    int16_t x1;
    int16_t y1;
    int16_t x2;
    int16_t y2;
} lcd_display_area_t;

/**********************************************************************
 * 函数名称： W25Q64_Init
 * 功能描述： W25Q64的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void lcd_100ask_spi_init(void);

void lcd_100ask_spi_set_clear(uint16_t color_le);

void lcd_100ask_spi_set_window(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y);

/**********************************************************************
 * 函数名称： W25Q64_Test
 * 功能描述： W25Q64测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
int lcd_100ask_spi_test(int argc, char *argv[]);


#endif /* _LCD_100ASK_SPI_H */

