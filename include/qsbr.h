/* 
 * QSBR Functions.
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
 
#ifndef __RCU_H
#define __RCU_H

#include "mr.h"

#define FUZZY 0
#define NOT_FUZZY 1

#define N_EPOCHS 3

#define QUIESCENCE_THRESHOLD 1000

#define MAX_EXIT_RETRIES 100 * qad.nthreads

void quiescent_state (int flag);

void free_node_later(void * q);

#endif
