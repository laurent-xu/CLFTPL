#include <benchmark/benchmark.h>

#pragma once

/**
** Quicksort function using arbitrary thread pool provided through Tp.
*/

template<typename Tp>
void quicksort_bench(benchmark::State& state);
