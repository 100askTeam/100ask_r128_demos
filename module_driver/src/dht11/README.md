Default GPIO:

```shell
DHT11_PIN   -------   GPIOA(6)
```

menuconfig:

```shell
Drivers Options  --->
    soc related device drivers  --->
            GPIO devices --->
                [*] enable GPIO driver
                [ ]   enable gpio hal APIs test command
                [ ] enable gpio chip aw9523
                [ ] enable amp gpio irq

```
