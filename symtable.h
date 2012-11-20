/*
 * =====================================================================================
 *
 *       Filename:  symtable.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/20/2012 00:18:56
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#ifndef __SYMTABLE_H
#define __SYMTABLE_H

#include "hashtable.h"
typedef unsigned int SYM_ID;
typedef struct{
  SYM_ID id; 
}symbol_t;

typedef struct{
  char name[32];
  hashtable_t ht;
  SYM_ID curid;
} symtable_t;

symtable_t *create_symbol_table(const char* name);
symbol_t* add_symbol(symtable_t *t, const char* name);
symbol_t *get_symbol(symtable_t *t, const char* name);

#endif

