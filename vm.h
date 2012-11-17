/*
 * =====================================================================================
 *
 *       Filename:  vm.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2012 11:22:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#ifndef __VM_H
#define __VM_H

#include "vm.h"
#include "opcode.h"

#define VM_MAXSTACK (1<<14)
struct vm_contex{
  NUM_TYPE *stack;
  NUM_TYPE reg[MAX_LOCALVAR];
  struct op_contex *opctx;
  unsigned int pc;
  unsigned int sp;
};

struct vm_contex *vm_create();
void vm_free(struct vm_contex* vm);
int vm_run(struct vm_contex* vm, struct op_contex*);

#endif
