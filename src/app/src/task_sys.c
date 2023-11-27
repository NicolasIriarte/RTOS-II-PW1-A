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
 * @file   : app.c
 * @date   : Feb 17, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "driver.h"
#include "task_sys.h"

// To create the led task dynamically
#include "task_led.h"

/********************** macros and definitions *******************************/
#define LED_TASK_PRIORITY 2

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
// Define the queue handle
QueueHandle_t button_events_queue;
QueueHandle_t led_events_queue;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

void
push_button_event (ButtonEventType_t event)
{
  // Send data to the queue
  xQueueSend(button_events_queue, &event, portMAX_DELAY);
}

LedEventType_t
pop_led_event (void)
{
  LedEventType_t event;
  // Receive data from the queue
  xQueueReceive (led_events_queue, &event, portMAX_DELAY);
  return event;
}

/////////////////////////
static ButtonEventType_t
pop_button_event ()
{
  ButtonEventType_t event;
  // Receive data from the queue
  xQueueReceive (button_events_queue, &event, portMAX_DELAY);
  return event;
}

static void
push_led_event (LedEventType_t event)
{
  // Send data to the queue
  xQueueSend(led_events_queue, &event, portMAX_DELAY);
}

void
task_SysEvent (void *pvParameters)
{
  // Create a queue with a capacity of 10 events
  button_events_queue = xQueueCreate(10, sizeof(ButtonEventType_t));
  assert(button_events_queue != NULL);

  led_events_queue = xQueueCreate(10, sizeof(LedEventType_t));
  assert(led_events_queue != NULL);

  while (1)
    {

      ButtonEventType_t receive_event = pop_button_event ();

      LedEventType_t request =
	{ };

      TaskHandle_t xLedTaskHandle = NULL;

      BaseType_t status = xTaskCreate (task_LedEvent, "task_LedEvent", 128,
      NULL,
				       LED_TASK_PRIORITY, &xLedTaskHandle);
      assert(status == pdPASS);

      switch (receive_event)
	{
	case NONE:

	  request.color = GREEN;
	  request.led_state = OFF;
	  push_led_event (request);

	  request.color = RED;
	  request.led_state = OFF;
	  push_led_event (request);

	  break;

	case SHORT:
	  request.color = GREEN;
	  request.led_state = ON;
	  push_led_event (request);

	  request.color = RED;
	  request.led_state = OFF;
	  push_led_event (request);
	  break;

	case LONG:
	  request.color = GREEN;
	  request.led_state = OFF;
	  push_led_event (request);

	  request.color = RED;
	  request.led_state = ON;
	  push_led_event (request);
	  break;

	case STUCK:
	  request.color = GREEN;
	  request.led_state = ON;
	  push_led_event (request);

	  request.color = RED;
	  request.led_state = ON;
	  push_led_event (request);
	  break;

	default:
	  break;
	}

      // Wait until the queue is empty
      while (uxQueueMessagesWaiting (led_events_queue) > 0)
	{
	  vTaskDelay (pdMS_TO_TICKS(1)); // Wait a millisecond
	}

      vTaskDelete (xLedTaskHandle);
    }
}

/********************** end of file ******************************************/
