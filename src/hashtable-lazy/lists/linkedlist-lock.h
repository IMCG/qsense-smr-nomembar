/*
 * File:
 *   linkedlist-lock.c
 * Author(s):
 *   Vincent Gramoli <vincent.gramoli@epfl.ch>
 * Description:
 *   Lock-based linked list implementation of an integer set
 *
 * Copyright (c) 2009-2010.
 *
 * linkedlist-lock.c is part of Synchrobench
 * 
 * Synchrobench is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _H_LINKEDLIST_LOCK_
#define _H_LINKEDLIST_LOCK_

#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>

#include <atomic_ops.h>
#include "lock_if.h"

#include "common.h"
#include "utils.h"
#include "measurements.h"
#include "ssalloc.h"
#include "ssmem.h"

#define DEFAULT_LOCKTYPE	  	2
#define DEFAULT_ALTERNATE		0
#define DEFAULT_EFFECTIVE		1

static volatile int stop;
extern __thread ssmem_allocator_t* alloc;

#define ALGO_TYPE                   algo_type

typedef ALIGNED(32) volatile struct node_l
{
  skey_t key;
  sval_t val;
  volatile struct node_l* next;
  volatile uint8_t marked;
#if !defined(LL_GLOBAL_LOCK)
  volatile ptlock_t lock;
#endif
} node_l_t;

STATIC_ASSERT(sizeof(node_l_t) % 32 == 0, "sizeof(node_l_t) should be 32bytes aligned");

typedef struct intset_l 
{
  node_l_t* head;
#if defined(LL_GLOBAL_LOCK)
  volatile ptlock_t* lock;
#endif
}intset_l_t;

node_l_t* new_node_l(skey_t key, sval_t val, node_l_t* next, int initializing);
void bucket_set_init_l(intset_l_t* set, ptlock_t* lock);
void set_delete_l(intset_l_t* set);
int set_size_l(intset_l_t* set);
void node_delete_l(node_l_t* node);

#endif	/* _H_LINKEDLIST_LOCK_ */
