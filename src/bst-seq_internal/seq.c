/*
 * File:
 *   .c
 * Author(s):
 * Description:
 */

#include "seq.h"


node_t** 
search(node_t** root, skey_t key) 
{
  node_t** node = root;
  while (unlikely(*node != NULL))
    {
      sval_t node_key = (*node)->key;
      if (key < node_key)
	{
	  node = (node_t**) &(*node)->left;
	}
      else if (key > node_key)
	{
	  node = (node_t**) &(*node)->right;
	}
      else
	{
	  break;
	}
    }
  return node;
}


void
seq_delete_node(node_t** node)
{
  node_t* old_node = *node;
  if ((*node)->left == NULL) 
    {
      *node = (node_t*) (*node)->right;
      node_delete(old_node);
    } 
  else if ((*node)->right == NULL) 
    {
      *node = (node_t*) (*node)->left;
      node_delete(old_node);
    } 
  else 
    {
      /* delete node with two children */
      node_t* pred = (node_t*) (*node)->left;
      node_t* pred_pred = *node;
      while(pred->right != NULL)
	{
	  pred_pred=pred;
	  pred = (node_t*) pred->right;
	}

      skey_t tmp = pred->key;
      (*node)->key = tmp;

      if(pred_pred == *node)
	{
	  (*node)->left = pred->left;
	}
      else
	{
	  pred_pred->right = pred->left;
	}

      node_delete(pred);
    }
}


sval_t
seq_delete(intset_t* set, skey_t key)
{
  node_t** node = search(&set->head, key);
  if (*node == NULL)
    {
      return 0;
    }
  
  seq_delete_node(node);

  return 1;
}

sval_t
seq_find(intset_t* set, skey_t key) 
{
  node_t* node = set->head;
  while (unlikely(node != NULL))
    {
      skey_t node_key = (node)->key;
      if (key < node_key)
	{
	  node = (node_t*) node->left;
	}
      else if (key > node_key)
	{
	  node = (node_t*) node->right;
	}
      else
	{
	  break;
	}
    }
  return (node != NULL);
}

int
seq_insert(intset_t* set, skey_t key, sval_t val) 
{
  node_t** node = search(&set->head, key);
  if (*node == NULL) 
    {
      *node = new_node(key, val, 0);
      return 1;
    }

  return 0;
}
