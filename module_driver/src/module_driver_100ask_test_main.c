#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <console.h>

#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_LCD
#include "driver_lcd.h"
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
#include "lcd_100ask_spi.h"
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_PASSIVE_BUZZER
#include "driver_passive_buzzer.h"
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_ROTARY_ENCODER
#include "driver_rotary_encoder.h"
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_IR_RECEIVER
#include "driver_ir_receiver.h"
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_FLASH
#include "driver_spiflash.h"
#endif


int module_driver_100ask_test_main(int argc, char *argv[])
{
#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_LCD
    lcd_100ask_spi_test(0, NULL);
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_IIC_OLED
    LCD_Test(0, NULL);
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_PASSIVE_BUZZER
    PassiveBuzzer_Test(0, NULL);
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_ROTARY_ENCODER
    RotaryEncoder_Test(0, NULL);
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_IR_RECEIVER
    IRReceiver_Test(0, NULL);
#endif

#if CONFIG_USE_100ASK_MODULE_DRIVER_SPI_FLASH
    spi_flash_test(0, NULL);
#endif

    return 0;
}


//FINSH_FUNCTION_EXPORT_CMD(module_driver_100ask_test_main, driver_100ask_test, module driver 100ask test demo);
