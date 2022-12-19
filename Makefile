CC := gcc
CFLAGS := -std=gnu99 -g -O2 -Iinclude

all: dirs bminor

dirs: 
	@mkdir -p objs

objs/%.o: src/%.c
	@echo Compiling $@...
	@$(CC) $(CFLAGS) -c $^ -o $@

src/parser.c include/token.h: src/parser.bison
	@echo Bisoning $@...
	@bison -t --defines=include/token.h  --output=src/parser.c -v src/parser.bison

src/scanner.c: src/scanner.flex include/token.h
	@echo Flexening $@...
	@flex -o src/scanner.c src/scanner.flex

test_scanner: bminor
	@./bin/run_all_tests.sh scanner

test_parser: bminor
	@./bin/run_all_tests.sh parser

test_printer: bminor
	@./bin/run_all_tests.sh printer

test_typecheck: bminor
	@./bin/run_all_tests.sh typecheck

test_codegen: bminor
	@./bin/run_all_tests.sh codegen

test_all: all
	@echo Testing Everything...
	@./bin/run_all_tests.sh all



bminor: objs/parser.o objs/scanner.o objs/options.o objs/bminor.o objs/stmt.o objs/decl.o objs/expr.o objs/symbol.o objs/type.o objs/error.o objs/param_list.o objs/scope.o objs/hash_table.o objs/scratch.o
	@echo Compiling $@...
	@$(CC) $(CFLAGS) $^ -o $@

clean:
	@echo Cleaning ...
	@rm -rf bminor objs src/scanner.c include/token.h src/parser.c src/parser.output out.s prog

