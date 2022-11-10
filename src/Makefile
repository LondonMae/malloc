DEBUGGER=-g
DEFINE=
CFLAGS=$(DEBUGGER) $(DEFINE) -fPIC -Wall -Werror
CC=gcc

all: basic_allocs_test alloc-scripts lynx_alloc_shared.so

%.so: %.o lynx_alloc.o
	$(CC) -g -shared $^ -o $@
	cp $@ liblynx_alloc.so

basic_allocs_test: basic_allocs_test.o lynx_alloc.o
	$(CC) -o $@ $^

sizes: sizes.o
	$(CC) -o $@ $^
	./sizes
	rm -f sizes sizes.o

alloc-scripts: run-with gdb_init_script

run-with: scripts/run_with.sh
	cp $^ run-with

gdb_init_script: scripts/gdb_script_template
	cat $^ | sed -e "s;XXX;${PWD};" > ./gdb_init_script

tests: lynx_alloc.o
	cd tests; make clean; make 

run_tests: tests all
	cd tests; make clean; make run_tests
	mv tests/test-output.md ./test-output.md

clean:
	rm -f *.o *.so sizes sizes.o basic_allocs_test gdb_init_script run-with
	cd tests; make clean
