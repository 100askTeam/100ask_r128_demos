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
 * 文件名称： driver_spiflash.c
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

#include <console.h>
#include <unistd.h>
#include <hal_log.h>
#include <hal_timer.h>

#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_FLASH_W25Q64
#include "driver_spiflash_w25q64.h"
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
#include "driver_lcd.h"
#endif

#define SPI_FALSH_100ASK_SPI_PORT           CONFIG_100ASK_MODULE_DRIVER_SPI_FLASH_PORT
#define SPI_FALSH_100ASK_SPI_FREQUENCY      CONFIG_100ASK_MODULE_DRIVER_SPI_FLASH_FREQUENCY


void spi_falsh_init(void)
{
#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_FLASH_W25Q64
    W25Q64_Init(SPI_FALSH_100ASK_SPI_PORT, SPI_FALSH_100ASK_SPI_FREQUENCY);
#endif
}


/**********************************************************************
 * 函数名称： spi_falsh_read
 * 功能描述： W25Q64读函数
 * 输入参数： offset - 读哪个地址
 *            len    - 读多少字节
 * 输出参数： buf - 用来保存数据
 * 返 回 值： 非负数 - 读取了多少字节的数据, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int spi_falsh_read(uint32_t offset, uint8_t *buf, uint32_t len)
{
#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_FLASH_W25Q64
    return spi_falsh_read(offset, buf, len);
#endif
}


/**********************************************************************
 * 函数名称： spi_flash_write
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
int spi_flash_write(uint32_t offset, uint8_t *buf, uint32_t len)
{
#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_FLASH_W25Q64
    return spi_flash_write(offset, buf, len);
#endif
}


/**********************************************************************
 * 函数名称： spi_flash_erase
 * 功能描述： W25Q64写函数
 * 输入参数： offset - 擦除哪个地址(4096对齐)
 *            len    - 擦除多少字节(4096对齐)
 * 输出参数： 无
 * 返 回 值： 非负数 - 擦除了多少字节的数据, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int spi_flash_erase(uint32_t offset, uint32_t len)
{
#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_FLASH_W25Q64
    return spi_flash_erase(offset, len);
#endif
}



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
int spi_flash_test(int argc, char *argv[])
{
    int sector;
    int len;
    uint8_t buf[4];
    int i;
    uint32_t val1, val2;

    spi_falsh_init();

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
    LCD_Init();
	LCD_Clear();

    while (1)
    {
        LCD_PrintString(0, 0, "W25Q64 Test: ");

        for (sector = 0; sector < 2048; sector++)
        {
            /* 擦除测试 */
            LCD_ClearLine(0, 2);
            len = LCD_PrintString(0, 2, "Erase ");
            LCD_PrintSignedVal(len, 2, sector);
            spi_flash_erase(sector * 4096, 4096);    
            spi_falsh_read(sector * 4096, buf, 4);
            LCD_ClearLine(0, 4);
            for (i = 0; i < 4; i++)
            {
                LCD_PrintHex(i*3, 4, buf[i], 0);
            }
            vTaskDelay(1000 / portTICK_RATE_MS);

            /* 写入测试 */
            LCD_ClearLine(0, 2);
            len = LCD_PrintString(0, 2, "Write ");
            LCD_PrintSignedVal(len, 2, sector);

            val1 = hal_gettime_ns();
            printf("val1:%d\n", val1);
            spi_flash_write(sector * 4096, (uint8_t *)&val1, 4);
            LCD_ClearLine(0, 4);
            LCD_PrintHex(0, 4, val1, 1);
            vTaskDelay(1000 / portTICK_RATE_MS);

            /* 读出测试 */
            LCD_ClearLine(0, 2);
            len = LCD_PrintString(0, 2, "Read ");
            LCD_PrintSignedVal(len, 2, sector);

            spi_falsh_read(sector * 4096, (uint8_t *)&val2, 4);
            printf("val2:%d\n", val2);
            LCD_ClearLine(0, 4);
            if (val1 == val2)
                LCD_PrintString(0, 4, "Test ok");
            else
            {
                LCD_PrintHex(0, 4, val2, 1);
                LCD_ClearLine(0, 6);
                LCD_PrintString(0, 6, "Test Failed");
            }

            vTaskDelay(1000 / portTICK_RATE_MS);
            
        }
    }
#else

    while (1)
    {
        for (sector = 0; sector < 2048; sector++)
        {
            /* 擦除测试 */
            spi_flash_erase(sector * 4096, 4096);    
            spi_falsh_read(sector * 4096, buf, 4);
            printf("Erase: 0x%x\n", buf);
            vTaskDelay(1000 / portTICK_RATE_MS);

            /* 写入测试 */
            val1 = hal_gettime_ns();
            printf("Write: 0x%x\n", val1);
            spi_flash_write(sector * 4096, (uint8_t *)&val1, 4);
            vTaskDelay(1000 / portTICK_RATE_MS);

            /* 读出测试 */
            spi_falsh_read(sector * 4096, (uint8_t *)&val2, 4);
            printf("Read: 0x%x\n", val2);
            if (val1 == val2)
                printf(" Test ok !!!\n\n");
            else
                printf(" Test Failed!!!\n\n");

            vTaskDelay(1000 / portTICK_RATE_MS);
            
        }
    }

#endif

    return 0;
}



FINSH_FUNCTION_EXPORT_CMD(spi_flash_test, test_100ask_spi_flash, module driver 100ask test spi flash);