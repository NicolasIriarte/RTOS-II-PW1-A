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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "cmsis_os.h"

#include "uart_driver.h"

/********************** macros and definitions *******************************/

#define RX_BUFFER_SIZE_         (32)    // [B]
#define RX_TICK_PERIOD_         (10)    // [ms]
#define RX_TIMEOUT__MS_         (100)   // [ms]
#define RX_TIMEOUT__TICKS_      (RX_TIMEOUT__MS_ / RX_TICK_PERIOD_) // [ticks]

#define RX_USER_                (rx_.user)
#define RX_USER_BUFFER_         (rx_.user->buffer)
#define RX_USER_LEN_            (rx_.user->len)
#define RX_USER_AVAILABLE_SIZE_ (RX_BUFFER_SIZE_ - RX_USER_LEN_)
#define RX_PERIPH_              (rx_.periph)
#define RX_PERIPH_BUFFER_       (rx_.periph->buffer)
#define RX_PERIPH_LEN_          (rx_.periph->len)
#define RX_PERIPH_ENABLE_       (rx_.periph->enable)

#define UART_HAL_HANDLE_        (&huart3)

/********************** internal data declaration ****************************/

typedef struct
{
  uint8_t buffer[RX_BUFFER_SIZE_];
  size_t len;
  bool enable;
} rx_swapbuffer_t_;

typedef struct
{
  rx_swapbuffer_t_ *user;
  rx_swapbuffer_t_ *periph;
  rx_swapbuffer_t_ swapbuffer[2];
  uint32_t timeout;
  bool error;
} driver_rx_t_;

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static driver_rx_t_ rx_;

/********************** external data definition *****************************/

extern UART_HandleTypeDef huart3;

/********************** internal functions definition ************************/

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  driver_uart_rx_error_callback();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  RX_PERIPH_ENABLE_ = true;
  RX_PERIPH_LEN_ = 0;
}

static void rx_swapbuffer_init_(rx_swapbuffer_t_ *swapbuffer)
{
  swapbuffer->enable = true;
  swapbuffer->len = 0;
}

static void rx_init_(void)
{
  rx_.user = &(rx_.swapbuffer[0]);
  rx_swapbuffer_init_(rx_.user);

  rx_.periph = &(rx_.swapbuffer[1]);
  rx_swapbuffer_init_(rx_.periph);

  rx_.timeout = 0;
}

static void rx_receive_buffer_(void)
{
  if(RX_PERIPH_ENABLE_ && (0 < RX_PERIPH_LEN_))
  {
    HAL_StatusTypeDef satus = HAL_UART_Receive_IT(UART_HAL_HANDLE_, RX_PERIPH_BUFFER_, RX_PERIPH_LEN_);
    switch (satus) {
      case HAL_OK:
        RX_PERIPH_ENABLE_ = false;
        break;
      case HAL_ERROR:
      case HAL_BUSY:
      case HAL_TIMEOUT:
      default:
        driver_uart_rx_error_callback();
        break;
    }
  }
}

static bool rx_swap_buffers_(void)
{
  if(0 == RX_PERIPH_LEN_)
  {
    rx_swapbuffer_t_ *aux = RX_USER_;
    RX_USER_ = RX_PERIPH_;
    RX_PERIPH_ = aux;
    return true;
  }
  return false;
}

static void rx_write_buffer_(uint8_t *buffer, size_t size)
{
  memcpy(RX_USER_BUFFER_ + RX_USER_LEN_, buffer, size);
  RX_USER_LEN_ += size;
}

static size_t rx_write_(uint8_t *buffer, size_t size, bool allow_swap)
{
  size_t size_to_transfer = 0;

  if((NULL == buffer) || (0 == size))
  {
    return size_to_transfer;
  }

  if (0 == RX_USER_AVAILABLE_SIZE_)
  {
    if(allow_swap)
    {
      if(rx_swap_buffers_())
      {
        size_to_transfer = rx_write_(buffer, size, false);
      }
    }
  }
  else
  {
    if (size <= RX_USER_AVAILABLE_SIZE_)
    {
      rx_write_buffer_(buffer, size);
      size_to_transfer += size;
    }
    else
    {
      size_to_transfer += RX_USER_AVAILABLE_SIZE_;
      rx_write_buffer_(buffer, RX_USER_AVAILABLE_SIZE_);
      size -= size_to_transfer;
      if(allow_swap)
      {
        if(rx_swap_buffers_())
        {
          size_to_transfer += rx_write_(buffer + size_to_transfer, size, false);
        }
      }
    }
  }

  return size_to_transfer;
}

static void rx_tick_(void)
{
  if(0 < rx_.timeout)
  {
    rx_.timeout--;
  }
  else
  {
    rx_receive_buffer_();
    if(0 < RX_USER_LEN_)
    {
      rx_.timeout = RX_TIMEOUT__TICKS_;
    }
  }
}

/********************** external functions definition ************************/

__weak void driver_uart_rx_error_callback(void)
{
  return;
}

void driver_uart_rx_init(void)
{
  rx_init_();
}

size_t driver_uart_rx(uint8_t *buffer, size_t size)
{
  return rx_write_(buffer, size, true);
}

void driver_uart_rx_tick()
{
  rx_tick_();
}

/********************** end of file ******************************************/
