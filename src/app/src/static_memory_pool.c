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
#include "static_memory_pool.h"
#include "memory.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

void SMemoryPool_init(SMemoryPool_t *self, void *static_memory,
		uint32_t block_size, uint32_t num_elements) {
	self->pool = static_memory;
	self->block_size = block_size;
	self->num_elements = num_elements;
	self->num_used_elements = 0;
	self->head = NULL;
	self->tail = NULL;
}

void* SMemoryPool_push(SMemoryPool_t *self, void *data) {
	if (self->num_used_elements >= self->num_elements) {
		return NULL;
	}

	void *ret = NULL;

	portENTER_CRITICAL(); //Enter on critical section
	if (self->head == NULL) {
		self->head = self->pool;
		self->tail = self->pool;
	} else {
		// If we are out of bound but we have free space, the last element is the
		// first block of the pool.
		if (self->tail + self->block_size
				>= self->pool + self->num_elements * self->block_size) {
			self->tail = self->pool;
		} else {
			self->tail += self->block_size;
		}
	}

	// Copy data to the pool
	memcpy(self->tail, data, self->block_size);

	ret = self->tail;
	self->num_used_elements++;

	portEXIT_CRITICAL(); //Exit from critical section
	return ret;
}

void* SMemoryPool_pop(SMemoryPool_t *self) {
	if (self->num_used_elements == 0) {
		return NULL;
	}

	portENTER_CRITICAL(); //Enter on critical section
	void *ret = self->head;

	if (self->head + self->block_size
			>= self->pool + self->num_elements * self->block_size) {
		self->head = self->pool;
	} else {
		self->head += self->block_size;
	}

	self->num_used_elements--;

	portEXIT_CRITICAL(); //Exit from critical section
	return ret;
}

/********************** end of file ******************************************/
