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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "main.h"

#include "uart_driver_rx.h"
#include "uart_driver_tx.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static char mgs1[] = "hola";
static char mgs2[] = "chau";

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

void driver_uart_tx_error_callback(void) {
	while (1)
		;
}

static void task_tx_example_(void *argument) {
	driver_uart_tx_init();

	while (true) {
		driver_uart_tx((uint8_t*) mgs1, (size_t) strlen(mgs1));
		driver_uart_tx((uint8_t*) mgs2, (size_t) strlen(mgs2));

		driver_uart_tx_tick();
		vTaskDelay((TickType_t) ((10) / portTICK_PERIOD_MS));
	}
}

void driver_uart_rx(uint8_t *buffer, size_t size) {
	// Put a breakpoint here to see the content of buffer and size.
	return;
}

static void task_rx_example_(void *argument) {
	driver_uart_rx_init();

	while (true) {
		driver_uart_rx_tick();
		vTaskDelay((TickType_t) ((10) / portTICK_PERIOD_MS));
	}
}

// TODO: Add task for RX here and add it to the scheduler.

/********************** external functions definition ************************/

void app_init(void) {
	// tasks
	{
		BaseType_t status;

		status = xTaskCreate(task_tx_example_, "task_tx_example", 128, NULL,
		tskIDLE_PRIORITY, NULL);
		assert(status == pdPASS);

		status = xTaskCreate(task_rx_example_, "task_rx_example", 128, NULL,
		tskIDLE_PRIORITY, NULL);
		assert(status == pdPASS);

		__enable_irq();

		while (pdPASS != status) {
			// error
		}
	}
}

/********************** end of file
 * ******************************************/
