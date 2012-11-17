/*
 * =====================================================================================
 *
 *       Filename:  def.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2012 04:08:34 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */

#ifndef __DEF_H
#define __DEF_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <sys/queue.h>

#include "type.h"
#include "y.tab.h"

enum AST_NODE_TYPE{
  AST_STMT,
  AST_STMTSEQ,
  AST_EXPR,
//  AST_BINOP,
  AST_ID,
  AST_NUM,
};

struct ast_node;

struct ast_expr{
  struct ast_node* lhs;
  struct ast_node* rhs;
  int op;
};

enum AST_STMT_TYPE{
  STMT_PRINT,
  STMT_IF,
  STMT_WHILE,
  STMT_ASSIGN
};

struct ast_stmt_if{
  struct ast_node* cond;
  struct ast_node* seq_then;
  struct ast_node* seq_else;
};

struct ast_stmt_while{
  struct ast_node* cond;
  struct ast_node* seq_body;
};

struct ast_stmt_assign{
  struct ast_node* id;
  struct ast_node* expr;
};

struct ast_stmt{
  enum AST_STMT_TYPE t;
  union{
    struct ast_node* print;
    struct ast_stmt_if statif;
    struct ast_stmt_while statwhile;
    struct ast_stmt_assign assign;
  }u;
  STAILQ_ENTRY(ast_stmt) next;
};

STAILQ_HEAD(stmtseq_list, ast_stmt);

struct ast_stmtseq{
  struct stmtseq_list list;
};

struct ast_node{
  enum AST_NODE_TYPE t;
  union{
    struct ast_stmtseq seq;
    struct ast_stmt stmt;
    struct ast_expr expr;

    NUM_TYPE val;
    ID_TYPE  id;
  }u;
};

#define MALLOC(type) ((type *)malloc(sizeof(type)))

static inline struct ast_node* new_astnode(enum AST_NODE_TYPE t){
  struct ast_node* n = MALLOC(struct ast_node);
  memset(n, 0, sizeof(struct ast_node));
  n->t = t;
  return n;
}

static inline struct ast_node* new_ast_seq()
{
  struct ast_node* n = new_astnode(AST_STMTSEQ); 
  STAILQ_INIT(&n->u.seq.list);
}

struct ast_node* number(NUM_TYPE num);
struct ast_node* name(ID_TYPE id);
struct ast_node* print(struct ast_node* inner);
struct ast_node* seq(struct ast_node* seq, struct ast_node* stat);
struct ast_node* ifstmt(struct ast_node* cond, struct ast_node* then, struct ast_node* seqelse);
struct ast_node* whilestmt(struct ast_node* cond, struct ast_node* body);
struct ast_node* assignment(struct ast_node* lhs, struct ast_node* rhs);

static inline struct ast_node* empty(){return NULL;}

struct ast_node* new_ast_expr(int op, struct ast_node* lhs, struct ast_node* rhs);

#define TRACE() do{printf("TRACE: %s\n", __func__);}while(0)

#define DEFINE_BINOP(name, op )  static inline struct ast_node* name (struct ast_node* lhs, struct ast_node* rhs) {TRACE(); return new_ast_expr(op, lhs, rhs);}

DEFINE_BINOP(plus, PLUS)
DEFINE_BINOP(minus, MINUS)
DEFINE_BINOP(mult, MULT)
DEFINE_BINOP(divide, DIVIDE)
DEFINE_BINOP(eq, EQ)
DEFINE_BINOP(ne, NE)
DEFINE_BINOP(lt, LT)
DEFINE_BINOP(le, LE)
DEFINE_BINOP(gt, GT)
DEFINE_BINOP(ge, GE)

#undef DEFINE_BINOP

#define MAX_LOCALVAR 32

#define ASSERT_AST_TYPE(node, type) if(node){assert((node)->t == (type));}

static inline struct ast_node* neg(struct ast_node* rhs)
{
  TRACE();
  return new_ast_expr(NEG, NULL, rhs);
}

void print_ast_node(struct ast_node* node);


#endif

