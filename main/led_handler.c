#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include <stdbool.h>

#define BLINK_GPIO 5
static bool isOn = 0;


void blink()
{
	gpio_pad_select_gpio(BLINK_GPIO);
	    /* Set the GPIO as a push/pull output */
	    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

	    if (isOn)
	    {
	    	gpio_set_level(BLINK_GPIO, 0);
	    	isOn = 0;
	    } else
	    {
	    	gpio_set_level(BLINK_GPIO, 1);
	    	isOn = 1;
	    }

}
