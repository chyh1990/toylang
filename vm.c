/*
 * =====================================================================================
 *
 *       Filename:  vm.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2012 11:23:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "def.h"
#include "vm.h"

struct vm_contex *vm_create()
{
  struct vm_contex* vm = (struct vm_contex*)malloc(sizeof(struct vm_contex));
  memset(vm, 0, sizeof(struct vm_contex));
  vm->stack = (NUM_TYPE*)malloc(sizeof(NUM_TYPE)*VM_MAXSTACK); 
  vm->pc = 0;
  vm->sp = 0;
}


int vm_run(struct vm_contex* vm, struct op_contex* ctx)
{
#define PUSH(x) do{if(vm->sp>=VM_MAXSTACK){fprintf(stderr, "VM: STACK OVERFLOW\n");ret = -2; goto out;}vm->stack[vm->sp++] = (x); }while(0)
#define POP() (vm->sp--)
#define CHECK_EMPTY(x) if(vm->sp<(x)){fprintf(stderr, "VM: STACK EMPTY\n"); ret=-4;goto out;}
#define TOP() (vm->stack[vm->sp-1])
  vm->opctx = ctx;
  int ret = 0;
  NUM_TYPE v = 0;
  while(1){
    int curpc = vm->pc;
    if(curpc >= cache_length(&ctx->cinsn))
      goto out;
    vm->pc++;
    INSN insn = cache_get(&ctx->cinsn, curpc);
    int op = OPCODE(insn);
    switch(op){
      case OP_NOP: break;
      case OP_PUSH_CONST:
        {
          v = cache_get(&ctx->cconst, OP_A(insn));
          PUSH(v);
        }
        break;
      case OP_PUSH_ID:
        PUSH(vm->reg[OP_A(insn)]); 
        break;
      case OP_STORE:
        CHECK_EMPTY(1);
        vm->reg[OP_A(insn)] = TOP();
        POP();
        break;
      case OP_NOT:
      {
        CHECK_EMPTY(1);
        v = TOP();
        POP();
        PUSH(v==0);
      }
        break;

      case OP_CALL:
      {
        if(OP_A(insn)!=1){
          fprintf(stderr, "VM: UNKNOWN CALL %d\n", OP_A(insn));
          ret = -5;
          goto out;
        }
        CHECK_EMPTY(1);
        v = TOP();
        POP();
        printf("VMOUT: %d\n", v);
      }
        break;

      case OP_TEST:
      {
        CHECK_EMPTY(1);
        v = TOP();
        POP();
        if(v!=0)
          vm->pc++;
      }
      break;
      case OP_JMP:
      {
        v = OP_OFFSET(insn);
        vm->pc += v;
      }
      break;

#define CASE_BINOP(op, o) case op: \
      { CHECK_EMPTY(2); \
        int a = TOP(); \
        POP();\
        vm->stack[vm->sp-1] = a o TOP();}break

      CASE_BINOP(OP_ADD, +);
      CASE_BINOP(OP_SUB, -);
      CASE_BINOP(OP_MUL, *);
      CASE_BINOP(OP_DIV, /);

      CASE_BINOP(OP_EQ, ==);
      CASE_BINOP(OP_NE, !=);
      CASE_BINOP(OP_LT, <);
      CASE_BINOP(OP_LE, <=);

#undef CASE_BINOP

      default:
        fprintf(stderr, "ERROR: opcode <%s> 0x%x\n", opcode2string(op), op);
        ret = -1;
        goto out;
    }
    //printf("## %d %d\n",  curpc, vm->sp);
  }
out:
  vm->opctx = NULL;
  return ret;
#undef PUSH
#undef POP
}

void vm_free(struct vm_contex* vm)
{
  if(!vm) return;
  free(vm->stack);
  free(vm);
}

