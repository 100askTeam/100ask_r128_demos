sys_config.fex:

```shell
[spi1]
spi1_used       = 1
spi1_cs_number  = 1
spi1_cs_bitmap  = 1
spi1_cs0        = port:PA12<6><0><3><default>
spi1_sclk       = port:PA13<6><0><3><default>
spi1_mosi       = port:PA18<6><0><3><default>
spi1_miso       = port:PA21<6><0><3><default>
;spi1_hold       = port:PA19<6><0><2><default>
;spi1_wp         = port:PA20<6><0><2><default>
```

menuconfig:

```shell

Drivers Options  --->
    soc related device drivers  --->
        SPI Devices   --->
            [*] enable spi driver
            [*]   support sp transfer if crash
            [ ]   enable spi hal APIs test command
```
