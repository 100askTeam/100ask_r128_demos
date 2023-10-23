> 注意检查引脚冲突

# 配置

sys_config.fex:

> /home/book/work/allwinner/R128-FreeRTOS/board/r128s2/pro/configs/sys_config.fex

## IIC OLED

sys_config.fex:

[twi1]
twi1_sck        = port:PB00<3><1><default><default>
twi1_sda        = port:PB01<3><1><default><default>

menuconfig:

Drivers Options  --->
    soc related device drivers  --->
        TWI Devices  --->
            -*- enable twi driver
            [ ]   enable twi hal APIs test command