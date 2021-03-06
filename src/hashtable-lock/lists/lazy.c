/*
 * File:
 *   lazy.c
 * Author(s):
 *   Vincent Gramoli <vincent.gramoli@epfl.ch>
 * Description:
 *   Lazy linked list implementation of an integer set based on Heller et al. algorithm
 *   "A Lazy Concurrent List-Based Set Algorithm"
 *   S. Heller, M. Herlihy, V. Luchangco, M. Moir, W.N. Scherer III, N. Shavit
 *   p.3-16, OPODIS 2005
 *
 * Copyright (c) 2009-2010.
 *
 * lazy.c is part of Synchrobench
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

#include "lazy.h"

/*
 * Checking that both curr and pred are both unmarked and that pred's next pointer
 * points to curr to verify that the entries are adjacent and present in the list.
 */
static inline int
parse_validate(node_l_t* pred, node_l_t* curr) 
{
  return (!pred->marked && (curr == NULL || (!curr->marked)) && (pred->next == curr));
}

sval_t
parse_find(intset_l_t *set, skey_t key)
{
  node_l_t* curr = set->head;
  while (curr != NULL && (curr->key < key || curr->marked))
    {
      curr = curr->next;
    }

  sval_t res = 0;
  if (curr != NULL && curr->key == key)
    {
      res = curr->val;
    }
  
  return res;
}

int
parse_insert(intset_l_t *set, skey_t key, sval_t val)
{
  node_l_t *curr, *pred, *newnode;
  int result = -1;
	
  do
    {
      pred = set->head;
      curr = pred->next;
      while (curr != NULL && curr->key < key) 
	{
	  pred = curr;
	  curr = curr->next;
	}

      if (curr != NULL && curr->key == key && !curr->marked)
	{
	  return false;
	}

      GL_LOCK(set->lock);		/* when GL_[UN]LOCK is defined the [UN]LOCK is not ;-) */
      PREFETCHW_LOCK(curr);
      LOCK(ND_GET_LOCK(pred));
      if (curr != NULL)
	{
	  LOCK(ND_GET_LOCK(curr));
	}
      if (parse_validate(pred, curr))
	{
	  result = (curr == NULL) || (curr->key != key);
	  if (result) 
	    {
	      newnode = new_node_l(key, val, curr, 0);
	      pred->next = newnode;
	    } 
	}
      GL_UNLOCK(set->lock);
      if (curr != NULL)
	{
	  UNLOCK(ND_GET_LOCK(curr));
	}
      UNLOCK(ND_GET_LOCK(pred));
    }
  while (result < 0);
  return result;
}

/*
 * Logically remove an element by setting a mark bit to 1 
 * before removing it physically.
 */
sval_t
parse_delete(intset_l_t *set, skey_t key)
{
  node_l_t *pred, *curr;
  sval_t result = 0;
  int done = 0;

  do
    {
      pred = set->head;
      curr = pred->next;
      while (curr != NULL && curr->key < key)
	{
	  pred = curr;
	  curr = curr->next;
	}

      if (curr != NULL && curr->key != key && !curr->marked)
	{
	  return false;
	}

      GL_LOCK(set->lock);		/* when GL_[UN]LOCK is defined the [UN]LOCK is not ;-) */
      PREFETCHW_LOCK(curr);
      LOCK(ND_GET_LOCK(pred));
      if (curr != NULL)
	{
	  LOCK(ND_GET_LOCK(curr));
	}

      if (parse_validate(pred, curr))
	{
	  if (curr != NULL && key == curr->key)
	    {
	      result = curr->val;
	      node_l_t* c_nxt = curr->next;
	      curr->marked = 1;
	      pred->next = c_nxt;
#if GC == 1
	      ssmem_free(alloc, curr);
#endif
	    }
	  done = 1;
	}

      GL_UNLOCK(set->lock);
      if (curr != NULL)
	{
	  UNLOCK(ND_GET_LOCK(curr));
	}
      UNLOCK(ND_GET_LOCK(pred));
    }
  while (!done);
  return result;
}
