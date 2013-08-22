///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: keyval.c
// PA Consulting - Lawrence Jones
// TODO - Need to move this into my c-tools repo
//        Also, just a general list will do.
///////////////////////////////////////////////////////////////////////////////

#include "keyval.h"
#include <string.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// MALLOC & INIT
///////////////////////////////////////////////////////////////////////////////

// Allocate and initialise a new keyval pair
KeyVal *keyval_malloc(char *key, char *val)
{
  // malloc a new KeyVal pointer
  KeyVal *kv = malloc(sizeof(KeyVal));
  // Create pointers for the new key/val
  kv->key = malloc(strlen(key));
  kv->val = malloc(strlen(val));
  // Use strcpy to make clones of the given key/val
  strcpy(kv->key, key);
  strcpy(kv->val, val);
  // Ensure the next field is null
  kv->next = NULL;
  // Return the newly allocated KeyVal pointer
  return kv;
}

///////////////////////////////////////////////////////////////////////////////
// KEYVAL MODIFICATIONS
///////////////////////////////////////////////////////////////////////////////

// Given a KeyVal* pointer, will pop the last KeyVal pointer
// from the end of the linked list
KeyVal *keyval_pop(KeyVal **ks)
{
  KeyVal *current = *ks, *res;
  if (current)
  {
    /* See if the current ks specifies a proceding ks */
    if (current->next)
    {
      /* There is a next ks, so see if it is the last one */
      if (!current->next->next)
      {     
        res = current->next;
        current->next = NULL;
        return res;
      }
      /* Move on to the next ks */
      return keyval_pop(&current->next);
    }
    *ks = NULL;
    return current;
  }
  return NULL;
}

// Given a KeyVal* pointer, will remove the first KeyVal from
// the start of the list
KeyVal *keyval_shift(KeyVal **kv)
{
  // If already empty
  if (!*kv)
  {
    // Return NULL
    return NULL;
  }
  // Pick off first keyval
  KeyVal *k = *kv;
  // Adjust initial pointer to next
  *kv = k->next;
  // Return the first
  return k;
}

// Given a KeyVal pointer, will return a pointer to the last
// KeyVal in the list
KeyVal *keyval_peek(KeyVal *kv)
{
  // While there is a next
  while (kv->next)
  {
    // Assign kv the next pointer
    kv = kv->next;
  }
  // Return the kv that has no next (ie, last)
  return kv;
}


///////////////////////////////////////////////////////////////////////////////
// DEALLOC
///////////////////////////////////////////////////////////////////////////////

// Deallocate all the keyval pairs
void keyval_dealloc(KeyVal **_k)
{
  // Create new valid KeyVal pointer
  KeyVal *a = malloc(sizeof(KeyVal)), *b;
  // Assign a's next keyval to the start of the given
  a->next = *_k;
  do {
    // Save a's next into b
    b = a->next;
    // If a is not already freed, then free
    // all it's fields and itself
    if (a)
    {
      free(a->key);
      free(a->val);
      free(a);
    }
    // Do this while the next pointer is not null
  } while ((a = b));
}
