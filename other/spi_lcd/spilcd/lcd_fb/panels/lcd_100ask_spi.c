/*
 * R128-FreeRTOS/lichee/rtos-hal/hal/source/spilcd/lcd_fb/panels/lcd_100ask_spi.c
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

#include "lcd_100ask_spi.h"

#define LCD_RESET_PIN		GPIOA(5)

// LCD_DISPLAY Commands
#define LCD_DISPLAY_CMD_RAMCTRL           0xb0 // RAM Control
#define LCD_DISPLAY_CMD_CASET             0x2a // Column address set
#define LCD_DISPLAY_CMD_RASET             0x2b // Row address set
#define LCD_DISPLAY_CMD_RAMWR             0x2c // Memory write
#define LCD_DISPLAY_CMDLIST_END           0xff // End command (used for command list)

static void LCD_power_on(u32 sel);
static void LCD_power_off(u32 sel);
static void LCD_bl_open(u32 sel);
static void LCD_bl_close(u32 sel);
static void LCD_panel_init(u32 sel);
static void LCD_panel_exit(u32 sel);

static void lcd_init_240x240(unsigned int sel, lcd_display_rotation_t rotation);
static void lcd_init_320x480(unsigned int sel, lcd_display_rotation_t rotation);

static uint16_t lcd_dev_xoffset = 0;
static uint16_t lcd_dev_yoffset = 0;
static struct disp_panel_para info[LCD_FB_MAX];

static void address(unsigned int sel, int x, int y, int width, int height)
{
	//lcd_fb_inf("---------------set lcd window-------------------\n");
	sunxi_lcd_cmd_write(sel, LCD_DISPLAY_CMD_CASET); /* Set coloum address */
	sunxi_lcd_para_write(sel, (((x + lcd_dev_xoffset) >> 8) & 0xff));
	sunxi_lcd_para_write(sel, ((x + lcd_dev_xoffset) & 0xff));
	sunxi_lcd_para_write(sel, ((width + lcd_dev_xoffset >> 8) & 0xff));
	sunxi_lcd_para_write(sel, ((width + lcd_dev_xoffset) & 0xff));

	sunxi_lcd_cmd_write(sel, LCD_DISPLAY_CMD_RASET); /* Set row address */
	sunxi_lcd_para_write(sel, ((y + lcd_dev_yoffset >> 8) & 0xff));
	sunxi_lcd_para_write(sel, ((y + lcd_dev_yoffset) & 0xff));
	sunxi_lcd_para_write(sel, ((height + lcd_dev_yoffset >> 8) & 0xff));
	sunxi_lcd_para_write(sel, ((height + lcd_dev_yoffset) & 0xff));

	sunxi_lcd_cmd_write(sel, LCD_DISPLAY_CMD_RAMWR);
}

static void LCD_panel_init(unsigned int sel)
{
	hal_gpio_set_pull(LCD_RESET_PIN, GPIO_PULL_UP);
    hal_gpio_set_direction(LCD_RESET_PIN, GPIO_DIRECTION_OUTPUT);
	hal_gpio_pinmux_set_function(LCD_RESET_PIN,GPIO_MUXSEL_OUT);
    hal_gpio_set_data(LCD_RESET_PIN, GPIO_DATA_LOW);
	sunxi_lcd_delay_ms(120);
	hal_gpio_set_data(LCD_RESET_PIN, GPIO_DATA_HIGH);
	sunxi_lcd_delay_ms(120);

	if (bsp_disp_get_panel_info(sel, &info[sel])) {
		lcd_fb_wrn("get panel info fail!\n");
		hal_log_info("lget panel info fail!\n");
		return;
	}

#ifdef CONFIG_USE_100ASK_DISPLAY_SCREEN_240X240
	lcd_init_240x240(sel, (lcd_display_rotation_t)(CONFIG_DISPLAY_SCREEN_100ASK_ROTATION / 90));
#elif CONFIG_USE_100ASK_DISPLAY_SCREEN_320X480
	lcd_init_320x480(sel, (lcd_display_rotation_t)(CONFIG_DISPLAY_SCREEN_100ASK_ROTATION / 90));
#else
	#error "Display screen init sequence is not defined for this device!"
#endif

#if ((CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 0) || (CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 180))
    address(sel, 0, 0, info[sel].lcd_x - 1, info[sel].lcd_y - 1);
#elif ((CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 90) || (CONFIG_DISPLAY_SCREEN_100ASK_ROTATION == 270))
    address(sel, 0, 0, info[sel].lcd_y - 1, info[sel].lcd_x - 1);
#else
    daddress(sel, 0, 0, info[sel].lcd_x - 1, info[sel].lcd_y - 1);
#endif

	hal_log_info("LCD_panel_init ok!\n");
}

static void LCD_panel_exit(unsigned int sel)
{
	sunxi_lcd_cmd_write(sel, 0x28);
	sunxi_lcd_delay_ms(20);
	sunxi_lcd_cmd_write(sel, 0x10);
	sunxi_lcd_delay_ms(20);
	sunxi_lcd_pin_cfg(sel, 0);
}

static s32 LCD_open_flow(u32 sel)
{
	lcd_fb_here;
	/* open lcd power, and delay 50ms */
	LCD_OPEN_FUNC(sel, LCD_power_on, 50);
	/* open lcd power, than delay 200ms */
	LCD_OPEN_FUNC(sel, LCD_panel_init, 200);

	LCD_OPEN_FUNC(sel, lcd_fb_black_screen, 50);
	/* open lcd backlight, and delay 0ms */
	LCD_OPEN_FUNC(sel, LCD_bl_open, 0);

	return 0;
}

static s32 LCD_close_flow(u32 sel)
{
	lcd_fb_here;
	/* close lcd backlight, and delay 0ms */
	LCD_CLOSE_FUNC(sel, LCD_bl_close, 50);
	/* open lcd power, than delay 200ms */
	LCD_CLOSE_FUNC(sel, LCD_panel_exit, 10);
	/* close lcd power, and delay 500ms */
	LCD_CLOSE_FUNC(sel, LCD_power_off, 10);

	return 0;
}

static void LCD_power_on(u32 sel)
{
	/* config lcd_power pin to open lcd power0 */
	lcd_fb_here;
	sunxi_lcd_power_enable(sel, 0);
	sunxi_lcd_pin_cfg(sel, 1);
}

static void LCD_power_off(u32 sel)
{
	lcd_fb_here;
	/* config lcd_power pin to close lcd power0 */
	sunxi_lcd_power_disable(sel, 0);
}

static void LCD_bl_open(u32 sel)
{
	sunxi_lcd_pwm_enable(sel);
	/* config lcd_bl_en pin to open lcd backlight */
	sunxi_lcd_backlight_enable(sel);
	lcd_fb_here;
}

static void LCD_bl_close(u32 sel)
{
	/* config lcd_bl_en pin to close lcd backlight */
	sunxi_lcd_backlight_disable(sel);
	sunxi_lcd_pwm_disable(sel);
	lcd_fb_here;
}

static int lcd_set_var(unsigned int sel, struct fb_info *p_info)
{
	return 0;
}

static int lcd_blank(unsigned int sel, unsigned int en)
{
#if 0
	if (en)
		sunxi_lcd_cmd_write(sel, 0x28);
	else
		sunxi_lcd_cmd_write(sel, 0x29);

	sunxi_lcd_cmd_write(sel, 0x2c); /* Display ON */
#endif

	return 0;
}

static int lcd_set_addr_win(unsigned int sel, int x, int y, int width, int height)
{
	address(sel, x, y, width, height);
	//sunxi_lcd_cmd_write(sel, 0x2c); /* Display ON */
	return 0;
}

struct __lcd_panel lcd_100ask_spi_panel = {
    /* panel driver name, must mach the name of lcd_drv_name in sys_config.fex
       */
	.name = "lcd_100ask_spi",
	.func = {
		.cfg_open_flow = LCD_open_flow,
		.cfg_close_flow = LCD_close_flow,
		.blank = lcd_blank,
		.set_var = lcd_set_var,
		.set_addr_win = lcd_set_addr_win,
		},
};


static void lcd_init_240x240(unsigned int sel, lcd_display_rotation_t rotation)
{
	hal_log_info("lcd_init_240x240 init rotation:%d!\n", rotation);

	sunxi_lcd_cmd_write(sel, 0x36);
	sunxi_lcd_para_write(sel, 0x00);
	sunxi_lcd_delay_ms(120);

	sunxi_lcd_cmd_write(sel, 0x3a);
	sunxi_lcd_para_write(sel, 0x05);

	sunxi_lcd_cmd_write(sel, 0xb2);
	sunxi_lcd_para_write(sel, 0x0c);
	sunxi_lcd_para_write(sel, 0x0c);
	sunxi_lcd_para_write(sel, 0x00);
	sunxi_lcd_para_write(sel, 0x33);
	sunxi_lcd_para_write(sel, 0x33);

	sunxi_lcd_cmd_write(sel, 0xb7);
	sunxi_lcd_para_write(sel, 0x35);

	sunxi_lcd_cmd_write(sel, 0xbb);
	sunxi_lcd_para_write(sel, 0x19);
	sunxi_lcd_cmd_write(sel, 0xc0);
	sunxi_lcd_para_write(sel, 0x2c);
	sunxi_lcd_cmd_write(sel, 0xc2);
	sunxi_lcd_para_write(sel, 0x01);
	sunxi_lcd_cmd_write(sel, 0xc3);
	sunxi_lcd_para_write(sel, 0x12);
	sunxi_lcd_cmd_write(sel, 0xc4);
	sunxi_lcd_para_write(sel, 0x20);
	sunxi_lcd_cmd_write(sel, 0xc6);
	sunxi_lcd_para_write(sel, 0x0f);
	sunxi_lcd_cmd_write(sel, 0xd0);
	sunxi_lcd_para_write(sel, 0xa4);
	sunxi_lcd_para_write(sel, 0xa1);

	sunxi_lcd_cmd_write(sel, 0xe0);
	sunxi_lcd_para_write(sel, 0xd0);
	sunxi_lcd_para_write(sel, 0x04);
	sunxi_lcd_para_write(sel, 0x0d);
	sunxi_lcd_para_write(sel, 0x11);
	sunxi_lcd_para_write(sel, 0x13);
	sunxi_lcd_para_write(sel, 0x2b);
	sunxi_lcd_para_write(sel, 0x3f);
	sunxi_lcd_para_write(sel, 0x54);
	sunxi_lcd_para_write(sel, 0x4c);
	sunxi_lcd_para_write(sel, 0x18);
	sunxi_lcd_para_write(sel, 0x0d);
	sunxi_lcd_para_write(sel, 0x0b);
	sunxi_lcd_para_write(sel, 0x1f);
	sunxi_lcd_para_write(sel, 0x23);

	sunxi_lcd_cmd_write(sel, 0xE1);
	sunxi_lcd_para_write(sel, 0xD0);
	sunxi_lcd_para_write(sel, 0x04);
	sunxi_lcd_para_write(sel, 0x0c);
	sunxi_lcd_para_write(sel, 0x11);
	sunxi_lcd_para_write(sel, 0x13);
	sunxi_lcd_para_write(sel, 0x2c);
	sunxi_lcd_para_write(sel, 0x3f);
	sunxi_lcd_para_write(sel, 0x44);
	sunxi_lcd_para_write(sel, 0x51);
	sunxi_lcd_para_write(sel, 0x2f);
	sunxi_lcd_para_write(sel, 0x1f);
	sunxi_lcd_para_write(sel, 0x1f);
	sunxi_lcd_para_write(sel, 0x20);
	sunxi_lcd_para_write(sel, 0x23);

	sunxi_lcd_cmd_write(sel, 0x21); /* Sleep Out */
	sunxi_lcd_cmd_write(sel, 0x11); /* Display ON */
	sunxi_lcd_delay_ms(120);
	sunxi_lcd_cmd_write(sel, 0x29); /* Display ON */

	switch (rotation)
	{
		case LCD_DISPLAY_ROTATION_0:
			lcd_dev_xoffset = 0;
			lcd_dev_yoffset = 0;
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x00);
			break;
		case LCD_DISPLAY_ROTATION_90:
			lcd_dev_xoffset = 0;
			lcd_dev_yoffset = 0;
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x60);
			break;
		case LCD_DISPLAY_ROTATION_180:
			lcd_dev_xoffset = 0;
			lcd_dev_yoffset = 80;
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0xc0);
			break;
		case LCD_DISPLAY_ROTATION_270:
			lcd_dev_xoffset = 80;
			lcd_dev_yoffset = 0;
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0xa0);
			break;
		default:
			lcd_dev_xoffset = 0;
			lcd_dev_yoffset = 0;
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x00);
			break;
	}
}

static void lcd_init_320x480(unsigned int sel, lcd_display_rotation_t rotation)
{
	hal_log_info("lcd_init_320x480 init rotation:%d!\n", rotation);
# if 1
	// Positive Gamma Control
    sunxi_lcd_cmd_write(sel, 0xe0);
    sunxi_lcd_para_write(sel, 0xf0);
    sunxi_lcd_para_write(sel, 0x3e);
    sunxi_lcd_para_write(sel, 0x30);
    sunxi_lcd_para_write(sel, 0x06);
    sunxi_lcd_para_write(sel, 0x0a);
    sunxi_lcd_para_write(sel, 0x03);
    sunxi_lcd_para_write(sel, 0x4d);
    sunxi_lcd_para_write(sel, 0x56);
    sunxi_lcd_para_write(sel, 0x3a);
    sunxi_lcd_para_write(sel, 0x06);
    sunxi_lcd_para_write(sel, 0x0f);
    sunxi_lcd_para_write(sel, 0x04);
    sunxi_lcd_para_write(sel, 0x18);
    sunxi_lcd_para_write(sel, 0x13);
    sunxi_lcd_para_write(sel, 0x00);

    // Negative Gamma Control
    sunxi_lcd_cmd_write(sel, 0xe1);
    sunxi_lcd_para_write(sel, 0x0f);
    sunxi_lcd_para_write(sel, 0x37);
    sunxi_lcd_para_write(sel, 0x31);
    sunxi_lcd_para_write(sel, 0x0b);
    sunxi_lcd_para_write(sel, 0x0d);
    sunxi_lcd_para_write(sel, 0x06);
    sunxi_lcd_para_write(sel, 0x4d);
    sunxi_lcd_para_write(sel, 0x34);
    sunxi_lcd_para_write(sel, 0x38);
    sunxi_lcd_para_write(sel, 0x06);
    sunxi_lcd_para_write(sel, 0x11);
    sunxi_lcd_para_write(sel, 0x01);
    sunxi_lcd_para_write(sel, 0x18);
    sunxi_lcd_para_write(sel, 0x13);
    sunxi_lcd_para_write(sel, 0x00);
    
    // Power Control 1
    sunxi_lcd_cmd_write(sel, 0xc0);
    sunxi_lcd_para_write(sel, 0x18);
    sunxi_lcd_para_write(sel, 0x17);

    // Power Control 2
    sunxi_lcd_cmd_write(sel, 0xc1);
    sunxi_lcd_para_write(sel, 0x41);

    // Power Control 3
    sunxi_lcd_cmd_write(sel, 0xc5);
    sunxi_lcd_para_write(sel, 0x00);

    // VCOM Control
    sunxi_lcd_cmd_write(sel, 0x1a);
    sunxi_lcd_para_write(sel, 0x80);

    // Memory Access Control
    sunxi_lcd_cmd_write(sel, 0x36);
    sunxi_lcd_para_write(sel, 0x48);

    // Pixel Interface Format
    sunxi_lcd_cmd_write(sel, 0x3a);
    sunxi_lcd_para_write(sel, 0x55);

    // Interface Mode Control
    sunxi_lcd_cmd_write(sel, 0xb0);
    sunxi_lcd_para_write(sel, 0x00);

    // Frame Rate Control
    sunxi_lcd_cmd_write(sel, 0xb1);
    sunxi_lcd_para_write(sel, 0xa0);

    // Display Inversion Control
    sunxi_lcd_cmd_write(sel, 0xb4);
    sunxi_lcd_para_write(sel, 0x02);

    // Display Function Control
    sunxi_lcd_cmd_write(sel, 0xb6);
    sunxi_lcd_para_write(sel, 0x02);
    sunxi_lcd_para_write(sel, 0x02);

    // Set image function
    sunxi_lcd_cmd_write(sel, 0xe9);
    sunxi_lcd_para_write(sel, 0x00);

    //Adjust Control 3
    sunxi_lcd_cmd_write(sel, 0xf7);
    sunxi_lcd_para_write(sel, 0xa9);
    sunxi_lcd_para_write(sel, 0x51);
    sunxi_lcd_para_write(sel, 0x2c);
    sunxi_lcd_para_write(sel, 0x82);

    // Write_memory_start
    sunxi_lcd_cmd_write(sel, 0x21);
    sunxi_lcd_delay_ms(120);
    //Exit Sleep
    sunxi_lcd_cmd_write(sel, 0x11);
    sunxi_lcd_delay_ms(120);
    
    // PROTATION 90
    sunxi_lcd_cmd_write(sel, 0x36);
    sunxi_lcd_para_write(sel, 0xe8);

	lcd_dev_xoffset = 0;
	lcd_dev_yoffset = 0;
	switch (rotation)
	{
		case LCD_DISPLAY_ROTATION_0:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x48);
			break;
		case LCD_DISPLAY_ROTATION_90:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0xe8);
			break;
		case LCD_DISPLAY_ROTATION_180:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x88);
			break;
		case LCD_DISPLAY_ROTATION_270:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x28);
			break;
		default:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x48);
			break;
	}

	// set_screen_size
    sunxi_lcd_cmd_write(sel, 0x2a);
    sunxi_lcd_para_write(sel, 0x00);
    sunxi_lcd_para_write(sel, 0x00);
    sunxi_lcd_para_write(sel, 0x01);
    sunxi_lcd_para_write(sel, 0x3f);

    sunxi_lcd_cmd_write(sel, 0x2b);
    sunxi_lcd_para_write(sel, 0x00);
    sunxi_lcd_para_write(sel, 0x00);
    sunxi_lcd_para_write(sel, 0x01);
    sunxi_lcd_para_write(sel, 0xdf);

	//Display on
    sunxi_lcd_cmd_write(sel, 0x29);
    sunxi_lcd_delay_ms(120);

#else
	sunxi_lcd_cmd_write(sel, 0x11);
	sunxi_lcd_para_write(sel, 0x00);
	sunxi_lcd_delay_ms(120);

	sunxi_lcd_cmd_write(sel, 0xf0);
	sunxi_lcd_para_write(sel, 0xc3);

	sunxi_lcd_cmd_write(sel, 0xf0);
	sunxi_lcd_para_write(sel, 0x96);

	sunxi_lcd_cmd_write(sel, 0x36);
	sunxi_lcd_para_write(sel, 0x48);

	sunxi_lcd_cmd_write(sel, 0xb4);
	sunxi_lcd_para_write(sel, 0x01);

	sunxi_lcd_cmd_write(sel, 0xb7);
	sunxi_lcd_para_write(sel, 0xc6);

	sunxi_lcd_cmd_write(sel, 0xe8);
	sunxi_lcd_para_write(sel, 0x40);
	sunxi_lcd_cmd_write(sel, 0x8a);
	sunxi_lcd_para_write(sel, 0x00);
	sunxi_lcd_cmd_write(sel, 0x00);
	sunxi_lcd_para_write(sel, 0x29);
	sunxi_lcd_cmd_write(sel, 0x19);
	sunxi_lcd_para_write(sel, 0xa5);
	sunxi_lcd_cmd_write(sel, 0x33);

	sunxi_lcd_cmd_write(sel, 0xc1);
	sunxi_lcd_para_write(sel, 0x06);

	sunxi_lcd_cmd_write(sel, 0xc2);
	sunxi_lcd_para_write(sel, 0xa7);

	sunxi_lcd_cmd_write(sel, 0xc5);
	sunxi_lcd_para_write(sel, 0x18);

	sunxi_lcd_cmd_write(sel, 0xe0);
	sunxi_lcd_para_write(sel, 0xf0);
	sunxi_lcd_para_write(sel, 0x09);
	sunxi_lcd_para_write(sel, 0x0b);
	sunxi_lcd_para_write(sel, 0x06);
	sunxi_lcd_para_write(sel, 0x04);
	sunxi_lcd_para_write(sel, 0x15);
	sunxi_lcd_para_write(sel, 0x2f);
	sunxi_lcd_para_write(sel, 0x54);
	sunxi_lcd_para_write(sel, 0x42);
	sunxi_lcd_para_write(sel, 0x3c);
	sunxi_lcd_para_write(sel, 0x17);
	sunxi_lcd_para_write(sel, 0x14);
	sunxi_lcd_para_write(sel, 0x18);
	sunxi_lcd_para_write(sel, 0x1b);

	sunxi_lcd_cmd_write(sel, 0xE1);
	sunxi_lcd_para_write(sel, 0xf0);
	sunxi_lcd_para_write(sel, 0x09);
	sunxi_lcd_para_write(sel, 0x0b);
	sunxi_lcd_para_write(sel, 0x06);
	sunxi_lcd_para_write(sel, 0x04);
	sunxi_lcd_para_write(sel, 0x03);
	sunxi_lcd_para_write(sel, 0x2d);
	sunxi_lcd_para_write(sel, 0x43);
	sunxi_lcd_para_write(sel, 0x42);
	sunxi_lcd_para_write(sel, 0x3b);
	sunxi_lcd_para_write(sel, 0x16);
	sunxi_lcd_para_write(sel, 0x14);
	sunxi_lcd_para_write(sel, 0x17);
	sunxi_lcd_para_write(sel, 0x1b);

	sunxi_lcd_cmd_write(sel, 0xf0);
	sunxi_lcd_para_write(sel, 0x3c);

	sunxi_lcd_cmd_write(sel, 0xf0);
	sunxi_lcd_para_write(sel, 0x69);

	sunxi_lcd_cmd_write(sel, 0x3a);
	sunxi_lcd_para_write(sel, 0x55);
	sunxi_lcd_delay_ms(120);
	sunxi_lcd_cmd_write(sel, 0x29); /* Display ON */
	
	lcd_dev_xoffset = 0;
	lcd_dev_yoffset = 0;
	switch (rotation)
	{
		case LCD_DISPLAY_ROTATION_0:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x48);
			break;
		case LCD_DISPLAY_ROTATION_90:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0xe8);
			break;
		case LCD_DISPLAY_ROTATION_180:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x88);
			break;
		case LCD_DISPLAY_ROTATION_270:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x28);
			break;
		default:
			sunxi_lcd_cmd_write(sel, 0x36);
			sunxi_lcd_para_write(sel, 0x48);
			break;
	}

	// set_screen_size
	sunxi_lcd_cmd_write(sel, 0x2a);
	sunxi_lcd_para_write(sel, 0x00);
	sunxi_lcd_para_write(sel, 0x00);
	sunxi_lcd_para_write(sel, 0x01);
	sunxi_lcd_para_write(sel, 0x3f);
	sunxi_lcd_cmd_write(sel, 0x2b);
	sunxi_lcd_para_write(sel, 0x00);
	sunxi_lcd_para_write(sel, 0x00);
	sunxi_lcd_para_write(sel, 0x01);
	sunxi_lcd_para_write(sel, 0xdf);
#endif

}

