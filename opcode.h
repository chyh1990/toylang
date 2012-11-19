/*
 * =====================================================================================
 *
 *       Filename:  opcode.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2012 07:49:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#ifndef __OPCODE_H
#define __OPCODE_H

typedef enum{
  OP_NOP,
  OP_PUSH_CONST,
  OP_PUSH_ID,

  OP_STORE,

  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,

  OP_EQ,
  OP_NE,
  OP_LT,
  OP_LE,
  OP_NOT,
  
/* if true, pop, pc++ */
  OP_TEST,

  OP_JMP,
  OP_CALL,

  OP_MEMBER_CNT
}OpCode;

typedef unsigned int INSN;

#define OPCODE(x) ((x) & 0xff)
#define OP_A(x) (((x)>>8) & 0xffffff)
#define OP_OFFSET(x) ( (int)(((int)x >= 0)?(x)>>8:((x)>>8)|0xff000000) )
#define NEW_INSN(op,a) ( ((op)&0xff) | ((a)<<8))


#define CACHE_TYPE unsigned int
typedef struct {
  CACHE_TYPE* head;
  unsigned int cap;
  unsigned int len;
} cache_t;

static inline void cache_init(cache_t *c)
{
  c->head = (CACHE_TYPE*)malloc(sizeof(CACHE_TYPE)*4);
  c->cap = 4;
  c->len = 0;
}

static inline void cache_free(cache_t *c)
{
  free(c->head);
  c->head = NULL;
  c->cap = c->len = 0;
}

static inline int cache_append(cache_t *c, CACHE_TYPE e)
{
  if(c->cap < c->len+1){
    //printf("## %d %d\n", c->cap, c->len);
    c->head = realloc(c->head, c->cap*2*sizeof(CACHE_TYPE));
    c->cap *= 2;
  }
  c->head[c->len++] = e;
  return c->len-1;
}

static inline CACHE_TYPE cache_get(cache_t* c, unsigned int idx){
  assert(idx < c->len);
  return c->head[idx];
}

static inline void cache_set(cache_t* c, unsigned int idx, CACHE_TYPE v)
{
  assert(idx < c->len);
  c->head[idx] = v;
}

#define cache_length(c) ((c)->len)


struct op_contex{
  cache_t cinsn;
  cache_t cconst;
  unsigned int pc;
};

void gen_stmtseq(struct op_contex* ctx, struct ast_node* list);
struct op_contex* init_op_ctx();
const char* opcode2string(int op);
void gencode(struct op_contex* ctx, struct ast_node* root);

#define CALLID_BUILTIN_PRINT 0x01

#endif

