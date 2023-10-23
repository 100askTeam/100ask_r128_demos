#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <console.h>

#include "common.h"

#ifndef configAPPLICATION_NORMAL_PRIORITY
#define configAPPLICATION_NORMAL_PRIORITY (15)
#endif

static void nwatch_time_update_thread_entry(void * param);

int nwatch_100ask_main(int argc, char *argv[])
{
    if (argv[1] == NULL || atoi(argv[1]) < 0 || atoi(argv[1]) > 3) {
        printf("nwatch_100ask 1, use rotary encoder for control\n");
        printf("nwatch_100ask 2, use infrared remote for control\n");
        printf("nwatch_100ask 3, use infrared remote and rotary encoder for control\n");
        return 0;
    }

    appconfig_init();

    beep_init();
    humiture_init();
    display_init();

    draw_init();

    time_init();
	///rtc_init();
	global_init();
    alarm_init();
    buttons_init(atoi(argv[1]));
	pwrmgr_init();
    
    // Set watchface
	display_set(watchface_normal);
	display_load();

    portBASE_TYPE task_ret;
    TaskHandle_t nwatch_time_update_task;
    task_ret = xTaskCreate(nwatch_time_update_thread_entry, (signed portCHAR *) "lv_100ask_desktop", 4096, NULL, configAPPLICATION_NORMAL_PRIORITY, &nwatch_time_update_task);

    while (1)
    {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
            time_update();
            if(pwrmgr_userActive())
            {
                ///battery_update();
                buttons_update();
            }
                
            buzzer_update();
            
            ///led_update();
            
    #if COMPILE_STOPWATCH
            stopwatch_update();
    #endif
            
            global_update();

            if(pwrmgr_userActive())
            {
                alarm_update();
                display_update();
            }

            pwrmgr_update();

    }

    return 0;
}


static void nwatch_time_update_thread_entry(void * param)
{
    while(1) {
        update = true;
	
#ifndef RTC_SRC
        ++timeDate.time.secs;
#endif
        sleep(1);
    }

    vTaskDelete(NULL);
}

FINSH_FUNCTION_EXPORT_CMD(nwatch_100ask_main, nwatch_100ask, NWatch 100ask demo);
