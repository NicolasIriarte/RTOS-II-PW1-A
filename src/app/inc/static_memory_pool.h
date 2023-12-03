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
 * @file   : app.h
 * @date   : Feb 17, 2023
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 * @version	v1.0.0
 */

#ifndef APP_INC_STATIC_MEMORY_POOL_H_
#define APP_INC_STATIC_MEMORY_POOL_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdint.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/
/**
 *
 * @brief A simple memory pool.
 *
 * @details A simple memory pool. The pool is created with a static memory
 *          allocated by the user. The pool is a FIFO, the first element pushed
 *          is the first element popped. The pool is not thread safe.
 *
 * @note The pool is not thread safe.
 *
 */

typedef struct SMemoryPool_t {
	void *pool;            // Static memory pointer
	uint32_t block_size;   // Size of each element on the pool
	uint32_t num_elements; // Size of elements which can be stored on the pool
	uint32_t num_used_elements; // Number of elements used on the pool
	void *head;                 // Pointer to the first element of the pool
	void *tail;                 // Pointer to the last element of the pool
} SMemoryPool_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

/**
 * @brief Initialize the memory pool.
 *
 * @details Initialize the memory pool. This function must be called before any
 *          other usage.
 *
 * @param self           The pointer to the SMemoryPool_t object.
 * @param static_memory  A pointer to the static memory to be used by the pool.
 * @param block_size     The size of each element on the pool.
 * @param num_elements   The number of elements which can be stored on the pool.
 *
 * @return void
 */

void SMemoryPool_init(SMemoryPool_t *self, void *static_memory,
		uint32_t block_size, uint32_t num_elements);

/**
 * @brief Push an element to the pool.
 *
 * @details The element is pushed to the tail of the pool. If the pool is full
 *          the element is not pushed and the function returns NULL. The element
 *          is copied to the pool.
 *
 * @param self  The pointer to the SMemoryPool_t object.
 * @param data  A pointer to the data to be pushed.
 *
 * @return a pointer to where the element was pushed, or NULL if the element
 *         couldn't be pushed.
 */
void* SMemoryPool_push(SMemoryPool_t *self, void *data);

/**
 * @brief Pop an element from the pool.
 *
 * @details Pop an element from the pool. If the pool is empty the function
 *          returns NULL.
 *
 * @param self The pointer to the SMemoryPool_t object.
 *
 * @return a pointer to the element popped, or NULL if the pool is empty.
 */

void* SMemoryPool_pop(SMemoryPool_t *self);
/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* APP_INC_STATIC_MEMORY_POOL_H_ */
/********************** end of file ******************************************/

