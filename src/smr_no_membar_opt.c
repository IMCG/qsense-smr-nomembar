/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright (c) Thomas E. Hart.
 */

#include "smr.h"
//#include "arch/atomic.h"
#include "ssalloc.h"
// #include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include "linkedlist-smr-no-membar-harris_opt/linkedlist.h"
#include "sleeper_threads.h"

// IGOR: SSALLOC allocator convention:
// 0 is for actual nodes
// 1 is for m_nodes

__thread smr_data_t sd;

uint8_t is_old_enough(mr_node_t* n);
void do_nothing(){}

void mr_init_global(uint8_t nthreads){
  /* Allocate HP array. Over-allocate since the parent has pid 32. */
  // HP = (hazard_pointer *)mapmem(sizeof(hazard_pointer) * K*(MAX_THREADS+1));
  HP = (hazard_pointer_t *)malloc(sizeof(hazard_pointer_t) * K*nthreads);
  if (HP == NULL) {
    fprintf(stderr, "SMR mr_init: out of memory\n");
    exit(-1);
  }

  /* Initialize the hazard pointers. */
  int i;
  for (i = 0; i < K*(nthreads); i++)
    HP[i].p = NULL;

  init_sleeper_threads(nthreads, do_nothing);

}

void mr_init_local(uint8_t thread_index, uint8_t nthreads){
  
  sd.rlist = (double_llist_t*) malloc(sizeof(double_llist_t));
  init(sd.rlist);

  sd.rcount = 0;
  sd.thread_index = thread_index;
  sd.nthreads = nthreads;
  sd.plist = (void **) malloc(sizeof(void *) * K * sd.nthreads);
  sd.free_calls = 0;
}

void mr_thread_exit()
{
    int i;
    
    for (i = 0; i < K; i++)
        HP[K * sd.thread_index + i].p = NULL;
    
    while (sd.rcount > 0) {
        scan();
        sched_yield();
    }
}

void mr_exit_global(){
    //join sleeper threads
    join_sleeper_threads();
}

void mr_reinitialize()
{
}

/* 
 * Comparison function for qsort.
 *
 * We just need any total order, so we'll use the arithmetic order 
 * of pointers on the machine.
 *
 * Output (see "man qsort"):
 *  < 0 : a < b
 *    0 : a == b
 *  > 0 : a > b
 */

int compare (const void *a, const void *b)
{
  return ( *(uint64_t*)a - *(uint64_t*)b );
}

/* Debugging function. Leave it around. */
inline int ssearch(void **list, size_t size, void *key) {
    int i;
    for (i = 0; i < size; i++) {
      if (list[i] == key) {
        return 1;
      }
    }
    return 0;
}

void scan()
{
    /* Iteratation variables. */
    mr_node_t *cur;
    int i;
    uint64_t freed = 0;

    /* List of SMR callbacks. */
    mr_node_t *tmplist;

    /* List of hazard pointers, and its size. */
    void **plist = sd.plist;
    size_t psize;

    //write_barrier();
     MEM_BARRIER;

    /* Stage 1: Scan HP list and insert non-null values in plist. */
    psize = 0;
    for (i = 0; i < H; i++) {
      if (HP[i].p != NULL){
        plist[psize] = HP[i].p;
        psize++;
      }
    }

    
    /* Stage 2: Sort the plist. */
    /* OANA For now, just do linear search*/
    //qsort(plist, psize, sizeof(void *), compare);

    // start iterating through rlist from the last (oldest) node
    cur = sd.rlist->tail;
    while (true) {
      if (cur == NULL || !is_old_enough(cur)) {
        break;
      } 

      // here cur is not NULL and is old enough
      // if cur is not protected by a HP, remove it
      if (!ssearch(plist, psize, cur->actual_node)) {
        //remove_from_tail(sd.rlist);
        remove_node(sd.rlist, cur);
        sd.rcount--;
        ((node_t *)(cur->actual_node))->key = 10000;
        ssfree_alloc(0, cur->actual_node);
        ssfree_alloc(1, cur);  
        freed++;
      }

      // go to the next oldest node
      cur = cur->mr_prev;
    }

    printf("Freed: %llu\n", freed);
}

void free_node_later(void *n)
{
    mr_node_t* wrapper_node = ssalloc_alloc(1, sizeof(mr_node_t));
    wrapper_node->actual_node = n;
    // Create timestamp in mr node
    gettimeofday(&(wrapper_node->created), NULL);

    // wrapper_node->mr_next = sd.rlist->head;
    // sd.rlist->head = wrapper_node;
    add_to_head(sd.rlist, wrapper_node);
    sd.rcount++;
    sd.free_calls++;

    if (sd.free_calls >= R) {
        sd.free_calls = 0;
        scan();
    }
}

uint8_t is_old_enough(mr_node_t* n) {
    struct timeval now;
    gettimeofday(&now, NULL);
    uint64_t msec; 
    msec = (now.tv_sec - n->created.tv_sec) * 1000; 
    msec += (now.tv_usec - n->created.tv_usec) / 1000; 
    return (msec >= (SLEEP_AMOUNT + MARGIN)); 
}