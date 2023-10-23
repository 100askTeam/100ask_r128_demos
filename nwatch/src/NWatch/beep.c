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
 * 文件名称： beep.c
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

#include "beep.h"

static byte buzzLen;
static millis8_t startTime;
static buzzFinish_f onFinish;
static tonePrio_t prio;
static uint icr;
static uint ocr;


static void stop(void);

void beep_init(void)
{
    PassiveBuzzer_Init();
}

// Non-blocking buzz
void buzzer_buzz(byte len, tone_t tone, vol_t volType, tonePrio_t _prio, buzzFinish_f _onFinish)
{
	if(_prio < prio)
		return;
	else if(tone == TONE_STOP)
	{
		stop();
		return;
	}

	// Tell power manager that we are busy buzzing
	pwrmgr_setState(PWR_ACTIVE_BUZZER, PWR_STATE_IDLE);

	prio = _prio;
	onFinish = _onFinish;
	buzzLen = len;
	startTime = millis();

	// Silent pause tone   //ÎÞÉùÔÝÍ£Òô
	if(tone == TONE_PAUSE)
	{
		PassiveBuzzer_Control(0);
		return;
	}

	// Workout volume
	byte vol;

	switch(volType)
	{
		case VOL_UI:
			vol = appConfig.volUI;
			break;
		case VOL_ALARM:
			vol = appConfig.volAlarm;
			break;
		case VOL_HOUR:
			vol = appConfig.volHour;
			break;
		default:
			vol = 2;
			break;
	}

	// Pulse width goes down as freq goes up
	// This keeps power consumption the same for all frequencies, but volume goes down as freq goes up

	vol--;
//	if(vol > 2)     //		byte volumes;³õÊ¼»¯µÄÊÊºÏÖµÊÇ255
//		return;

    icr = tone;
	ocr = icr * (256/vol);
	
	PassiveBuzzer_Set_Freq_Duty(ocr, 50);
}

// Are we buzzing?
bool buzzer_buzzing(void)
{
	return buzzLen;
}

// See if its time to stop buzzing
void buzzer_update(void)
{
	if(buzzLen && (millis8_t)(millis() - startTime) >= buzzLen)
	{
		stop();

		if(onFinish != NULL)
			onFinish();
	}
}

static void stop(void)
{
	PassiveBuzzer_Control(0);
	buzzLen = 0;
	prio = PRIO_MIN;

	pwrmgr_setState(PWR_ACTIVE_BUZZER, PWR_STATE_NONE);
}

