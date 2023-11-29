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
#include "app.h"
#include "task_button.h"
#include "task_led.h"
#include "led_event_queue.h"
#include "memory_pool.h"

/********************** macros and definitions *******************************/

// Define the number of tasks the Queue and the memory pool will handle
#define NUM_TASKS 2

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static memory_pool_t memory_pool;
static EventType_t static_memory[NUM_TASKS];

/**
 * Initialize this module
 */
void led_event_init(void) {
	memory_pool_init(&memory_pool, &static_memory, NUM_TASKS,
			sizeof(EventType_t));

}

/**
 * This function push a pointer event into the queue.
 * If the send fails, the caller is responsible to free the memory.
 * If the event is succefully pushed, the receiver is in charge of
 * freeing the memory.
 */
void push_led_event(EventType_t event) {
	memory_pool_block_put(&memory_pool, &event);

	volatile int lala;
}

/**
 * Get a pointer to an event, the caller of this function must
 * free the memory.
 */
EventType_t* pop_led_event(void) {
	EventType_t *event;
	// Receive data from the memory pool
	void *data = memory_pool_block_get(&memory_pool);
	if (data == NULL) {
		volatile int lala;
	}
	event = (EventType_t*) data;

	return event;
}

/********************** end of file ******************************************/
