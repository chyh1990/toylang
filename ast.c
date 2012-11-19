/*
 * =====================================================================================
 *
 *       Filename:  ast.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2012 04:47:54 PM
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

static const char *op2string(int op)
{
  switch(op){
    case PLUS: return "+";
    case MINUS: return "-";
    case MULT: return "*";
    case DIVIDE: return "/";
    case EQ: return "=";
    case LT: return "<";
    case LE: return "<=";
    case GT: return ">";
    case GE: return ">=";
    case ASSIGN: return ":=";
    default: return "<UNKNOWN>";
  }
  return NULL;
}

static const char* asttype2string(int t)
{
#define __CASE(x) case x: return #x
  switch(t){
    __CASE(AST_ID);
    __CASE(AST_NUM);
    __CASE(AST_EXPR);
    __CASE(AST_STMT);
    __CASE(AST_STMTSEQ);
    default: return "<UNKNOWN>";
  }
#undef __CASE
}


void print_ast_node(struct ast_node* node)
{
  printf("NODE: %s ", asttype2string(node->t));
  switch(node->t){
    case AST_ID:
      printf("< %c >", 'a'+node->id);
      break;
    case AST_NUM:
      printf("< %d >", node->val);
      break;
    case AST_STMT:
      printf("<Type: %d >", node->stmt.t);
      break;
    case AST_EXPR:
      printf("<OP: %s >", op2string(node->expr.op));
      break;
    default:
      printf("<UNKNOWN>");
  }
  printf("\n");
}

struct ast_node* new_ast_expr(int op, struct ast_node* lhs, struct ast_node* rhs)
{
  struct ast_node* n = new_astnode(AST_EXPR);
  //ASSERT_AST_TYPE(lhs, AST_EXPR);
  n->expr.lhs = lhs;
  n->expr.rhs = rhs;
  n->expr.op = op;
  return n;
}

struct ast_node* number(NUM_TYPE num)
{
  TRACE();
  struct ast_node *n = new_astnode(AST_NUM);
  n->val = num;
  return n;
}

struct ast_node* name(ID_TYPE id)
{
  TRACE();
  struct ast_node *n = new_astnode(AST_ID);
  n->id = id;
  return n;
}

struct ast_node* print(struct ast_node* inner)
{
  TRACE();
  struct ast_node *n = new_astnode(AST_STMT);
  //ASSERT_AST_TYPE(inner, AST_EXPR);
  n->stmt.t = STMT_PRINT;
  n->stmt.print = inner;
  return n;
}

struct ast_node* ifstmt(struct ast_node* cond, struct ast_node* then, struct ast_node* seqelse)
{
  TRACE();
  struct ast_node *n = new_astnode(AST_STMT);
  ASSERT_AST_TYPE(cond, AST_EXPR);
  ASSERT_AST_TYPE(then, AST_STMTSEQ);
  ASSERT_AST_TYPE(seqelse, AST_STMTSEQ);
  n->stmt.t = STMT_IF;
  n->stmt.statif.cond = cond;
  n->stmt.statif.seq_then = then;
  n->stmt.statif.seq_else = seqelse;
  return n;
}

struct ast_node* whilestmt(struct ast_node* cond, struct ast_node* body)
{
  TRACE();
  struct ast_node *n = new_astnode(AST_STMT);
  ASSERT_AST_TYPE(cond, AST_EXPR);
  ASSERT_AST_TYPE(body, AST_STMTSEQ);
  n->stmt.t = STMT_WHILE;
  n->stmt.statwhile.cond = cond;
  n->stmt.statwhile.seq_body = body;
  return n;
}

struct ast_node* seq(struct ast_node* seq, struct ast_node* stat)
{
  TRACE();
  ASSERT_AST_TYPE(seq, AST_STMTSEQ);
  ASSERT_AST_TYPE(stat, AST_STMT);
  if(seq==NULL)
    seq = new_ast_seq();
  STAILQ_INSERT_TAIL(&seq->seq.list, &stat->stmt, next);
  return seq;
}

struct ast_node* assignment(struct ast_node* lhs, struct ast_node* rhs)
{
  TRACE();
  ASSERT_AST_TYPE(lhs, AST_ID);
  struct ast_node *n = new_astnode(AST_STMT);
  n->stmt.t = STMT_ASSIGN;
  n->stmt.assign.id = lhs;
  n->stmt.assign.expr = rhs;
  return n;
}

