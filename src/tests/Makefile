DEBUGGER=-g
DEFINE=-DDEBUG
CFLAGS=$(DEBUGGER) $(DEFINE) -Wall -Werror
CC=gcc
ALLOC=../lynx_alloc.o # we will statically link the allocator

# tests, sorted in order they should be run
TESTS = 
TESTS += allocate-and-fit region-allocate region-cleanup
TESTS += split coalesce gc-regions calloc realloc reallocarray
TESTS += ls lab3-word-count lab3-stress
TESTS += large-basic large-mixed large-calloc large-realloc
TESTS += scribble set-large region-alignment set-region
TESTS += excruciating # 🫠

# create -test binaries
TEST_TARGETS=$(foreach TEST,$(TESTS), $(TEST)-test)

# create list of test targets to be run with the test harness
comma := ,
null := 
space := $(null) #
test_list := $(subst $(space),$(comma),$(strip $(TEST_TARGETS)))


all: $(TEST_TARGETS)

%-test: test_utils.o $(wildcard %.c)
	$(eval name=$(subst -test,,$@))
	if [ -f "$(name).c" ]; then \
		$(CC) $(CFLAGS) -o $@ $(name).c test_utils.o $(ALLOC); \
	else \
		cp $(name) $@; \
		chmod u+x $@; \
	fi

run_tests: $(TEST_TARGETS)
	./run_tests.py -t $(test_list) -p ./ -o test-output.md

clean:
	rm -f *.o $(TEST_TARGETS)
