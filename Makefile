CXX= g++
LDFLAGS=-lpthread -latomic
CXXFLAGS=-Wall -Wextra -Wpedantic -Werror -std=c++14 -O0 -g -Isrc

BIN_TEST= test
TEST_OBJ= quicksort.o
TEST_OBJS = $(addprefix tests/, $(TEST_OBJ))

check: $(BIN_TEST)

$(BIN_TEST): $(TEST_OBJS)
	$(LINK.cc) $^ -o $@

main: tests/main.cc
	$(LINK.cc) $^ -o $@

clean:
	rm -rf $(BIN_TEST) $(TEST_OBJS)

.PHONY: clean check
