#include "lvgl/lvgl.h"
#include "lv_drivers/display/sunxifb.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_100ask_desktop/src/lv_100ask_desktop.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <console.h>

#ifndef configAPPLICATION_NORMAL_PRIORITY
#define configAPPLICATION_NORMAL_PRIORITY (15)
#endif
static int exit_flag = 0;

static void lv_thread_entry(void * param)
{
    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
        usleep(5000);
        if(exit_flag == 1)
        {
            exit_flag = 0;
            break;
        }
    }
    vTaskDelete(NULL);
}


int lvgl_100ask_dektop_main(int argc, char *argv[])
{    
    /*LittlevGL init*/
    lv_init();

    uint32_t rotated = LV_DISP_ROT_NONE;

    /*RTOS frame buffer device init*/
    sunxifb_init(rotated);

    /*A buffer for LittlevGL to draw the screen's content*/
    static uint32_t width, height;
    sunxifb_get_sizes(&width, &height);

    static lv_color_t *buf;
    buf = (lv_color_t*) sunxifb_alloc(width * height * sizeof(lv_color_t),
            "lv_100ask_desktop");

    if (buf == NULL) {
        sunxifb_exit();
        printf("malloc draw buffer fail\n");
        return 0;
    }

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, width * height);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = sunxifb_flush;
    disp_drv.hor_res    = width;
    disp_drv.ver_res    = height;
    disp_drv.rotated    = rotated;
#ifndef USE_SUNXIFB_G2D_ROTATE
    if (rotated != LV_DISP_ROT_NONE)
        disp_drv.sw_rotate = 1;
#endif
    lv_disp_drv_register(&disp_drv);

#if USE_EVDEV
    evdev_init();
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);                /*Basic initialization*/
    indev_drv.type =LV_INDEV_TYPE_POINTER;        /*See below.*/
    indev_drv.read_cb = evdev_read;               /*See below.*/
    /*Register the driver in LVGL and save the created input device object*/
    lv_indev_t * evdev_indev = lv_indev_drv_register(&indev_drv);
#endif

    /*Create a Demo*/
    lv_100ask_desktop();

    portBASE_TYPE task_ret;
    TaskHandle_t lv_task;
    task_ret = xTaskCreate(lv_thread_entry, (signed portCHAR *) "lv_100ask_desktop", 4096, NULL, configAPPLICATION_NORMAL_PRIORITY, &lv_task);
    if (task_ret != pdPASS) {
        printf("create lv_100ask_desktop task err\n");
    }
    while(1)
    {
        char cRxed = 0;

        cRxed = getchar();
        if(cRxed == 'q' || cRxed == 3)
        {
            exit_flag = 1;
            sunxifb_free((void**) &buf, "lv_100ask_desktop");
            sunxifb_exit();
            lv_deinit();
            return 0;
        }
    }

    /*sunxifb_free((void**) &buf, "lv_100ask_desktop");*/
    /*sunxifb_exit();*/
    return 0;
}


/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t __attribute__((weak)) custom_tick_get(void) {
    static uint64_t start_ms = 0;
    if (start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = ((uint64_t) tv_start.tv_sec * 1000000
                + (uint64_t) tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = ((uint64_t) tv_now.tv_sec * 1000000 + (uint64_t) tv_now.tv_usec)
            / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

FINSH_FUNCTION_EXPORT_CMD(lvgl_100ask_dektop_main, lv_100ask_desktop, lvgl_100ask_desktop);
