/*
 * Project: Lightweight millisecond tracking library
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/millisecond-tracking-library-for-avr/
 */

#ifndef ARDUINO


#include "common.h"
#include <hal_timer.h>

//millis_t milliseconds;

// Get current milliseconds
millis_t millis_get(void)
{
	uint64_t hal_time_value = hal_gettime_ns();
    if(hal_time_value < 0) hal_time_value = 0;

    return (uint32_t)(hal_gettime_ns() / 1000 / 1000);

	///return system_get_ms();
}

#endif
