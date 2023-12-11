#include "uart_driver_rx.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"
#include "main.h"

#include "uart_driver_rx.h"

/********************** macros and definitions *******************************/

#define RX_BUFFER_SIZE_ (32) // [B]
#define RX_NUMBER_OF_BUFFERS_ (3)
#define RX_TICK_PERIOD_ (10)                                   // [ms]
#define RX_TIMEOUT__MS_ (100)                                  // [ms]
#define RX_TIMEOUT__TICKS_ (RX_TIMEOUT__MS_ / RX_TICK_PERIOD_) // [ticks]

#define UART_HAL_HANDLE_ (&huart3)

/********************** internal data declaration ****************************/

typedef struct {
	uint8_t buffer[RX_BUFFER_SIZE_];
	bool is_free;
} rx_buffer_t_;

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static rx_buffer_t_ rx_[RX_NUMBER_OF_BUFFERS_];

static uint32_t timeout_ = RX_TIMEOUT__TICKS_;

/********************** external data definition *****************************/

extern UART_HandleTypeDef huart3;
#define USED_UART_ID USART3

/********************** internal functions definition ************************/

static rx_buffer_t_* get_free_buffer() {
	for (size_t i = 0; i < RX_NUMBER_OF_BUFFERS_; i++) {
		if (rx_[i].is_free) {
			rx_[i].is_free = false;
			return &(rx_[i]);
		}
	}
	return NULL; // No free buffer available
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	driver_uart_rx_error_callback();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance != USED_UART_ID) {
		return;
	}

	rx_buffer_t_ *free_buffer = NULL;

	while (true) {
		free_buffer = get_free_buffer();

		if (free_buffer != NULL) {
			break;
		}
	}

	// Start a new UART receive operation
	HAL_StatusTypeDef status = HAL_UART_Receive_IT(huart, free_buffer->buffer,
			RX_BUFFER_SIZE_);

	switch (status) {
	case HAL_OK:
		break;
	case HAL_ERROR:
	case HAL_BUSY:
	case HAL_TIMEOUT:
	default:
		driver_uart_rx_error_callback();
		break;
	}
}

static void rx_init_(void) {
	for (int i = 0; i < RX_NUMBER_OF_BUFFERS_; i++) {
		rx_[i].is_free = true;
	}

	/* HAL_UART_RxCpltCallback(&huart3); */
	rx_[0].is_free = false;
	HAL_UART_Receive_IT(&huart3, rx_[0].buffer, RX_BUFFER_SIZE_);
}

static void process_used_buffers(void) {
	for (int i = 0; i < RX_NUMBER_OF_BUFFERS_; i++) {
		if (!rx_[i].is_free) {
			size_t size = strlen((char*) rx_[i].buffer);
			driver_uart_rx(rx_[i].buffer, size);
			rx_[i].is_free = true;
		}
	}
}

static void rx_tick_(void) {
	if (0 < timeout_) {
		timeout_--;
	} else {
		process_used_buffers();
		timeout_ = RX_TIMEOUT__TICKS_;
	}
}

/********************** external functions definition ************************/

__weak void driver_uart_rx_error_callback(void) {
	return;
}

void driver_uart_rx_init(void) {
	rx_init_();
}

void driver_uart_rx_tick() {
	rx_tick_();
}

/********************** end of file ******************************************/
