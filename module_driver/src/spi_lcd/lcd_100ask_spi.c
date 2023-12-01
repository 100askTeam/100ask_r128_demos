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
 * 文件名称： lcd_100ask_spi.c
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



#include <console.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <hal_log.h>
#include <hal_cmd.h>
#include <hal_mem.h>

#include <sunxi_hal_spi.h>
#include <sunxi_hal_pwm.h>
#include <hal_timer.h>

#include "lcd_100ask_spi.h"

#define LCD_100ASK_SPI_PORT                 CONFIG_100ASK_MODULE_DRIVER_SPI_LCD_SPI_PORT
#define LCD_100ASK_SPI_FREQUENCY            CONFIG_100ASK_MODULE_DRIVER_SPI_LCD_FREQUENCY

#define LCD_100ASK_SCREEN_WIDTH             CONFIG_100ASK_MODULE_DRIVER_SPI_LCD_WIDTH
#define LCD_100ASK_SCREEN_HEIGHT            CONFIG_100ASK_MODULE_DRIVER_SPI_LCD_HEIGHT
#define LCD_100ASK_SPI_BUFFER_SIZE          (LCD_100ASK_SCREEN_WIDTH * LCD_100ASK_SCREEN_HEIGHT)

//#define LCD_100ASK_SPI_PORT                 1
//#define LCD_100ASK_SCREEN_WIDTH             320
//#define LCD_100ASK_SCREEN_HEIGHT            480
//#define LCD_100ASK_SPI_BUFFER_SIZE          (LCD_100ASK_SCREEN_WIDTH * LCD_100ASK_SCREEN_HEIGHT)

// LCD rotation
#define LCD_100ASK_SPI_ROTATION             CONFIG_100ASK_MODULE_DRIVER_SPI_LCD_ROTATION /*0, 90, 180, 270*/

/* CS: PB9
 * SPI1_MISO: PA6
 * SPI1_MOSI: PA7
 * SPI1_SCK:  PA5
 */
//#define LCD_100ASK_PIN_MOSI               -1
//#define LCD_100ASK_PIN_MISO               -1
//#define LCD_100ASK_PIN_CLK                -1
//#define LCD_100ASK_PIN_CS                 -1
//#define LCD_100ASK_PIN_DC                   GPIOA(19)
//#define LCD_100ASK_PIN_RST                  GPIOA(20)
#define LCD_100ASK_PIN_DC                   GPIOA(4)
#define LCD_100ASK_PIN_RST                  GPIOA(3)

// PWM
#define LCD_100ASK_PIN_PWM                  GPIOA(26)

#define LCD_100ASK_PWM_PORT                 CONFIG_100ASK_MODULE_DRIVER_SPI_LCD_PWM_PORT


// LCD_DISPLAY Commands
#define LCD_DISPLAY_CMD_RAMCTRL             0xb0 // RAM Control
#define LCD_DISPLAY_CMD_CASET               0x2a // Column address set
#define LCD_DISPLAY_CMD_RASET               0x2b // Row address set
#define LCD_DISPLAY_CMD_RAMWR               0x2c // Memory write
#define LCD_DISPLAY_CMDLIST_END             0xff // End command (used for command list)

typedef uint16_t lcd_display_color_t;


static uint16_t lcd_100ask_spi_buffer[LCD_100ASK_SPI_BUFFER_SIZE];
//static uint16_t lcd_100ask_spi_buffer[10];
static uint16_t lcd_dev_xoffset = 0;
static uint16_t lcd_dev_yoffset = 0;

static void lcd_100ask_init(void);
static void lcd_100ask_reset(void);
static void lcd_100ask_send_data(void *buf, size_t length);
//static void lcd_100ask_send_para(uint8_t para);
static void lcd_100ask_send_para(uint8_t *para, uint32_t len);
static void lcd_100ask_send_cmd(uint8_t cmd);

void lcd_100ask_spi_init(void)
{
    hal_spi_master_status_t ret;
    int  port = LCD_100ASK_SPI_PORT;
    //hal_spi_master_port_t  port = 1;
    hal_spi_master_config_t cfg;

	/* init spi */
    cfg.clock_frequency = LCD_100ASK_SPI_FREQUENCY;
    //cfg.clock_frequency = 40000000;
    cfg.chipselect = 0;
    cfg.mode = 0;

    ret = hal_spi_init(port, &cfg);
    if (ret != HAL_SPI_MASTER_OK)
		printf("init spi master failed - %d\n", ret);
	// ret = hal_spi_hw_config(port, &cfg);

    
    /* pin init */
    // DC PIN
    hal_gpio_set_pull(LCD_100ASK_PIN_DC, GPIO_PULL_UP);
    hal_gpio_set_direction(LCD_100ASK_PIN_DC, GPIO_DIRECTION_OUTPUT);
    hal_gpio_pinmux_set_function(LCD_100ASK_PIN_DC, GPIO_MUXSEL_OUT);

    //// RESET PIN
    hal_gpio_set_pull(LCD_100ASK_PIN_RST, GPIO_PULL_UP);
    hal_gpio_set_direction(LCD_100ASK_PIN_RST, GPIO_DIRECTION_OUTPUT);
    hal_gpio_pinmux_set_function(LCD_100ASK_PIN_RST, GPIO_MUXSEL_OUT);

    // PWM PIN
#if 1
    hal_gpio_set_pull(LCD_100ASK_PIN_PWM, GPIO_PULL_UP);
    hal_gpio_set_direction(LCD_100ASK_PIN_PWM, GPIO_DIRECTION_OUTPUT);
    hal_gpio_pinmux_set_function(LCD_100ASK_PIN_PWM, GPIO_MUXSEL_OUT);

    hal_gpio_set_data(LCD_100ASK_PIN_PWM, GPIO_DATA_HIGH);
#else
    ret = hal_pwm_init();
    if(ret)
        printf("hal_pwm_init failed!\n");
#endif
    lcd_100ask_init();
}


void lcd_100ask_spi_set_window(uint16_t start_x, uint16_t start_y, uint16_t end_x, uint16_t end_y)
{
	uint8_t caset[4];
	uint8_t raset[4];

	caset[0] = (uint8_t)((start_x + lcd_dev_xoffset) >> 8) & 0xFF;
	caset[1] = (uint8_t)((start_x + lcd_dev_xoffset) & 0xff);
	caset[2] = (uint8_t)((end_x + lcd_dev_xoffset) >> 8) & 0xFF;
	caset[3] = (uint8_t)((end_x + lcd_dev_xoffset) & 0xff) ;
	
	raset[0] = (uint8_t)((start_y + lcd_dev_yoffset) >> 8) & 0xFF;
	raset[1] = (uint8_t)((start_y + lcd_dev_yoffset) & 0xff);
	raset[2] = (uint8_t)((end_y + lcd_dev_yoffset) >> 8) & 0xFF;
	raset[3] = (uint8_t)((end_y + lcd_dev_yoffset) & 0xff);

	lcd_100ask_send_cmd(LCD_DISPLAY_CMD_CASET);
	lcd_100ask_send_para(caset, 4);

	lcd_100ask_send_cmd(LCD_DISPLAY_CMD_RASET);
	lcd_100ask_send_para(raset, 4);

	lcd_100ask_send_cmd(LCD_DISPLAY_CMD_RAMWR);
}

void lcd_100ask_spi_set_clear(uint16_t color_le)
{
    lcd_100ask_spi_set_window(0, 0, LCD_100ASK_SCREEN_WIDTH, LCD_100ASK_SCREEN_HEIGHT); // We ignore margins here
	
	size_t update_size = LCD_100ASK_SCREEN_WIDTH * LCD_100ASK_SCREEN_HEIGHT;
    uint16_t color_be = (color_le << 8) | (color_le >> 8);

    ///memset(lcd_100ask_spi_buffer, color_be, LCD_100ASK_SPI_BUFFER_SIZE);
    for (size_t i = 0; i < LCD_100ASK_SPI_BUFFER_SIZE; ++i)
        lcd_100ask_spi_buffer[i] = color_be;
    lcd_100ask_send_data(lcd_100ask_spi_buffer, LCD_100ASK_SPI_BUFFER_SIZE * sizeof(lcd_display_color_t));
    //lcd_100ask_send_data(lcd_100ask_spi_buffer, LCD_100ASK_SPI_BUFFER_SIZE);
}


static void lcd_100ask_init(void)
{
	lcd_100ask_reset();
    /*initialize screen*/
	lcd_100ask_send_cmd(0x11);

	lcd_100ask_send_cmd(0x20);

	lcd_100ask_send_cmd(0x36);
	lcd_100ask_send_para((uint8_t *)"\x48", 1);

	lcd_100ask_send_cmd(0x3a);
	lcd_100ask_send_para((uint8_t *)"\x55", 1);

	lcd_100ask_send_cmd(0x13);

	lcd_100ask_send_cmd(0x29);

	lcd_100ask_send_cmd(0x11);
	lcd_100ask_send_para((uint8_t *)"\x00", 1);
	vTaskDelay(120 / portTICK_RATE_MS);
}


static void lcd_100ask_reset(void)
{
    hal_gpio_set_data(LCD_100ASK_PIN_RST, GPIO_DATA_LOW);
    vTaskDelay(20 / portTICK_RATE_MS);
    hal_gpio_set_data(LCD_100ASK_PIN_RST, GPIO_DATA_HIGH);
    vTaskDelay(20 / portTICK_RATE_MS);

}


static void lcd_100ask_send_cmd(uint8_t cmd)
{
    hal_gpio_set_data(LCD_100ASK_PIN_DC, GPIO_DATA_LOW);
    hal_spi_write(LCD_100ASK_SPI_PORT, &cmd, 1);
}

static void lcd_100ask_send_para(uint8_t *para, uint32_t len)
{
	hal_gpio_set_data(LCD_100ASK_PIN_DC, GPIO_DATA_HIGH);
    hal_spi_write(LCD_100ASK_SPI_PORT, para, len);
}

static void lcd_100ask_send_data(void *buf, size_t length)
{
	hal_spi_master_status_t ret;
	hal_spi_master_transfer_t t;

    hal_gpio_set_data(LCD_100ASK_PIN_DC, GPIO_DATA_HIGH);

	memset(&t, 0, sizeof(hal_spi_master_transfer_t));
#if 1
	t.tx_buf = (void *)buf;
	t.tx_len	= length;
	t.tx_single_len	= length;
	t.bits_per_word	= 8;
	t.dummy_byte = 0;
	t.rx_buf = NULL;
	t.rx_len = 0;
	t.tx_nbits = SPI_NBITS_SINGLE;

	ret =  hal_spi_xfer(LCD_100ASK_SPI_PORT, &t, 1);

#else
    hal_spi_write(LCD_100ASK_SPI_PORT, buf, length);
#endif
}



#define LCD_COLOR_BLACK     0x0000
#define LCD_COLOR_WHITE     0xFFFF
#define LCD_COLOR_RED       0xF800
#define LCD_COLOR_GREEN     0x07E0
#define LCD_COLOR_BLUE      0x001F
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
int lcd_100ask_spi_test(int argc, char *argv[])
{
    lcd_100ask_spi_init();

    while (1)
    {
        printf("Color red\n");
        lcd_100ask_spi_set_clear(LCD_COLOR_RED);
        vTaskDelay(1000 / portTICK_RATE_MS);

		printf("Color white\n");
        lcd_100ask_spi_set_clear(LCD_COLOR_WHITE);
        vTaskDelay(1000 / portTICK_RATE_MS);


        printf("Color green\n");
        lcd_100ask_spi_set_clear(LCD_COLOR_GREEN);
        vTaskDelay(1000 / portTICK_RATE_MS);

        printf("Color blue\n");
        lcd_100ask_spi_set_clear(LCD_COLOR_BLUE);
        vTaskDelay(1000 / portTICK_RATE_MS);

        printf("Color black\n");
        lcd_100ask_spi_set_clear(LCD_COLOR_BLACK);
        vTaskDelay(1000 / portTICK_RATE_MS);   
    }

    return 0;
}

FINSH_FUNCTION_EXPORT_CMD(lcd_100ask_spi_test, test_100ask_spi_lcd, module driver 100ask test spi lcd);
