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

