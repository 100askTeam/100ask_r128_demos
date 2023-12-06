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
 * 文件名称：driver_servo.c
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
 * 2023.12.6   v0.1         zhouyuebiao   创建文件
 *-----------------------------------------------------
 */

#include <console.h>
#include <unistd.h>
#include <hal_log.h>
#include <hal_time.h>
#include <sunxi_hal_pwm.h>

#include "driver_servo.h"

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
#include "driver_lcd.h"
#endif

#define SERVO_PERIOD_NS (20000000)   /*50HZ*/

#define SERVO_PWM_PORT 	CONFIG_100ASK_MODULE_DRIVER_SERVO_PORT


static uint32_t g_pwm_port = SERVO_PWM_PORT;

/**********************************************************************
 * 函数名称： servo_init
 * 功能描述： 初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void servo_init(void)
{
    hal_pwm_init();
}


/**********************************************************************
 * 函数名称： servo_pwm_state
 * 功能描述： 控制函数
 * 输入参数： on - 1-enable, 0-disable
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void servo_pwm_state(uint8_t port, int on)
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
 * 函数名称： servo_set_angle
 * 功能描述： angle
 * 输入参数： port angle
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void servo_set_angle(uint8_t port, int16_t angle)
{
    if(angle < 0 || angle > 180)    return;

    float t_ms = 0.5 + (float)(2.0 / 180.0) * angle;

    struct pwm_config config;
    config.duty_ns   = t_ms*1000*1000;
    config.period_ns = SERVO_PERIOD_NS;
    config.polarity  = PWM_POLARITY_NORMAL;
    //printf("duty_ns = %d,period_ns = %d, polarity = %d", config.duty_ns, config.period_ns, config.polarity);

    hal_pwm_control(port, &config);
}

/**********************************************************************
 * 函数名称： servo_test
 * 功能描述： 测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
int servo_test(int argc, char *argv[])
{
    int angle = 0;
    g_pwm_port = SERVO_PWM_PORT;

    if (argc < 4 || argv[1] == NULL) {
        printf("test_100ask_servo <pwm port>, \
        Default pwm port: %d\n\n", g_pwm_port);
        //return 0;
    }
    else {
        g_pwm_port = atoi(argv[1]);
    }

	servo_init();
    servo_pwm_state(g_pwm_port, 1);

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
    
	LCD_Init();
	LCD_Clear();

    while (1)
    {
        angle = 0;

        // angle: 0
        LCD_PrintString(0, 0, "Set servo angle");
        LCD_PrintSignedVal(0, 2, angle);
        servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;

        // angle: 45
        LCD_PrintString(0, 0, "Set servo angle");
        LCD_PrintSignedVal(0, 2, angle);
        servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;

        // angle: 90
        LCD_PrintString(0, 0, "Set servo angle");
        LCD_PrintSignedVal(0, 2, angle);
        servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;

        // angle: 135
        LCD_PrintString(0, 0, "Set servo angle");
        LCD_PrintSignedVal(0, 2, angle);
        servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;

        // angle: 180
        LCD_PrintString(0, 0, "Set servo angle");
        LCD_PrintSignedVal(0, 2, angle);
        servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;
    }
#else

	while (1)
    {
        angle = 0;
		
        // angle: 0
        printf("Set servo angle: %d\n", angle);
		servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;

        // angle: 45
        printf("Set servo angle: %d\n", angle);
		servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;

        // angle: 90
        printf("Set servo angle: %d\n", angle);
		servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;

        // angle: 135
        printf("Set servo angle: %d\n", angle);
		servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;

        // angle: 180
        printf("Set servo angle: %d\n", angle);
		servo_set_angle(g_pwm_port, angle);
        hal_mdelay(1000);
        angle += 45;
    }
#endif

    servo_pwm_state(g_pwm_port, 0);

	return 0;

}

FINSH_FUNCTION_EXPORT_CMD(servo_test, test_100ask_servo, module driver 100ask test servo);
