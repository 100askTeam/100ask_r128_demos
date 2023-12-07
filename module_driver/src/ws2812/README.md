1. 前往 lichee/rtos/drivers/rtos-hal/hal/source/ledc/platform/ledc_sun20iw2.h 并编辑 LEDC 的引脚和MUX

```sehll
#define LEDC_PIN    GPIOA(13)
#define LEDC_PINMUXSEL  7
```

2. 然后编辑 lichee/rtos/drivers/rtos-hal/hal/source/ledc/hal_ledc.c 配置 WS2812 的时序参数：

```c
struct ledc_config ledc_config = {
    .led_count = 4,
    .reset_ns = 84,
    .t1h_ns = 1000,
    .t1l_ns = 1000,
    .t0h_ns = 580,
    .t0l_ns = 1000,
    .wait_time0_ns = 84,
    .wait_time1_ns = 84,
    .wait_data_time_ns = 600000,
    .output_mode = "GRB",
};
```

3. mrtos_menuconfig

```shell
Drivers Options  --->
    soc related device drivers  --->
            LEDC devices --->
                [*] enable ledc driver
```
