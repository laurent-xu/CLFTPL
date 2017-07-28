#define CLFCTPL_DEFINE_STOP
#define DEBUG 0

#include <iostream>
#include <algorithm>
#include <cstring>
#include <stdlib.h>
#include <benchmark/benchmark.h>

#include "../CTPL/ctpl_stl.h"
#include "../src/clftpl.hxx"
#include "thread_bomb.hh"
#include "single_thread.hh"
#include "../src/queue.hxx"

/**********************************************************************
                      QUICKSORT IMPLEMENTATION
**********************************************************************/
void print(int* arr, int n)
{
  int i = 0;
  while(i < n)
    {
        std::cout << arr[i] << ",";
        i++;
    }
  std::cout << "\n";
  fflush(stdout);
}

int partition(int* arr, const int left, const int right)
{
  const int mid = left + (right - left) / 2;
  const int pivot = arr[mid];
  // move the mid point value to the front.
  std::swap(arr[mid],arr[left]);
  int i = left + 1;
  int j = right;
  while (i <= j)
    {
      while(i <= j && arr[i] <= pivot)
        i++;

      while(i <= j && arr[j] > pivot)
        j--;

      if (i < j)
        std::swap(arr[i], arr[j]);
    }
  std::swap(arr[i - 1], arr[left]);
  return i - 1;
}

template <typename Tp>
int quicksort(int id, int* arr, const int left, const int right,
              const int sz, Tp* tp)
{
  if (left >= right)
    return id;
  int part = partition(arr, left, right);
  tp->push(quicksort<Tp>, arr, left, part - 1, sz, tp);
  tp->push(quicksort<Tp>, arr, part + 1, right, sz, tp);
  return id;
}


/**********************************************************************
                             Benchmark
**********************************************************************/
int max_value = 1000000;

// Use this to fill the data only once for both benchmark.
std::unique_ptr<int[]> get_data(int sz)
{
  std::unique_ptr<int[]> data(new int[sz]);
  srand(42);
  for (int i=0; i < sz; ++i)
    data[i] = (rand()%max_value)+1;
#if DEBUG
  std::cerr << "Initializing array" << std::endl;
#endif
  return data;
}

template <typename Tp>
void quicksort_bench(benchmark::State& state)
{
  // Get data.
  int size = state.range(0);
  std::unique_ptr<int[]> data = get_data(size);
#if DEBUG
  print(data.get(), size);
#endif
  while (state.KeepRunning())
    {
      Tp tp(8);
      tp.push(quicksort<Tp>, data.get(), 0, size - 1, size, &tp);
    }
#if DEBUG
  print(data_cpy, size);
#endif
}

void quicksort_bench_ctpl(benchmark::State& state)
{
  return quicksort_bench<ctpl::thread_pool>(state);
}

void quicksort_bench_single(benchmark::State& state)
{
  return quicksort_bench<single_thread::thread_pool>(state);
}

void quicksort_bench_bomb(benchmark::State& state)
{
  return quicksort_bench<thread_bomb::thread_pool>(state);
}

void quicksort_bench_clftpl(benchmark::State& state)
{
  using func_t = std::function<void(int)>;
  using queue_t = queue<func_t*>;
  return quicksort_bench<clfctpl::thread_pool<queue_t>>(state);
}

BENCHMARK(quicksort_bench_ctpl)->RangeMultiplier(2)->Range(8, 8<<15) \
    ->UseRealTime()->Unit(benchmark::kMicrosecond);
BENCHMARK(quicksort_bench_single)->RangeMultiplier(2)->Range(8, 8<<15) \
    ->UseRealTime()->Unit(benchmark::kMicrosecond);
BENCHMARK(quicksort_bench_bomb)->RangeMultiplier(2)->Range(8, 8<<15) \
    ->UseRealTime()->Unit(benchmark::kMicrosecond);
BENCHMARK(quicksort_bench_clftpl)->RangeMultiplier(2)->Range(8, 8<<15) \
    ->UseRealTime()->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN()
