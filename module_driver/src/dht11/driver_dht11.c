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
 * 文件名称： driver_dht11.c
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
 * 2023.10.20      v01      zhouyuebiao   创建文件
 *-----------------------------------------------------
 */

#include <unistd.h>
#include <hal_gpio.h>
#include <hal_time.h>
#include <console.h>

#include "driver_dht11.h"

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
#include "driver_lcd.h"
#endif

#define DHT11_PIN   	GPIOA(6)

static gpio_pin_t  dht11_pin = DHT11_PIN;

/* 控制GPIO读取DHT11的数据 
 * 1. 主机发出至少18MS的低脉冲: start信号
 * 2. start信号变为高, 20-40us之后, dht11会拉低总线维持80us
      然后拉高80us: 回应信号
 * 3. 之后就是数据, 逐位发送
 *    bit0 : 50us低脉冲, 26-28us高脉冲
 *    bit1 : 50us低脉冲, 70us高脉冲
 * 4. 数据有40bit: 8bit湿度整数数据+8bit湿度小数数据
                   +8bit温度整数数据+8bit温度小数数据
                   +8bit校验和
 */


/* 先实现GPIO的基本操作 */
/**********************************************************************
 * 函数名称： DHT11_PinCfgAsOutput
 * 功能描述： 把DHT11的数据引脚配置为输出
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DHT11_PinCfgAsOutput(void)
{
    /* 把DHT11的引脚配置为"open drain, pull-up" */
	hal_gpio_set_pull(dht11_pin, GPIO_PULL_UP);
    hal_gpio_set_direction(dht11_pin, GPIO_DIRECTION_OUTPUT);
    hal_gpio_pinmux_set_function(dht11_pin, GPIO_MUXSEL_OUT);
	///hal_gpio_set_driving_level(dht11_pin, GPIO_DRIVING_LEVEL3);

	//hal_gpio_set_data(dht11_pin, GPIO_DATA_LOW);
}

/**********************************************************************
 * 函数名称： DHT11_PinCfgAsInput
 * 功能描述： 把DHT11的数据引脚配置为输入
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DHT11_PinCfgAsInput(void)
{
    /* 把DHT11的引脚配置为"open drain, pull-up" 
	 * 让它输出1就不会驱动这个引脚, 并且可以读入引脚状态
     */
	hal_gpio_set_pull(dht11_pin, GPIO_PULL_UP);
    hal_gpio_set_direction(dht11_pin, GPIO_DIRECTION_INPUT);
    hal_gpio_pinmux_set_function(dht11_pin, GPIO_MUXSEL_IN);
	//hal_gpio_set_driving_level(dht11_pin, GPIO_DRIVING_LEVEL3);

    //hal_gpio_set_data(dht11_pin, GPIO_DATA_HIGH);
}

/**********************************************************************
 * 函数名称： DHT11_PinSet
 * 功能描述： 设置DHT11的数据引脚的输出值
 * 输入参数： val - 输出电平
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DHT11_PinSet(int val)
{
	if (val)
		hal_gpio_set_data(dht11_pin, GPIO_DATA_HIGH);
	else
		hal_gpio_set_data(dht11_pin, GPIO_DATA_LOW);
}

/**********************************************************************
 * 函数名称： DHT11_PinRead
 * 功能描述： 读取DHT11的数据引脚
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1-高电平, 0-低电平
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DHT11_PinRead(void)
{
	gpio_data_t key_value = GPIO_DATA_LOW;

	
	hal_gpio_get_data(dht11_pin, &key_value);

    if (GPIO_DATA_HIGH == key_value)
		return 1;
	else
		return 0;
}


/* 再来实现DHT11的读操作 */
/**********************************************************************
 * 函数名称： DHT11_Start
 * 功能描述： 给DHT11发出启动信号 
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void DHT11_Start(void)
{
	//DHT11_PinCfgAsOutput();
	DHT11_PinSet(0);
	hal_udelay(20000);
	DHT11_PinSet(1);
	hal_udelay(30);
	DHT11_PinCfgAsInput();
}


/**********************************************************************
 * 函数名称： DHT11_Wait_Ack
 * 功能描述： 等待DHT11的回应信号
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1-无响应, 0-有响应
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DHT11_Wait_Ack(void)
{
	hal_udelay(30);
	return DHT11_PinRead();
}

/**********************************************************************
 * 函数名称： DHT11_WaitFor_Val
 * 功能描述： 在指定时间内等待数据引脚变为某个值
 * 输入参数： val - 期待数据引脚变为这个值
 *            timeout_us - 超时时间(单位us)
 * 输出参数： 无
 * 返 回 值： 0-成功, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DHT11_WaitFor_Val(int val, int timeout_us)
{
	while (timeout_us--)
	{
		if (DHT11_PinRead() == val)
			return 0; /* ok */
		hal_udelay(1);
	}
	return -1; /* err */
}

/**********************************************************************
 * 函数名称： DHT11_ReadByte
 * 功能描述： 读取DH11 1byte数据
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 数据
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int DHT11_ReadByte(void)
{
	int i;
	int data = 0;

	for (i = 0; i < 8; i++)
	{
		if (DHT11_WaitFor_Val(1, 1000))
		{
			printf("dht11 wait for high data err!\n");
			return -1;
		}
		hal_udelay(40);
		data <<= 1;
		if (DHT11_PinRead() == 1)
			data |= 1;
		
		if (DHT11_WaitFor_Val(0, 1000))
		{
			printf("dht11 wait for low data err!\n");
			return -1;
		}
	}
	
	return data;
}



/* 公开的函数 */

/**********************************************************************
 * 函数名称： DHT11_Init
 * 功能描述： DHT11的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void DHT11_Init(void)
{
	DHT11_PinCfgAsOutput();
	DHT11_PinSet(1);
}


/**********************************************************************
 * 函数名称： DHT11_Read
 * 功能描述： 读取DHT11的温度/湿度
 * 输入参数： 无
 * 输出参数： hum  - 用于保存湿度值
 *            temp - 用于保存温度值
 * 返 回 值： 0 - 成功, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int DHT11_Read(int *hum, int *temp)
{
	unsigned char hum_m, hum_n;
	unsigned char temp_m, temp_n;
	unsigned char check;	
	
	DHT11_Start();
	
	if (0 != DHT11_WaitFor_Val(0, 1000))
	{
		printf("dht11 not ack, err!\n");
		return -1;
	}

	if (0 != DHT11_WaitFor_Val(1, 1000))  /* 等待ACK变为高电平, 超时时间是1000us */
	{
		printf("dht11 wait for ack high err!\n");
		return -1;
	}

	if (0 != DHT11_WaitFor_Val(0, 1000))  /* 数据阶段: 等待低电平, 超时时间是1000us */
	{
		printf("dht11 wait for data low err!\n");
		return -1;
	}

	hum_m  = DHT11_ReadByte();
	hum_n  = DHT11_ReadByte();
	temp_m = DHT11_ReadByte();
	temp_n = DHT11_ReadByte();
	check  = DHT11_ReadByte();

	DHT11_PinCfgAsOutput();
	DHT11_PinSet(1);

	if (hum_m + hum_n + temp_m + temp_n == check)
	{
		*hum  = hum_m;
		*temp = temp_m;
		return 0;
	}
	else
	{
		printf("dht11 checksum err!\n");
		return -1;
	}

}


/**********************************************************************
 * 函数名称： DHT11_Test
 * 功能描述： DHT11测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
int DHT11_Test(int argc, char *argv[])
{
	int hum, temp;
    int len;

	if (argv[1] == NULL) {
        printf("test_100ask_dht11 <GPIO Pin Number>, Default: PA(6)\n");
		dht11_pin = DHT11_PIN;
        //return 0;
    }
	else {
		dht11_pin = atoi(argv[1]);
	}

	DHT11_Init();

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
    
	LCD_Init();
	LCD_Clear();

	while (1)
	{
		if (DHT11_Read(&hum, &temp) != 0 )
		{
			printf("\ndht11 read err!\n");
			DHT11_Init();
		}
		else
		{
			LCD_PrintString(0, 0, "DHT11:");
            LCD_PrintString(0, 2, "Humidity Temp");
            len = LCD_PrintSignedVal(0, 4, hum);
            LCD_PutChar(len, 4, '%');
            LCD_PrintSignedVal(9, 4, temp);
		}
		
		hal_udelay(1000000);	/* 读取周期是不能读太频繁 */
	}
#else

	while (1)
	{
		if (DHT11_Read(&hum, &temp) != 0 )
		{
			printf("\ndht11 read err!\n");
			DHT11_Init();
		}
		
		else
		{
			printf("Humidity: %d, Temp: %d\n", hum, temp);
		}

		hal_udelay(1000000);	/* 读取周期是不能读太频繁 */
	}

#endif

}


FINSH_FUNCTION_EXPORT_CMD(DHT11_Test, test_100ask_dht11, module driver 100ask test dht11);
