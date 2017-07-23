LDFLAGS=-lpthread -latomic
CXXFLAGS=-Wall -Wextra -Wpedantic -Werror -std=c++14 -O0 -g -Isrc

main: tests/main.cc
	$(LINK.cc) $^ -o $@
