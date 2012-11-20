/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2012 03:24:52 PM
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
#include <assert.h>

#include "def.h"
#include "opcode.h"
#include "vm.h"


void yyerror(const char *str)
{
  fprintf(stderr,"error: %s\n",str);
}

int yywrap()
{
  return 1;
} 

void nextline(){
}

static void print_level(int l){
  int i;
  for(i=0;i<l;i++)
    printf("  ");
}

static const char* stmttype2string(int t)
{
#define __CASE(x) case x: return #x
  switch(t){
    __CASE(STMT_PRINT);
    __CASE(STMT_IF);
    __CASE(STMT_WHILE);
    __CASE(STMT_ASSIGN);
    default: return "<UNKNOWN>";
  }
#undef __CASE
}

static void print_tree(struct ast_node *node, int level);
void print_ast_stmt(struct ast_stmt* stmt, int level)
{
  if(!stmt)
    return ;
  print_level(level);
  printf("STMT <Type:%s>\n", stmttype2string(stmt->t));
  switch(stmt->t){
    case STMT_ASSIGN:
      print_level(level);
      printf("LHS:\n");
      print_tree(stmt->assign.id, level+1);
      print_level(level);
      printf("RHS:\n");
      print_tree(stmt->assign.expr, level+1);
      break;
    case STMT_WHILE:
      print_level(level);
      printf("COND:\n");
      print_tree(stmt->statwhile.cond, level+1);
      print_level(level);
      printf("BODY:\n");
      print_tree(stmt->statwhile.seq_body, level+1);
      break;
    case STMT_IF:
      print_level(level);
      printf("COND:\n");
      print_tree(stmt->statif.cond, level+1);
      print_level(level);
      printf("THEN:\n");
      print_tree(stmt->statif.seq_then, level+1);
      print_level(level);
      printf("ELSE:\n");
      print_tree(stmt->statif.seq_else, level+1);
      break;
    case STMT_PRINT:
      print_tree(stmt->print, level+1);
      break;
    default:
      printf("<UNKNOWN>");
  }
  printf("\n");
}

static void print_tree(struct ast_node *node, int level)
{
  print_level(level);
  if(!node) {
    printf("<NULL>\n");
    return;
  }
  switch(node->t){
    case AST_ID:
    case AST_NUM:
      print_ast_node(node);
      break;
    case AST_EXPR:
      print_ast_node(node);
      print_tree(node->expr.lhs, level+1);
      print_tree(node->expr.rhs, level+1);
      break;
    case AST_STMTSEQ:
      {
        struct ast_stmt* stmt = NULL;
        print_ast_node(node);
        STAILQ_FOREACH(stmt, &node->seq.list, next){
          print_ast_stmt(stmt, level+1);
        }
      }
      break;
    default:
      printf("ERROR\n");
  }
}

struct interp_contex{
  NUM_TYPE reg[MAX_LOCALVAR];
};


NUM_TYPE eval_expr(struct interp_contex* ctx, struct ast_node* expr)
{
  if(!expr) return 0;
  switch(expr->t){
    case AST_NUM:
      return expr->val;
    case AST_ID:
    {
      symbol_t *sym = get_symbol(get_ast_context()->global_v, expr->id);
      if(!sym){
        fprintf(stderr, "%s undefined", expr->id);
        exit(-1);
      }
      return ctx->reg[sym->id];
    }
    case AST_EXPR:
      {
        struct ast_expr* e = &expr->expr;
        NUM_TYPE rhs = eval_expr(ctx, e->rhs);
        NUM_TYPE lhs = eval_expr(ctx, e->lhs);
#define __CASE(op, cmp) case op: return lhs cmp rhs
        switch(e->op){
          __CASE(EQ, ==);
          __CASE(NE, !=);
          __CASE(LT, <);
          __CASE(LE, <=);
          __CASE(GT, >);
          __CASE(GE, >=);
          __CASE(PLUS, +);
          __CASE(MINUS, -);
          __CASE(MULT, *);
          case DIVIDE:
            if(rhs==0)
              fprintf(stderr, "ERROR: DIVIDE 0\n");
            else
              return lhs / rhs;
          case NEG:
            return -rhs;
          default:
            assert(0 && "e->op");
        }
#undef __CASE
      }
      break;
    default:
      assert(0 && "not a expr");
  }
}

void eval_stmtseq(struct interp_contex* ctx, struct ast_node* list)
{
  ASSERT_AST_TYPE(list, AST_STMTSEQ);
  struct ast_stmt* stmt = NULL;
  STAILQ_FOREACH(stmt, &list->seq.list, next){
    switch(stmt->t){
      case STMT_ASSIGN:
        {
          NUM_TYPE rhs = eval_expr(ctx, stmt->assign.expr);
          symbol_t *sym = add_symbol(get_ast_context()->global_v, stmt->assign.id->id);
          if( sym->id >= MAX_LOCALVAR ){
            fprintf(stderr, "Too many vars\n");
            exit(-1);
          }
          ctx->reg[sym->id] = rhs;
        }
        break;
      case STMT_PRINT:
        {
          NUM_TYPE rhs = eval_expr(ctx, stmt->print);
          printf("OUTPUT: %d\n", rhs);
        }
        break;
      case STMT_IF:
        {
          struct ast_stmt_if *stmtif = &stmt->statif;
          assert(stmtif->cond);
          NUM_TYPE cond = eval_expr(ctx, stmtif->cond);
          if(cond){
            assert(stmtif->seq_then);
            eval_stmtseq(ctx, stmtif->seq_then);
          }else{
            if(stmtif->seq_else)
              eval_stmtseq(ctx, stmtif->seq_else);
          }
        }
        break;
      case STMT_WHILE:
        {
          struct ast_stmt_while *stmtwhile = &stmt->statwhile;
          assert(stmtwhile->cond);
          while(1){
            NUM_TYPE cond = eval_expr(ctx, stmtwhile->cond);
            if(!cond) break;
            eval_stmtseq(ctx, stmtwhile->seq_body);
          }
        }
        break;
      default:
        assert(0 && "unknown stmt type");
    }
  }
}

void run(struct ast_node* root)
{
  ASSERT_AST_TYPE(root, AST_STMTSEQ);
  struct interp_contex* ctx = (struct interp_contex*)malloc(sizeof(struct interp_contex));
  memset(ctx, 0, sizeof(struct interp_contex));
  eval_stmtseq(ctx, root);
  free(ctx);
}

void runvm(struct op_contex *ctx)
{
  struct vm_contex *vm = vm_create();
  vm_run(vm, ctx);
  vm_free(vm);
}

static struct op_contex* gennow(struct ast_node* root)
{
  struct op_contex* ctx = init_op_ctx();
  gencode(ctx, root);
  dump_opcodes(ctx);
  return ctx;
}

void execute(struct ast_node* ast_root)
{
  print_tree(ast_root, 0);
  struct op_contex* ctx = gennow(ast_root);
  run(ast_root);

  runvm(ctx);
}

int main()
{
  init_ast_context();
  yyparse();
} 

