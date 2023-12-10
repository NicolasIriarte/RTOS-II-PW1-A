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
 * @file   : driver_uart.c
 * @date   : Dec 6, 2023
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

#include "uart_driver.h"

/********************** macros and definitions *******************************/

#define TX_BUFFER_SIZE_ (32)                                   // [B]
#define TX_TICK_PERIOD_ (10)                                   // [ms]
#define TX_TIMEOUT__MS_ (100)                                  // [ms]
#define TX_TIMEOUT__TICKS_ (TX_TIMEOUT__MS_ / TX_TICK_PERIOD_) // [ticks]

#define TX_USER_ (tx_.user)
#define TX_USER_BUFFER_ (tx_.user->buffer)
#define TX_USER_LEN_ (tx_.user->len)
#define TX_USER_AVAILABLE_SIZE_ (TX_BUFFER_SIZE_ - TX_USER_LEN_)
#define TX_PERIPH_ (tx_.periph)
#define TX_PERIPH_BUFFER_ (tx_.periph->buffer)
#define TX_PERIPH_LEN_ (tx_.periph->len)
#define TX_PERIPH_ENABLE_ (tx_.periph->enable)

#define UART_HAL_HANDLE_ (&huart3)

/********************** internal data declaration ****************************/

typedef struct {
	uint8_t buffer[TX_BUFFER_SIZE_];
	size_t len;
	bool enable;
} tx_swapbuffer_t_;

typedef struct {
	tx_swapbuffer_t_ *user;
	tx_swapbuffer_t_ *periph;
	tx_swapbuffer_t_ swapbuffer[2];
	uint32_t timeout;
	bool error;
} driver_tx_t_;

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static driver_tx_t_ tx_;

/********************** external data definition *****************************/

extern UART_HandleTypeDef huart3;

/********************** internal functions definition ************************/

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	driver_uart_tx_error_callback();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	TX_PERIPH_ENABLE_ = true;
	TX_PERIPH_LEN_ = 0;
}

static void tx_swapbuffer_init_(tx_swapbuffer_t_ *swapbuffer) {
	swapbuffer->enable = true;
	swapbuffer->len = 0;
}

static void tx_init_(void) {
	tx_.user = &(tx_.swapbuffer[0]);
	tx_swapbuffer_init_(tx_.user);

	tx_.periph = &(tx_.swapbuffer[1]);
	tx_swapbuffer_init_(tx_.periph);

	tx_.timeout = 0;
}

static void tx_send_buffer_(void) {
	if (TX_PERIPH_ENABLE_ && (0 < TX_PERIPH_LEN_)) {
		HAL_StatusTypeDef satus = HAL_UART_Transmit_IT(
		UART_HAL_HANDLE_, TX_PERIPH_BUFFER_, TX_PERIPH_LEN_);
		switch (satus) {
		case HAL_OK:
			TX_PERIPH_ENABLE_ = false;
			break;
		case HAL_ERROR:
		case HAL_BUSY:
		case HAL_TIMEOUT:
		default:
			driver_uart_tx_error_callback();
			break;
		}
	}
}

static bool tx_swap_buffers_(void) {
	if (0 == TX_PERIPH_LEN_) {
		tx_swapbuffer_t_ *aux = TX_USER_;
		TX_USER_ = TX_PERIPH_;
		TX_PERIPH_ = aux;
		return true;
	}
	return false;
}

static void tx_write_buffer_(uint8_t *buffer, size_t size) {
	memcpy(TX_USER_BUFFER_ + TX_USER_LEN_, buffer, size);
	TX_USER_LEN_ += size;
}

static size_t tx_write_(uint8_t *buffer, size_t size, bool allow_swap) {
	size_t size_to_transfer = 0;

	if ((NULL == buffer) || (0 == size)) {
		return size_to_transfer;
	}

	if (0 == TX_USER_AVAILABLE_SIZE_) {
		if (allow_swap) {
			if (tx_swap_buffers_()) {
				size_to_transfer = tx_write_(buffer, size, false);
			}
		}
	} else {
		if (size <= TX_USER_AVAILABLE_SIZE_) {
			tx_write_buffer_(buffer, size);
			size_to_transfer += size;
		} else {
			size_to_transfer += TX_USER_AVAILABLE_SIZE_;
			tx_write_buffer_(buffer, TX_USER_AVAILABLE_SIZE_);
			size -= size_to_transfer;
			if (allow_swap) {
				if (tx_swap_buffers_()) {
					size_to_transfer += tx_write_(buffer + size_to_transfer,
							size, false);
				}
			}
		}
	}

	return size_to_transfer;
}

static void tx_tick_(void) {
	if (0 < tx_.timeout) {
		tx_.timeout--;
	} else {
		tx_send_buffer_();
		if (0 < TX_USER_LEN_) {
			tx_.timeout = TX_TIMEOUT__TICKS_;
		}
	}
}

/********************** external functions definition ************************/

__weak void driver_uart_tx_error_callback(void) {
	return;
}

void driver_uart_tx_init(void) {
	tx_init_();
}

size_t driver_uart_tx(uint8_t *buffer, size_t size) {
	return tx_write_(buffer, size, true);
}

void driver_uart_tx_tick() {
	tx_tick_();
}

/********************** end of file ******************************************/
