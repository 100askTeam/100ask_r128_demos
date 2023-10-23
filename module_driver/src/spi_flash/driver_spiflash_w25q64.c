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
 * 文件名称： w25q64_100ask_spi.c
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

#include <hal_timer.h>

#include "driver_spiflash_w25q64.h"

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
#include "driver_lcd.h"
#endif

#define W25Q64_TIMEOUT       500

static hal_spi_master_port_t  g_spi_port = 1;


void W25Q64_Init(hal_spi_master_port_t port, uint32_t clock_frequency)
{
    spi_master_status_t ret;
    hal_spi_master_config_t cfg;

    g_spi_port = port;
    //g_spi_port = 1;

	/* init spi */
    cfg.clock_frequency = clock_frequency;
    //cfg.clock_frequency = 18000000;
    cfg.slave_port = HAL_SPI_MASTER_SLAVE_0;
    cfg.cpha = HAL_SPI_MASTER_CLOCK_PHASE0;
    cfg.cpol = HAL_SPI_MASTER_CLOCK_POLARITY0;
    cfg.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    //cfg.spol = 0;
    //cfg.sip = 0;
    //cfg.flash = 0;

    ret = hal_spi_init(port, &cfg);
	
    if (ret != SPI_MASTER_OK)
		printf("init spi master failed - %d\n", ret);
	///ret = hal_spi_hw_config(port, &cfg);

}


/**********************************************************************
 * 函数名称： W25Q64_TxRx
 * 功能描述： 使用SPI发送/接收数据(注意这个函数没有设置片选信号)
 * 输入参数： pTxData - 要发送的数据
 *            Size    - 数据长度
 *            Timeout - 超时时间(单位ms)
 * 输出参数： pRxData - 接收缓冲区
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int  W25Q64_TxRx(uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
    spi_master_status_t ret;
    hal_spi_master_transfer_t t;

    memset(&t, 0, sizeof(hal_spi_master_transfer_t));

	t.tx_buf = pTxData;
	t.tx_len = Size;
	t.rx_buf = pRxData;
	t.rx_len = Size;
	t.dummy_byte = 0;
	t.tx_single_len = Size;
    t.bits_per_word	= 8;
	t.tx_nbits = SPI_NBITS_SINGLE;

	ret = hal_spi_xfer(g_spi_port, &t);

    if(ret == SPI_MASTER_OK)
        return 0;
    else
        return -1;
}

/**********************************************************************
 * 函数名称： W25Q64_Tx
 * 功能描述： 使用SPI发送数据(注意这个函数没有设置片选信号)
 * 输入参数： pTxData - 要发送的数据
 *            Size    - 数据长度
 *            Timeout - 超时时间(单位ms)
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int  W25Q64_Tx(uint8_t *pTxData, uint16_t Size, uint32_t Timeout)
{
    spi_master_status_t ret;
    hal_spi_master_transfer_t t;

#if 1
    ret = hal_spi_write(g_spi_port, pTxData, Size);

    if(ret == SPI_MASTER_OK)
        return 0;
    else
        return -1;
#else
    t.tx_buf = (void *)pTxData;
	t.tx_len	= Size;
	t.tx_single_len	= Size;
	t.bits_per_word	= 8;
	t.dummy_byte = 0;
	t.rx_buf = NULL;
	t.rx_len = 0;
	t.tx_nbits = SPI_NBITS_SINGLE;

	ret =  hal_spi_xfer(g_spi_port, &t);
#endif
}

/**********************************************************************
 * 函数名称： W25Q64_Rx
 * 功能描述： 使用SPI读取数据(注意这个函数没有设置片选信号)
 * 输入参数： Size    - 数据长度
 *            Timeout - 超时时间(单位ms)
 * 输出参数： pRxData - 接收缓冲区
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int  W25Q64_Rx(uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
    spi_master_status_t ret;
    hal_spi_master_transfer_t t;

#if 1
    ret = hal_spi_read(g_spi_port, pRxData, Size);
    if(ret == SPI_MASTER_OK)
        return 0;
    else
        return -1;
#else
    t.tx_buf = NULL;
	t.tx_len	= 0;
	t.tx_single_len	= 0;
	t.bits_per_word	= 8;
	t.dummy_byte = 0;
	t.rx_buf = pRxData;
	t.rx_len = Size;
	t.tx_nbits = SPI_NBITS_SINGLE;

	ret =  hal_spi_xfer(g_spi_port, &t);
#endif
}


/**********************************************************************
 * 函数名称： W25Q64_WaitReady
 * 功能描述： 等待W25Q64就绪
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int W25Q64_WaitReady(void)
{
    unsigned char tx_buf[2];
    unsigned char rx_buf[2];
    int timeout = W25Q64_TIMEOUT;

    tx_buf[0] = 0x05; /* 读状态 */
    tx_buf[1] = 0xff;
	
	while (timeout--)
	{
		rx_buf[0] = rx_buf[1] = 0;
        
		W25Q64_TxRx(tx_buf, rx_buf, 2, W25Q64_TIMEOUT);
        
		if ((rx_buf[1] & 1) == 0)
        {
            return 0;
        }
        usleep(1000);
        //vTaskDelay(1 / portTICK_RATE_MS);
	}

    if (!timeout)
    {
        printf("time out\n");
    	return -1;
    }
	return 0;
}

/**********************************************************************
 * 函数名称： W25Q64_WriteEnable
 * 功能描述： 写使能
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, (-1)-失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int W25Q64_WriteEnable(void)
{
    unsigned char tmpbuf[1];
    int err;
    
    tmpbuf[0] = 0x06; /* 写使能 */
    
    err = W25Q64_Tx(tmpbuf, 1, W25Q64_TIMEOUT);
    

    return err;
}



/**********************************************************************
 * 函数名称： W25Q64_Read
 * 功能描述： W25Q64读函数
 * 输入参数： offset - 读哪个地址
 *            len    - 读多少字节
 * 输出参数： buf - 用来保存数据
 * 返 回 值： 非负数 - 读取了多少字节的数据, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int W25Q64_Read(uint32_t offset, uint8_t *buf, uint32_t len)
{
    unsigned char tmpbuf[4];
    int err;
    
    /* 自己实现SPI Flash的读操作 */
    tmpbuf[0] = 0x03;
    tmpbuf[1] = (offset >> 16) & 0xff;
    tmpbuf[2] = (offset >> 8) & 0xff;
    tmpbuf[3] = (offset >> 0) & 0xff;
    
    
    /* 发送读命令 */
    err = W25Q64_Tx(tmpbuf, 4, W25Q64_TIMEOUT);
    if (err)
    {        
        return -1;
    }
      

    /* 读数据 */
    err = W25Q64_Rx(buf, len, W25Q64_TIMEOUT);
    if (err)
    {        
        return -1;
    }
        
    return len;
}


/**********************************************************************
 * 函数名称： W25Q64_Write
 * 功能描述： W25Q64写函数(需要先擦除)
 * 输入参数： offset - 写哪个地址
 *            buf    - 数据buffer
 *            len    - 写多少字节
 * 输出参数： 无
 * 返 回 值： 非负数 - 写了多少字节的数据, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int W25Q64_Write(uint32_t offset, uint8_t *buf, uint32_t len)
{
    uint8_t tmpbuf[4];
    uint32_t phy_pos = offset;
    int err;
    uint32_t cur_len;
    uint32_t remain_len;

    /* 写数据 */
    phy_pos = offset;
    remain_len = len;

    /* 可能不是从Page开头写数据(1个Page是256字节) */
    cur_len = offset & (256-1);
    cur_len = 256 - cur_len;
    if (cur_len > len)
        cur_len = len;
    
    for (; phy_pos < offset + len; )
    {
        /* 写使能 */
        err = W25Q64_WriteEnable();
        if (err)
        {
            return -1;
        }
        
        tmpbuf[0] = 0x02; /* page program */
        tmpbuf[1] = (phy_pos >> 16) & 0xff;
        tmpbuf[2] = (phy_pos >> 8) & 0xff;
        tmpbuf[3] = (phy_pos >> 0) & 0xff;

        /* 发送page program命令 */
        
        err = W25Q64_Tx(tmpbuf, 4, W25Q64_TIMEOUT);
        if (err)
        {
            return -1;
        }
        
        /* 发送数据 */
        err = W25Q64_Tx(buf, cur_len, W25Q64_TIMEOUT);
        if (err)
        {
            
            return -1;
        }
        
        
        /* 读状态 */
        err = W25Q64_WaitReady();
        if (err)
        {
            return -1;
        }

        phy_pos += cur_len;
        buf     += cur_len;
        remain_len -= cur_len;

        cur_len = (remain_len < 256) ? remain_len : 256;
    }
    return len;
}


/**********************************************************************
 * 函数名称： W25Q64_Erase
 * 功能描述： W25Q64写函数
 * 输入参数： offset - 擦除哪个地址(4096对齐)
 *            len    - 擦除多少字节(4096对齐)
 * 输出参数： 无
 * 返 回 值： 非负数 - 擦除了多少字节的数据, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int W25Q64_Erase(uint32_t offset, uint32_t len)
{
    unsigned char tmpbuf[4];
    uint32_t phy_pos = offset;
    int err;

    if ((offset & (4096-1)) && (len & (4096-1)))
        return -1;

    for (int sector = 0; sector < len/4096; sector++)
    {
        /* 写使能 */
        err = W25Q64_WriteEnable();
        if (err)
        {
            return -1;
        }
        
        tmpbuf[0] = 0x20; /* 擦除 */
        tmpbuf[1] = (phy_pos >> 16) & 0xff;
        tmpbuf[2] = (phy_pos >> 8) & 0xff;
        tmpbuf[3] = (phy_pos >> 0) & 0xff;
        
        err = W25Q64_Tx(tmpbuf, 4, W25Q64_TIMEOUT);
        if (err)
        {

            return -1;
        }
        
        phy_pos += 4096;

        /* 读状态 */
        err = W25Q64_WaitReady();
        if (err)
        {
            return -1;
        }
    }

    return len;
}

