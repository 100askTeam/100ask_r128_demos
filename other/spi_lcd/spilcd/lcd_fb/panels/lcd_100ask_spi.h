/*
 * R128-FreeRTOS/lichee/rtos-hal/hal/source/spilcd/lcd_fb/panels/lcd_100ask_spi.h
 *
 * Copyright (c) 2008-2023 Shenzhen Baiwenwang Technology CO LTD
 * Author: zhouyuebiao <https://forums.100ask.net>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef _LCD_100ASK_SPI_H
#define _LCD_100ASK_SPI_H

#include "panels.h"

struct __lcd_panel lcd_100ask_spi_panel;

typedef enum
{
    LCD_DISPLAY_ROTATION_0 = 0,
    LCD_DISPLAY_ROTATION_90,
    LCD_DISPLAY_ROTATION_180,
    LCD_DISPLAY_ROTATION_270,
} lcd_display_rotation_t;

#endif /*End of file*/
