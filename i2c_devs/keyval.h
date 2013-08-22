///////////////////////////////////////////////////////////////////////////////
// Raspberry Pi GPIO Interface
// ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
// File: keyval.h
// PA Consulting - Lawrence Jones
///////////////////////////////////////////////////////////////////////////////

#ifndef KEYVAL_MAP_INC
#define KEYVAL_MAP_INC

///////////////////////////////////////////////////////////////////////////////
// TYPEDEFS
///////////////////////////////////////////////////////////////////////////////

// Define generic KeyVal pair for config
typedef struct KeyVal KeyVal;
struct KeyVal {
  char *key;
  char *val;
  KeyVal *next;
};

///////////////////////////////////////////////////////////////////////////////
// INTERFACE - FUNCTION STUBS
///////////////////////////////////////////////////////////////////////////////

// TODO - Implement detection of infinite loops (woops)
// Initialise a new KeyVal with a given key value pair
KeyVal *keyval_malloc   (  char*     key, 
                           char*     val  );
// Given a KeyVal* pointer, will pop the last KeyVal pointer
// from the end of the linked list
KeyVal *keyval_pop      (  KeyVal**  kv  );
// Given a KeyVal* pointer, will remove the first KeyVal from
// the start of the list
KeyVal *keyval_shift    (  KeyVal**  kv  );
// Given a KeyVal pointer, will return a pointer to the last
// KeyVal in the list
KeyVal *keyval_peek     (  KeyVal*   kv  );
// Deallocate the entire KeyVal list
void    keyval_dealloc  (  KeyVal**  _k  );

#endif
