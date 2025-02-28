/*
 * Copyright (c) 2010, Kelvin Lawson. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. No personal names or organizations' names associated with the
 *    Atomthreads project may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ATOMTHREADS PROJECT AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ATOM_H
#define __ATOM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "atomtimer.h"
#include "atomport.h"

/* Data types */

/* Forward declaration */
struct atom_tcb;
//任务控制块
typedef struct atom_tcb
{
    /*
     * Thread's current stack pointer. When a thread is scheduled
     * out the architecture port can save its stack pointer here.
     */
    POINTER sp_save_ptr;

    /* Thread's port specific private data */
#if defined(THREAD_PORT_PRIV)
    THREAD_PORT_PRIV;
#endif

    /* Thread priority (0-255) */
    uint8_t priority;

    /* Thread entry point and parameter */
    void (*entry_point)(uint32_t); /* Thread 入口函数 */
    uint32_t entry_param;

    /* Queue pointers */
    struct atom_tcb *prev_tcb;    /* Previous TCB in doubly-linked TCB list */
    struct atom_tcb *next_tcb;    /* Next TCB in doubly-linked list */

    /* Suspension data */
    uint8_t suspended;            /* TRUE if task is currently suspended */
    uint8_t suspend_wake_status;  /* Status returned to woken suspend calls */
    ATOM_TIMER *suspend_timo_cb;  /* Callback registered for suspension timeouts */
    uint8_t terminated;           /* TRUE if task is being terminated (run to completion) */

    /* Details used if thread stack-checking is required */
#ifdef ATOM_STACK_CHECKING
    POINTER stack_bottom;         /* Pointer to bottom of stack allocation */
    uint32_t stack_size;          /* Size of stack allocation in bytes */
#endif

} TCB_t;


/* Global data */
extern TCB_t *tcbReadyQ;
extern uint8_t atomOSStarted;


/* Constants */
#define TRUE                    1
#define FALSE                   0

/* Error values */

#define ATOM_OK                 0
#define ATOM_ERROR              1
#define ATOM_TIMEOUT            2
#define ATOM_WOULDBLOCK         3
#define ATOM_ERR_CONTEXT        200
#define ATOM_ERR_PARAM          201
#define ATOM_ERR_DELETED        202
#define ATOM_ERR_OVF            203
#define ATOM_ERR_QUEUE          204
#define ATOM_ERR_TIMER          205
#define ATOM_ERR_NOT_FOUND      206
#define ATOM_ERR_OWNERSHIP      207

/* Idle thread priority (lowest) */
#define IDLE_THREAD_PRIORITY    0
/* Maximum number of priorities 最高优先级*/
#define configMAX_PRIORITIES    32
/* 允许多个任务共享一个优先级 */
#define configUSE_TIME_SLICING 1

/* Function prototypes功能原型 */
extern uint8_t atomOSInit (void *idle_thread_stack_bottom, uint32_t idle_thread_stack_size, uint8_t idle_thread_stack_check);
extern void atomOSStart (void);

extern void atomSched (uint8_t timer_tick);

extern void atomIntEnter (void);
extern void atomIntExit (uint8_t timer_tick);

extern uint8_t tcbEnqueuePriority (TCB_t **tcb_queue_ptr, TCB_t *tcb_ptr);
extern TCB_t *tcbDequeueHead (TCB_t **tcb_queue_ptr);
extern TCB_t *tcbDequeueEntry (TCB_t **tcb_queue_ptr, TCB_t *tcb_ptr);
extern TCB_t *tcbDequeuePriority (TCB_t **tcb_queue_ptr, uint8_t priority);

extern TCB_t *atomCurrentContext (void);

extern uint8_t atomTaskCreate (TCB_t *tcb_ptr, uint8_t priority, void (*entry_point)(uint32_t), uint32_t entry_param, void *stack_bottom, uint32_t stack_size, uint8_t stack_check);
extern uint8_t atomThreadStackCheck (TCB_t *tcb_ptr, uint32_t *used_bytes, uint32_t *free_bytes);
//如果想要退出任务，需要调用atomThreadExit()函数。任务函数一般不允许跳出循环，如果一定要跳出循环的话在跳出循环以后一定要调用
//函数 vTaskDelete(NULL)删除此任务！
extern uint8_t atomTaskDelete (void );
extern void archContextSwitch (TCB_t *old_tcb_ptr, TCB_t *new_tcb_ptr);
extern void archThreadContextInit (TCB_t *tcb_ptr, void *stack_top, void (*entry_point)(uint32_t), uint32_t entry_param);
extern void archFirstThreadRestore(TCB_t *new_tcb_ptr);
//系统时钟周期处理程序。
extern void atomTimerTick (void);

#ifdef __cplusplus
}
#endif

#endif /* __ATOM_H */
