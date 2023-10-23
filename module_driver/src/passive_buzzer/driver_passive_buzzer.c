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

#include <console.h>
#include <unistd.h>
#include <hal_log.h>
#include <hal_time.h>
#include <sunxi_hal_pwm.h>

#include "driver_passive_buzzer.h"

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
#include "driver_lcd.h"
#endif

#define PASSIVE_BUZZER_PORT 	CONFIG_100ASK_MODULE_DRIVER_PASSIVE_BUZZER_PORT

static uint32_t g_pwm_port = PASSIVE_BUZZER_PORT;

/**********************************************************************
 * 函数名称： PassiveBuzzer_Init
 * 功能描述： 无源蜂鸣器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Init(void)
{
    hal_pwm_init();
}


/**********************************************************************
 * 函数名称： PassiveBuzzer_Control
 * 功能描述： 无源蜂鸣器控制函数
 * 输入参数： on - 1-响, 0-不响
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Control(int on)
{
    if (on)
    {
        hal_pwm_enable_controller(g_pwm_port);
    }        
    else
    {
        hal_pwm_disable_controller(g_pwm_port);
    }
}

/**********************************************************************
 * 函数名称： PassiveBuzzer_Set_Freq_Duty
 * 功能描述： 无源蜂鸣器控制函数: 设置频率和占空比
 * 输入参数： freq - 频率, duty - 占空比
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void PassiveBuzzer_Set_Freq_Duty(int freq, int duty)
{
    struct pwm_config config;
    uint8_t port = g_pwm_port;

    config.duty_ns   = (4000000000 / freq - 1) * duty / 100;
    config.period_ns = 4000000000 / freq - 1;
    config.polarity  = PWM_POLARITY_INVERSED;

    hal_pwm_control(port, &config);
}


static void PassiveBuzzer_Set_Freq_Duty_test(int freq, int duty)
{
    struct pwm_config config;
    uint8_t port = g_pwm_port;

    config.duty_ns   = duty;
    config.period_ns = freq;
    config.polarity  = PWM_POLARITY_NORMAL;
    printf("duty_ns = %d,period_ns = %d, polarity = %d", config.duty_ns, config.period_ns, config.polarity);

    hal_pwm_control(port, &config);
}

/**********************************************************************
 * 函数名称： PassiveBuzzer_Test
 * 功能描述： 无源蜂鸣器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
int PassiveBuzzer_Test(int argc, char *argv[])
{
    int freq = 500000, duty = 100000;
    g_pwm_port = PASSIVE_BUZZER_PORT;

    if (argc < 4 || argv[1] == NULL) {
        printf("test_100ask_passive_buzzer <pwm port> <freq> <duty>, \
        Default pwm port: %d, freq:%d duty: %d\n\n", g_pwm_port, freq, duty);
        //return 0;
    }
    else {
        g_pwm_port = atoi(argv[1]);
        freq = atoi(argv[2]);
        duty = atoi(argv[3]);
    }

	PassiveBuzzer_Init();

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
    
	LCD_Init();
	LCD_Clear();

    while (1)
    {
        LCD_PrintString(0, 0, "PassiveBuzzer");
        LCD_PrintString(0, 2, "ON ");
        PassiveBuzzer_Set_Freq_Duty_test(freq, duty);
        PassiveBuzzer_Control(1);
        hal_sleep(1);

        LCD_PrintString(0, 0, "PassiveBuzzer");
        LCD_PrintString(0, 2, "OFF");
        PassiveBuzzer_Control(0);
        hal_sleep(1);
    }
#else

	while (1)
    {
		printf("PassiveBuzzer: ON\n");
		PassiveBuzzer_Set_Freq_Duty_test(freq, duty);
        PassiveBuzzer_Control(1);
        hal_sleep(1);

        printf("PassiveBuzzer: OFF\n");
        PassiveBuzzer_Control(0);
        hal_sleep(1);
    }
#endif

	return 0;

}

FINSH_FUNCTION_EXPORT_CMD(PassiveBuzzer_Test, test_100ask_passive_buzzer, module driver 100ask test passive buzzer);
