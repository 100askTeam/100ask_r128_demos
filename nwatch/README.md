# NWatch-R128

> 注意检查引脚冲突

# 配置

sys_config.fex:

> /home/book/work/allwinner/R128-FreeRTOS/board/r128s2/pro/configs/sys_config.fex

## IIC OLED

sys_config.fex:

```shell
[twi1]
twi1_sck        = port:PB00<3><1><default><default>
twi1_sda        = port:PB01<3><1><default><default>
```

menuconfig:

```shell
Drivers Options  --->
    soc related device drivers  --->
        TWI Devices  --->
            -*- enable twi driver
            [ ]   enable twi hal APIs test command
```

## BEEP

sys_config.fex:

```shell

[pwm6]
pwm_used        = 1
pwm_positive    = port:PA26<4><0><2><default>
```


menuconfig:

```shell
Drivers Options  --->
    soc related device drivers  --->
        PWM devices --->
            [*] enable pwm driver
            [ ]   enable pwm hal APIs test command
```

## EC11


GPIO:

```shell
S1_GPIO_PIN   -------  GPIOA(24)
S2_GPIO_PIN   -------  GPIOA(25)
KEY_GPIO_PIN  -------  GPIOA(29)
```

menuconfig:

```shell
Drivers Options  --->
    soc related device drivers  --->
        GPIO devices --->
            [*] enable GPIO driver
            [ ]   enable gpio hal APIs test command
            [ ] enable gpio chip aw9523
            [*] enable amp gpio irq
```

## IR

GPIO:

```shell
IR_RECEIVER_PIN   -------   GPIOA(10)
```
menuconfig:

```shell
Drivers Options  --->
    soc related device drivers  --->
            GPIO devices --->
                [*] enable GPIO driver
                [ ]   enable gpio hal APIs test command
                [ ] enable gpio chip aw9523
                [*] enable amp gpio irq
```



