/*
 * Implementation of a lock-free list-based set based on Michael's 
 * modification of Harris' original algorithm using hazard pointers.
 *
 * Follows the pseudocode given in :
 *  M. M. Michael. Hazard Pointers: Safe Memory Reclamation for Lock-Free 
 *  Objects. IEEE TPDS (2004) IEEE Transactions on Parallel and Distributed 
 *  Systems 15(8), August 2004.
 *
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
#include "list.h"
// #include "test.h"
//#include "arch/atomic.h"
#include "atomic_ops_if.h"
#include "utils.h"
#include "ssalloc.h"
#include "node.h"
// #include "util.h"
// #include "backoff.h"
#include <stdio.h>

extern __thread smr_data_t sd;


struct list_aux_data {
    /* 
     * Private per-thread node pointers for lock-free linked list:
     *   prev : holds the old address of cur for CAS operations 
     *   cur  : the node we want to do something with 
     *          (delete, insert after, etc.)
     *   next : the successor of cur
     */
    node_t **prev;
    node_t *next;
    node_t *cur;
};

typedef ALIGNED(CACHE_LINE_SIZE) struct list_aux_data list_aux_data_t;

__thread list_aux_data_t list_data;

struct list {
    node_t *list_head;
};

void list_init(struct list **l) {
    // IGOR OANA potential problem here: allocating different size with 0 allocator
    *l = (struct list *)ssalloc_aligned_alloc(0, CACHE_LINE_SIZE, sizeof(struct list));
    (*l)->list_head = NULL;
}

void list_destroy(struct list **l) {
    ssfree_alloc(0,*l);
}

/* 
 * Takes advantage of pointers always lying on 4-byte boundaries
 * since words are 32-bits on x86. Hence we can mark the low-order 
 * two bytes of a pointer. We only mark the low-order one.
 *
 * Postcondition:
 *   1. If list is empty, prev=head, cur=NULL, return false.
 *   2. If we find a q s. th. q->key >= key (q has the *smallest* key <= key), 
 *      then prev = &cur, cur = q, an we return (q->key == key).
 *   3. If the key isn't in the list, *prev=(last node), cur=NULL, next=null, 
 *      return false.
 *
 * NOTE: It is this postcondition, as it applies to prev, that requires us to 
 *       use two levels of indirection for the head: we need prev to point to 
 *       the right thing in the calling function, not a local head variable.
 */
int find (node_t **head, long key)
{
    node_t **prev, *cur, *next = NULL;
    int base = sd.thread_index*K;
    int off = 0;

 try_again:
    prev = head;
    
    for (cur = *prev; cur != NULL; cur = next) {
        
        /* Protect cur with a hazard pointer. */
        HP[base+off].p = cur;
        // memory_barrier();
        MEM_BARRIER;
        if (*prev != cur) goto try_again;
        
        next = cur->next;
    
        /* If the bit is marked... */
        if ((uint64_t)next & 1) {
            
            /* ... update the link and retire the element. */
            if (!CAS_PTR_bool(prev, cur, (node_t *)((uint64_t)next-1))) {
                // IGOR OANA potential source of problems
                // backoff_delay();
                goto try_again;
            }
            free_node_later(cur, 1);
            next = (node_t *)((uint64_t)next-1);

        } else {
            
            if (*prev != cur) goto try_again;
            
            if (cur->key >= 10000) {
                fprintf(stderr, "touched illegal node in find. Type:%d\n", cur->key - 10000);
            }
            
            if (cur->key >= key) {
                list_data.cur = cur;
                list_data.prev = prev;
                list_data.next = next;
                return (cur->key == key);
            }
            
            prev = &cur->next;
            off = !off;
        }
    }

    list_data.cur = cur;
    list_data.prev = prev;
    list_data.next = next;
    return (0);
}

int insert(struct list *l, long key)
{
    node_t **head = &l->list_head;
    //node_t *n = new_node();

    node_t *n = ssalloc_alloc(0, sizeof(node_t));


    // backoff_reset();

    while (1) {
        if (find(head, key)) {
            ssfree_alloc(0, n);
            return (0);
        }
        n->key = key;
        n->next = list_data.cur;
        // write_barrier();
        MEM_BARRIER;
        if (CAS_PTR_bool(list_data.prev, list_data.cur, n)) {
            return (1);
        }
        // } else {
        //     backoff_delay();
        // }
    }
}

int delete(struct list *l, long key)
{
    node_t **head = &l->list_head;
    // uint64_t myTID = getTID();
    
    // backoff_reset();

    while (1) {
        /* Try to find the key in the list. */
        if (!find(head, key)) {
            return (0);
        }
        
        /* Mark if needed. */
        if (!CAS_PTR_bool(&(list_data.cur->next), 
                        list_data.next, 
                        (node_t *)((uint64_t)list_data.next+1))) {
            // backoff_delay();
            continue; /* Another thread interfered. */
        }
        
        // write_barrier();
        MEM_BARRIER;
        if (CAS_PTR_bool(list_data.prev, 
               list_data.cur, list_data.next)) /* Unlink */
            free_node_later(list_data.cur, 2); /* Reclaim */
        /* 
         * If we want to revent the possibility of there being an 
         * unbounded number of unmarked nodes, add "else _find(head,key)."
         * This is not necessary for correctness.
         */
        return (1);
    }
}

int search (struct list *l, long key)
{
    node_t **prev, *cur, *next;
    int base = sd.thread_index*K;
    int off = 0;

    // backoff_reset();
    
 try_again:
    prev = &l->list_head;
    
    for (cur = *prev; cur != NULL; cur = next) {
        
        /* Protect cur with a hazard pointer. */
        HP[base+off].p = cur;
        // memory_barrier();
        MEM_BARRIER;
        if (*prev != cur) goto try_again;
        
        next = cur->next;
    
        /* If the bit is marked... */
        if ((uint64_t)next & 1) {
            
            /* ... update the link and retire the element. */
            if (!CAS_PTR_bool(prev, cur, (node_t *)((uint64_t)next-1))) {
                // backoff_delay();
                goto try_again;
            }
            free_node_later(cur, 3);
            next = (node_t *)((uint64_t)next-1);

        } else {
            long ckey = cur->key;
            if (*prev != cur) goto try_again;

            if (cur->key >= 10000) {
                fprintf(stderr, "touched illegal node in search. Type:%d\n", cur->key - 10000);
            }

            if (ckey >= key) {
                return (ckey == key);
            }
            
            prev = &cur->next;
            off = !off;
        }
    }

    return (0);
}

int size(struct list *l) {
    node_t *n = l->list_head;
    int count = 0;
    while (n != NULL) {
        count++;
        n = n->next;
    }

    return count;
}
