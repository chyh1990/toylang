/*
 * =====================================================================================
 *
 *       Filename:  opcode.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2012 08:14:49 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Chen Yuheng (Chen Yuheng), chyh1990@163.com
 *   Organization:  Tsinghua Unv.
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "type.h"
#include "def.h"
#include "opcode.h"

struct op_contex* init_op_ctx()
{
  struct op_contex* ctx = (struct op_contex*)malloc(sizeof(struct op_contex));
  memset(ctx, 0, sizeof(struct op_contex));
  cache_init(&ctx->cinsn);
  cache_init(&ctx->cconst);
  cache_append(&ctx->cconst, 0);
  return ctx;
}

#define MAXVALS 65535
#define check_maxval(ctx) assert(ctx->cconst.len < MAXVALS)


static int genINSN(struct op_contex* ctx, INSN insn)
{
  cache_append(&ctx->cinsn, insn);
  ctx->pc ++;
  return ctx->pc-1;
}

void gen_expr(struct op_contex* ctx, struct ast_node *expr)
{
  int t1, tpc;
  if(!expr) return ;
  switch(expr->t){
    case AST_NUM:
      t1 = cache_append(&ctx->cconst, expr->val);
      check_maxval(ctx);
      tpc = genINSN(ctx, NEW_INSN(OP_PUSH_CONST, t1));
      break;
    case AST_ID:
    {
      symbol_t *sym = get_symbol(get_ast_context()->global_v, expr->id);
      if(!sym){
        fprintf(stderr, "undefined %s\n", expr->id);
        exit(-1);
      }
      t1 = sym->id;
      tpc = genINSN(ctx, NEW_INSN(OP_PUSH_ID, t1));
    }
      break;
    case AST_EXPR:
      {
        struct ast_expr* e = &expr->expr;
        gen_expr(ctx, e->rhs);
        gen_expr(ctx, e->lhs);
#define __CASE(op, oc) case op: genINSN(ctx, NEW_INSN(oc, 0)); break;
        switch(e->op){
          __CASE(EQ, OP_EQ);
          __CASE(NE, OP_NE);
          __CASE(LT, OP_LT);
          __CASE(LE, OP_LE);
          __CASE(PLUS, OP_ADD);
          __CASE(MINUS, OP_SUB);
          __CASE(MULT, OP_MUL);
          __CASE(DIVIDE, OP_DIV);
          //__CASE(GT, >);
          //__CASE(GE, >=);
          case GT:
            genINSN(ctx, NEW_INSN(OP_LE, 0));
            genINSN(ctx, NEW_INSN(OP_NOT, 0));
            break;
          case GE:
            genINSN(ctx, NEW_INSN(OP_LT, 0));
            genINSN(ctx, NEW_INSN(OP_NOT, 0));
            break;
          case NEG:
            genINSN(ctx, NEW_INSN(OP_PUSH_CONST,0));
            genINSN(ctx, NEW_INSN(OP_SUB, 0));
            break;
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

static void patch_jump(struct op_contex* ctx, unsigned int pc, unsigned int target)
{
  assert(OPCODE(cache_get(&ctx->cinsn, pc)) == OP_JMP);
  INSN n = NEW_INSN(OP_JMP, (unsigned int)(target - (pc+1)));
  cache_set(&ctx->cinsn, pc, n);
}

void gen_stmtseq(struct op_contex* ctx, struct ast_node* list)
{
  ASSERT_AST_TYPE(list, AST_STMTSEQ);
  struct ast_stmt* stmt = NULL;
  STAILQ_FOREACH(stmt, &list->seq.list, next){
    switch(stmt->t){
      case STMT_ASSIGN:
        {
          symbol_t *sym = add_symbol(get_ast_context()->global_v, stmt->assign.id->id);
          if( sym->id >= MAX_LOCALVAR ){
            fprintf(stderr, "Too many vars\n");
            exit(-1);
          }
          gen_expr(ctx, stmt->assign.expr);
          genINSN(ctx, NEW_INSN(OP_STORE, sym->id));
        }
        break;
      case STMT_PRINT:
        {
          gen_expr(ctx, stmt->print);
          genINSN(ctx, NEW_INSN(OP_CALL, CALLID_BUILTIN_PRINT));
        }
        break;
      case STMT_IF:
        {
          struct ast_stmt_if *stmtif = &stmt->statif;
          assert(stmtif->cond);
          gen_expr(ctx, stmtif->cond);
          genINSN(ctx, NEW_INSN(OP_TEST,0));
          unsigned int l1 = genINSN(ctx, NEW_INSN(OP_JMP, 0)); 
          gen_stmtseq(ctx, stmtif->seq_then);
          if(stmtif->seq_else){
            unsigned int l2 = genINSN(ctx, NEW_INSN(OP_JMP, 0));
            patch_jump(ctx, l1, ctx->pc);
            gen_stmtseq(ctx, stmtif->seq_else);
            patch_jump(ctx, l2, ctx->pc);
          }else{
            patch_jump(ctx, l1, ctx->pc);
          }
        }
        break;
      case STMT_WHILE:
        {
          struct ast_stmt_while *stmtwhile = &stmt->statwhile;
          assert(stmtwhile->cond);
          int bpc = ctx->pc;
          gen_expr(ctx, stmtwhile->cond);
          genINSN(ctx, NEW_INSN(OP_TEST,0));
          unsigned int le = genINSN(ctx, NEW_INSN(OP_JMP,0));
          gen_stmtseq(ctx, stmtwhile->seq_body);
          unsigned int t = genINSN(ctx, NEW_INSN(OP_JMP, 0));
          patch_jump(ctx, t, bpc);
          patch_jump(ctx, le, ctx->pc);
        }
        break;
      default:
        assert(0 && "unknown stmt type");
    }
  }
}

void gencode(struct op_contex* ctx, struct ast_node* root){
  gen_stmtseq(ctx, root);
  /* gen a nop at the end */
  genINSN(ctx, NEW_INSN(OP_NOP,0));
}

const char* opcode2string(int op)
{
  switch(op){
#define __CASE(op) case op: return #op
    __CASE(OP_NOP);
    __CASE(OP_PUSH_CONST);
    __CASE(OP_PUSH_ID);
    __CASE(OP_STORE);
    __CASE(OP_ADD);
    __CASE(OP_SUB);
    __CASE(OP_MUL);
    __CASE(OP_DIV);
    __CASE(OP_EQ);
    __CASE(OP_LT);
    __CASE(OP_LE);
    __CASE(OP_NOT);
    __CASE(OP_TEST);
    __CASE(OP_CALL);
    __CASE(OP_JMP);
#undef __CASE
    default:
      return "<UNKNOWN>";
  }
}

static void print_one_opcode(struct op_contex* ctx, INSN insn, int idx)
{
  switch(OPCODE(insn)){
    case OP_PUSH_ID:
      printf("%s\t%d", opcode2string(OPCODE(insn)), OP_A(insn));
      break;
    case OP_PUSH_CONST:
      printf("%s\t%d [%d]", opcode2string(OPCODE(insn)), OP_A(insn), cache_get(&ctx->cconst, OP_A(insn)));
      break;
    case OP_JMP:
      printf("%s\t%d [%d]", opcode2string(OPCODE(insn)), OP_OFFSET(insn), idx+1+OP_OFFSET(insn));
      break;
    default:
      printf("%s\t%d", opcode2string(OPCODE(insn)), OP_A(insn));
  }
}

void dump_opcodes(struct op_contex* ctx)
{
  int i;
  for(i=0;i<cache_length(&ctx->cinsn);i++){
    printf("%4d ", i);
    print_one_opcode(ctx, cache_get(&ctx->cinsn, i), i);
    printf("\n");
  }
}

