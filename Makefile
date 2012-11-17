all: toylang 
OBJS:= y.tab.o lex.yy.o main.o ast.o opcode.o vm.o
CFLAGS+=-g

toylang: $(OBJS)
	gcc -o $@ $^

y.tab.c y.tab.h: ini.y
	yacc -d $^

lex.yy.c: ini.lex
	flex $^

$(OBJS):  def.h type.h opcode.h vm.h

clean:
	rm -f *.o toylang y.tab.c y.tab.h lex.yy.c
