CXX= g++
LDFLAGS=-lpthread -lbenchmark -latomic
CXXFLAGS=-Wall -Wextra -Wpedantic -std=c++14 -O0 -g -Isrc

# Add each test binary name here.
BINS_TEST= quicksort

# Do the same for each test.
QUICK_OBJS = $(addprefix tests/, bench.o quicksort.o)

# Do not forget to launch each binary test.
bench: $(BINS_TEST)
	./quicksort

# Define the target rule for each test.
quicksort: $(QUICK_OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

main: tests/main.cc
	$(LINK.cc) $^ -o $@

clean:
	rm -rf $(BINS_TEST) $(QUICK_OBJS)

.PHONY: clean
