/*
  Copyright (c) 2002, 2004, Christopher Clark
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  
  * Neither the name of the original author; nor the names of any contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.
  
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

struct hashtable;


/* Example of use:
 *
 *      struct hashtable  *h;
 *      struct some_key   *k;
 *      struct some_value *v;
 *
 *      static uint_t         hash_from_key_fn( void *k );
 *      static int                  keys_equal_fn ( void *key1, void *key2 );
 *
 *      h = create_hashtable(16, hash_from_key_fn, keys_equal_fn);
 *      k = (struct some_key *)     malloc(sizeof(struct some_key));
 *      v = (struct some_value *)   malloc(sizeof(struct some_value));
 *
 *      (initialise k and v to suitable values)
 * 
 *      if (! hashtable_insert(h,k,v) )
 *      {     exit(-1);               }
 *
 *      if (NULL == (found = hashtable_search(h,k) ))
 *      {    printf("not found!");                  }
 *
 *      if (NULL == (found = hashtable_remove(h,k) ))
 *      {    printf("Not found\n");                 }
 *
 */





/* These cannot be inlined because they are referenced as fn ptrs */
uint32_t invirt_hash_ptr(uintptr_t val);
uint32_t invirt_hash_buffer(uint8_t * msg, uint32_t length);



struct hashtable * 
invirt_create_htable(uint32_t   min_size,
		  uint32_t (*hashfunction) (uintptr_t key),
		  int      (*key_eq_fn) (uintptr_t key1, uintptr_t key2));

void invirt_free_htable(struct hashtable * htable, int free_values, int free_keys);

/*
 * returns non-zero for successful insertion
 *
 * This function will cause the table to expand if the insertion would take
 * the ratio of entries to table size over the maximum load factor.
 *
 * This function does not check for repeated insertions with a duplicate key.
 * The value returned when using a duplicate key is undefined -- when
 * the hashtable changes size, the order of retrieval of duplicate key
 * entries is reversed.
 * If in doubt, remove before insert.
 */
int invirt_htable_insert(struct hashtable * htable, uintptr_t key, uintptr_t value);

int invirt_htable_change(struct hashtable * htable, uintptr_t key, uintptr_t value, int free_value);


// returns the value associated with the key, or NULL if none found
uintptr_t invirt_htable_search(struct hashtable * htable, uintptr_t key);

// returns the value associated with the key, or NULL if none found
uintptr_t invirt_htable_remove(struct hashtable * htable, uintptr_t key, int free_key);

uint32_t invirt_htable_count(struct hashtable * htable);

// Specialty functions for a counting hashtable 
int invirt_htable_inc(struct hashtable * htable, uintptr_t key, uintptr_t value);
int invirt_htable_dec(struct hashtable * htable, uintptr_t key, uintptr_t value);


/* ************ */
/* ITERATOR API */
/* ************ */


/*****************************************************************************/
/* This struct is only concrete here to allow the inlining of two of the
 * accessor functions. */
struct hashtable_iter {
    struct hashtable * htable;
    struct hash_entry * entry;
    struct hash_entry * parent;
    uint32_t index;
};


struct hashtable_iter * invirt_htable_create_iter(struct hashtable * htable);

void invirt_htable_free_iter(struct hashtable_iter * iter);

/* - return the value of the (key,value) pair at the current position */
uintptr_t invirt_htable_get_iter_key(struct hashtable_iter * iter);

/* value - return the value of the (key,value) pair at the current position */
uintptr_t invirt_htable_get_iter_value(struct hashtable_iter * iter);




/* returns zero if advanced to end of table */
int invirt_htable_iter_advance(struct hashtable_iter * iter);

/* remove current element and advance the iterator to the next element
 *          NB: if you need the value to free it, read it before
 *          removing. ie: beware memory leaks!
 *          returns zero if advanced to end of table 
 */
int invirt_htable_iter_remove(struct hashtable_iter * iter, int free_key);


/* search - overwrite the supplied iterator, to point to the entry
 *          matching the supplied key.
 *          returns zero if not found. */
int invirt_htable_iter_search(struct hashtable_iter * iter, struct hashtable * htable, uintptr_t key);



#ifdef __cplusplus
}
#endif


#endif
