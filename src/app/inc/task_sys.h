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
 * @file   : task_sys.h
 * @date   : Feb 17, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

#ifndef APP_INC_TASK_SYS_H_
#define APP_INC_TASK_SYS_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdio.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/

typedef enum
{
  RED, GREEN,
} LedsColor_t;

typedef enum
{
  ON = 1, OFF = 0,
} LedState_t;

typedef struct
{
  LedsColor_t color;
  LedState_t led_state;
} LedEventType_t;

typedef enum
{
  NONE, SHORT, LONG, STUCK,
} ButtonEventType_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

void
task_SysEvent (void *pvParameters);

void
push_button_event (ButtonEventType_t event);

LedEventType_t
pop_led_event (void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* APP_INC_TASK_SYS_H_ */
/********************** end of file ******************************************/

