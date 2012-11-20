/*
 * =====================================================================================
 *
 *       Filename:  symtable.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/20/2012 00:18:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symtable.h"

#define HASH_INIT_SIZE 307

symtable_t *create_symbol_table(const char* name)
{
  symtable_t *t = (symtable_t*)calloc(1, sizeof(symtable_t));
  strncpy(t->name, name, 32 -1 );
  t->curid = 1;
  hashtable_init(&t->ht, HASH_INIT_SIZE, NULL);
  return t;
}

symbol_t* add_symbol(symtable_t *t, const char* name)
{
  size_t len = strlen(name)+1;
  
  symbol_t *sym = get_symbol(t, name);
  if(sym)
    return sym;
  
  sym = (symbol_t*)calloc(1, sizeof(symbol_t));
  sym->id = t->curid++;
  hashtable_set(&t->ht, name, len, sym);
  return sym;
}

symbol_t *get_symbol(symtable_t *t, const char* name){
  size_t len = strlen(name)+1;
  symbol_t *sym = NULL;
  hashtable_get(&t->ht, name, len, (void**)&sym);
  return sym;
}

