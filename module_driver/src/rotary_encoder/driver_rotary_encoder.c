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
 * 文件名称：driver_color_led.c
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
 * 2023.10.13   v01         zhouyuebiao   创建文件
 *-----------------------------------------------------
 */

#include <unistd.h>
#include <hal_gpio.h>
#include <hal_time.h>
#include <console.h>
#include "driver_rotary_encoder.h"

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
#include "driver_lcd.h"
#endif

/*
 * PA24 - S1
 * PA25  - S2
 * PA29  - Key
 */
#define ROTARY_ENCODER_S1_GPIO_PIN    GPIOA(24)
#define ROTARY_ENCODER_S2_GPIO_PIN    GPIOA(25)
#define ROTARY_ENCODER_KEY_GPIO_PIN   GPIOA(29)

static int32_t g_count = 0;
static int32_t g_speed = 0; /* 速度(正数表示顺时针旋转,负数表示逆时针旋转,单位:每秒转动次数) */

static int encode_counter = 0;
static int encode_current_clk;
static int encode_lask_clk;
static int current_dir = 0;

/**********************************************************************
 * 函数名称： RotaryEncoder_Get_Key
 * 功能描述： 读取旋转编码器Key引脚电平
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1-按键被按下, 0-按键被松开
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int RotaryEncoder_Get_Key(void)
{
    gpio_data_t key_value = GPIO_DATA_LOW;
    
    hal_gpio_get_data(ROTARY_ENCODER_KEY_GPIO_PIN, &key_value);
    if(key_value == GPIO_DATA_LOW)
        return 1;
    else
        return 0;
}

/**********************************************************************
 * 函数名称： RotaryEncoder_Get_S1
 * 功能描述： 读取旋转编码器S1引脚电平
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int RotaryEncoder_Get_S1(void)
{
    gpio_data_t s1_value = GPIO_DATA_LOW;
    
    hal_gpio_get_data(ROTARY_ENCODER_S1_GPIO_PIN, &s1_value);
    if(s1_value == GPIO_DATA_LOW)
        return 0;
    else
        return 1;
}

/**********************************************************************
 * 函数名称： RotaryEncoder_Get_S2
 * 功能描述： 读取旋转编码器S2引脚电平
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int RotaryEncoder_Get_S2(void)
{
    gpio_data_t s2_value = GPIO_DATA_LOW;
    
    hal_gpio_get_data(ROTARY_ENCODER_S2_GPIO_PIN, &s2_value);
    if(s2_value == GPIO_DATA_LOW)
        return 0;
    else
        return 1;
}


/**********************************************************************
 * 函数名称： RotaryEncoder_IRQ_Callback
 * 功能描述： 旋转编码器的中断回调函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static hal_irqreturn_t RotaryEncoder_IRQ_Callback(void *data)
{
    // 获取引脚的高低电平状态
    gpio_data_t s1_value = GPIO_DATA_LOW;
    gpio_data_t s2_value = GPIO_DATA_LOW;
    
    hal_gpio_get_data(ROTARY_ENCODER_S1_GPIO_PIN, &s1_value);
    hal_gpio_get_data(ROTARY_ENCODER_S2_GPIO_PIN, &s2_value);

    // 判断当前数据状态
    encode_current_clk = s2_value;
    if (encode_current_clk != encode_lask_clk && encode_current_clk == 1){
        // 判断正反转
        if (s1_value != encode_current_clk) {
            // 正转
            encode_counter ++;
            current_dir = 1;
        } else if (s1_value == encode_current_clk){
            // 反转
            encode_counter --;
            current_dir = -1;
        }
        //printf("Direction = %d, Counter = %d\n", current_dir, encode_counter);
    }
    // 刷新当前状态
    encode_lask_clk = encode_current_clk;

    return 0;

}



/**********************************************************************
 * 函数名称： RotaryEncoder_Init
 * 功能描述： 旋转编码器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void RotaryEncoder_Init(void)
{
    /* PB0,PB1在MX_GPIO_Init中被配置为输入引脚 */
    /* PB12在MX_GPIO_Init中被配置为中断引脚,上升沿触发 */

    int ret = 0;

    //key
    hal_gpio_set_pull(ROTARY_ENCODER_KEY_GPIO_PIN, GPIO_PULL_UP);
    hal_gpio_set_direction(ROTARY_ENCODER_KEY_GPIO_PIN, GPIO_DIRECTION_INPUT);
    hal_gpio_pinmux_set_function(ROTARY_ENCODER_KEY_GPIO_PIN, GPIO_MUXSEL_IN);

	// A24 -> CLK, A25 -> DT. A29 -> SW
    hal_gpio_set_pull(ROTARY_ENCODER_S1_GPIO_PIN, GPIO_PULL_DOWN_DISABLED);
    hal_gpio_set_direction(ROTARY_ENCODER_S1_GPIO_PIN, GPIO_DIRECTION_INPUT);
    hal_gpio_pinmux_set_function(ROTARY_ENCODER_S1_GPIO_PIN, GPIO_MUXSEL_IN);

    // 获取初始编码器 CLK 状态
    gpio_data_t clk_data;
    hal_gpio_get_data(ROTARY_ENCODER_S1_GPIO_PIN, &clk_data);
    encode_lask_clk = clk_data;

    hal_gpio_set_pull(ROTARY_ENCODER_S2_GPIO_PIN, GPIO_PULL_DOWN_DISABLED);
    hal_gpio_set_direction(ROTARY_ENCODER_S2_GPIO_PIN, GPIO_DIRECTION_INPUT);
    hal_gpio_pinmux_set_function(ROTARY_ENCODER_S2_GPIO_PIN, GPIO_MUXSEL_IN);

    // 存放 CLK，DT 中断号
    uint32_t irq_clk, irq_dt;

    // 申请 ROTARY_ENCODER_S1_GPIO_PIN 为中断引脚，跳变触发
    ret = hal_gpio_to_irq(ROTARY_ENCODER_S1_GPIO_PIN, &irq_clk);
    if (ret < 0){
        printf("gpio to irq error, irq num:%d error num: %d\n", irq_clk, ret);
    }
    // 绑定中断处理函数
    ret = hal_gpio_irq_request(irq_clk, RotaryEncoder_IRQ_Callback, IRQ_TYPE_EDGE_BOTH, NULL);
    if (ret < 0){
        printf("request irq error, irq num:%d error num: %d\n", irq_clk, ret);
    }
    // 启用中断
    ret = hal_gpio_irq_enable(irq_clk);
    if (ret < 0){
        printf("request irq error, error num: %d\n", ret);
    }

    // 申请 ROTARY_ENCODER_S2_GPIO_PIN 为中断引脚，跳变触发
    ret = hal_gpio_to_irq(ROTARY_ENCODER_S2_GPIO_PIN, &irq_dt);
    if (ret < 0){
        printf("gpio to irq error, irq num:%d error num: %d\n", irq_dt, ret);
    }
    // 绑定中断处理函数
    ret = hal_gpio_irq_request(irq_dt, RotaryEncoder_IRQ_Callback, IRQ_TYPE_EDGE_BOTH, NULL);
    if (ret < 0){
        printf("request irq error, irq num:%d error num: %d\n", irq_dt, ret);
    }
    // 启用中断
    ret = hal_gpio_irq_enable(irq_dt);
    if (ret < 0){
        printf("request irq error, error num: %d\n", ret);
    }
}

/**********************************************************************
 * 函数名称： RotaryEncoder_Write
 * 功能描述： 旋转编码器的读取函数
 * 输入参数： 无
 * 输出参数： pCnt   - 用于保存计数值 
 *            pSpeed - 用于保存速度(正数表示顺时针旋转,负数表示逆时针旋转,单位:每秒转动次数)
 *            pKey   - 用于保存按键状态(1-按键被按下, 0-按键被松开)
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void RotaryEncoder_Read(int32_t *pCnt, int32_t *pSpeed, int32_t *pKey)
{
    static int32_t pre_count = 0;
    *pCnt = encode_counter;

    if (encode_counter == pre_count)
        *pSpeed = 0;
    else
        *pSpeed = current_dir;

    pre_count = encode_counter;
    
    *pKey = RotaryEncoder_Get_Key();

}


/**********************************************************************
 * 函数名称： RotaryEncoder_Test
 * 功能描述： 旋转编码器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/05        V1.0     韦东山       创建
 ***********************************************************************/
int RotaryEncoder_Test(int argc, char *argv[])
{
    int32_t count, speed, key;
    int len;
    
    RotaryEncoder_Init();

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
    LCD_Init();
	LCD_Clear();

    while (1)
    {
        LCD_PrintString(0, 0, "RotaryEncoder: ");

        RotaryEncoder_Read(&count, &speed, &key);

        len = LCD_PrintString(0, 2, "Cnt  : ");
        len += LCD_PrintSignedVal(len, 2, count);
        LCD_ClearLine(len, 2);
        
        len = LCD_PrintString(0, 4, "Speed: ");
        len += LCD_PrintSignedVal(len, 4, speed);
        LCD_ClearLine(len, 4);
        
        len = LCD_PrintString(0, 6, "Key  : ");
        len += LCD_PrintString(len, 6, key ? "Pressed":"Released");
        LCD_ClearLine(len, 6);
    }
#else
    while (1)
    {
        RotaryEncoder_Read(&count, &speed, &key);
        printf("Cnt:%d, Speed:%d, Key: %s\n", count, speed, (key ? "Pressed":"Released"));
        sleep(1);
    }
#endif

    return 0;
}

FINSH_FUNCTION_EXPORT_CMD(RotaryEncoder_Test, test_100ask_rotary_encoder, module driver 100ask test rotary encoder);