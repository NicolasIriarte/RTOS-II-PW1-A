/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : led_driver.c
 * @date   : Feb 17, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include "led_driver.h"
#include "eboard.h"

#include "hal.h"
#include "stm32f4xx_hal_conf.h"

#include <assert.h>
#include <stdbool.h>

/********************** macros and definitions *******************************/

#define BLINK_PERIOD_MS (500)

// Define the task stack size
#define TASK_STACK_SIZE 128

/********************** internal data declaration ****************************/

typedef struct {
  led_color_t color;
  bool is_active;
} internal_led_state_t;

/********************** internal functions declaration ***********************/
static void change_all_leds_state(bool state);

static void turn_on_led(led_color_t color);

static void add_or_update_blink_task(led_color_t color);

static void remove_blink_task(void);

static void blink_led_task(void *pvParameters);

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

static internal_led_state_t led_state;

static TaskHandle_t xTaskBlinkLedHandle = NULL;

/********************** internal functions definition ************************/

void led_driver_init(void) {
  // The driver shall grant that all leds are off at the beginning.
  eboard_led_red(false);
  eboard_led_green(false);
  eboard_led_blue(false);

  // Internal driver state
  led_state.color = RED; // To initialize the driver with a valid color.
  led_state.is_active = false;
}

void led_driver_set_pattern(led_color_t color, led_pattern_t pattern) {

  switch (pattern) {
  case OFF:
    remove_blink_task(); // If the task is running, remove it.
    change_all_leds_state(false);
    break;
  case ON:
    remove_blink_task(); // If the task is running, remove it.
    turn_on_led(color);
    break;
  case BLINK:
    add_or_update_blink_task(color);
    break;
  default:
    // error
    assert(0);
  break;
  }
}

static void change_all_leds_state(bool state) {
  eboard_led_red(state);
  eboard_led_green(state);
  eboard_led_blue(state);
}

static void turn_on_led(led_color_t color) {

  // Validación de la entrada
  if (color < RED || color > WHITE) {
    // error;
    assert(0);
    return;
  }

  bool red_on = false;
  bool green_on = false;
  bool blue_on = false;

  switch (color) {
    case RED:
      red_on = true;
      break;
    case GREEN:
      green_on = true;
      break;
    case BLUE:
      blue_on = true;
      break;
    case YELLOW:
      red_on = green_on = true;
      break;
    case CYAN:
      green_on = blue_on = true;
      break;
    case MAGENTA:
      red_on = blue_on = true;
      break;
    case WHITE:
      red_on = green_on = blue_on = true;
      break;
    default:
      // error;
      assert(0);
      break;
    return;
  }

  eboard_led_red(red_on);
  eboard_led_green(green_on);
  eboard_led_blue(blue_on);
}


static void add_or_update_blink_task(led_color_t color) {
  // If the task is not running, create it.
  led_state.color = color;

  taskENTER_CRITICAL();

  if (!led_state.is_active) {
    // Create the task
    BaseType_t status =
        xTaskCreate(blink_led_task, "blink_led_task", TASK_STACK_SIZE, NULL,
                    tskIDLE_PRIORITY, &xTaskBlinkLedHandle);

    assert(status == pdPASS);
  }

  led_state.is_active = true;
  taskEXIT_CRITICAL();
}

static void remove_blink_task(void) {
  taskENTER_CRITICAL();

  if (led_state.is_active) {
    // Remove the task from FreeRTOS scheduler
    vTaskDelete(xTaskBlinkLedHandle);
    xTaskBlinkLedHandle = NULL;
  }

  led_state.is_active = false;

  taskEXIT_CRITICAL();
}

static void blink_led_task(void *pvParameters) {
  while (1) {
    turn_on_led(led_state.color);
    vTaskDelay(BLINK_PERIOD_MS / portTICK_PERIOD_MS);
    change_all_leds_state(false);
    vTaskDelay(BLINK_PERIOD_MS / portTICK_PERIOD_MS);
  }
}

/********************** end of file ******************************************/
