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
 * 文件名称： humiture.c
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
 * 2023.10.20   v01         zhouyuebiao   创建文件
 *-----------------------------------------------------
 */

#include "humiture.h"

static dht11_t  g_dht11_data_t;

void humiture_init(void)
{
    DHT11_Init();
}

void DHT11_Update(void)
{
	static millis_t lastUpdate = 0;
	int res;

	// Update every 10ms
	millis_t now = millis();
	if ((millis_t)(now - lastUpdate) >= 1000)
	{
		lastUpdate = now;

		res = DHT11_Read(&g_dht11_data_t.hum, &g_dht11_data_t.temp);
		if(res)
			DHT11_Init();
	}
}	


dht11_t *DHT11_Get_Data(void)
{
	return &g_dht11_data_t;
}
