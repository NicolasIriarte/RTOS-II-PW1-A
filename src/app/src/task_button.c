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
#include "task_button.h"
#include "app.h"

/********************** macros and definitions *******************************/

#define TASK_DELAY 5

#define pdTICKS_TO_MS( xTicks ) \
    ( ( ( TickType_t ) ( xTicks ) * 1000u ) / configTICK_RATE_HZ )

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static EventType_t
TimeToEventType (ButtonTime_t time)
{

  EventType_t event_type;
  // Classify time
  if (time < SHORT_TIME)
    {
      event_type = NONE;
    }
  else if (time < LONG_TIME)
    {
      event_type = SHORT;
    }
  else if (time < STUCK_TIME)
    {
      event_type = LONG;
    }
  else
    {
      event_type = STUCK;
    }
  return event_type;
}

void
task_ButtonEvent (void *pvParameters)
{
  ButtonTime_t last_time_event = 0;
  ButtonTime_t delta_time = 0;
  bool restart_timer_flag = true;

  while (1)
    {
      if (eboard_switch ()) // Button pressed
	{

	  if (restart_timer_flag)
	    {
	      last_time_event = pdTICKS_TO_MS(eboard_osal_port_get_time ());
	      restart_timer_flag = false;
	    }

	  delta_time = pdTICKS_TO_MS(eboard_osal_port_get_time ())
	      - last_time_event;

	  if (delta_time > STUCK_TIME)
	    {
	      push_led_event (STUCK);
	    }
	  else
	    {
	      push_led_event (NONE);
	    }
	}
      else // Button not pressed
	{

	  EventType_t event_type = TimeToEventType (delta_time);

	  if (delta_time > STUCK_TIME)
	    {
	      event_type = NONE;
	    }

	  // Push event to the queue
	  push_led_event (event_type);
	  restart_timer_flag = true;
	}
      eboard_osal_port_delay (TASK_DELAY);
    }
}

/********************** end of file ******************************************/
