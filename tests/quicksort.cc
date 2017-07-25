#define CLFCTPL_DEFINE_STOP
#define DEBUG 0

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <benchmark/benchmark.h>

#include "bench.hh"
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
int size = 1000000; // Size of array to use.
int max_value = 1000000;

// Use this to fill the data only once for both benchmark.
int* get_data()
{
  static int* data = nullptr;
  if (!data)
    {
      srand(42);
      data = new int[size];
      for (int i=0; i < size; ++i)
        data[i] = (rand()%max_value)+1;
#if DEBUG
      print(data, size);
#endif
    }
  return data;
}

template <typename Tp>
static void quicksort_bench(benchmark::State& state)
{
  int* data = get_data();
  while (state.KeepRunning())
    {
      Tp tp(8);
      tp.push(quicksort<Tp>, data, 0, size - 1, size, &tp);
    }
#if DEBUG
      print(data, size);
#endif
}

static void quicksort_bench_ctpl(benchmark::State& state)
{
  return quicksort_bench<ctpl::thread_pool>(state);
}

static void quicksort_bench_single(benchmark::State& state)
{
  return quicksort_bench<single_thread::thread_pool>(state);
}

static void quicksort_bench_bomb(benchmark::State& state)
{
  return quicksort_bench<thread_bomb::thread_pool>(state);
}

static void quicksort_bench_clctpl(benchmark::State& state)
{
  using func_t = std::function<void(int)>;
  using queue_t = queue<func_t*>;
  return quicksort_bench<clfctpl::thread_pool<queue_t>>(state);
}

BENCHMARK(quicksort_bench_ctpl)->UseRealTime()->Unit(benchmark::kMicrosecond);
BENCHMARK(quicksort_bench_single)->UseRealTime()->Unit(benchmark::kMicrosecond);
BENCHMARK(quicksort_bench_bomb)->UseRealTime()->Unit(benchmark::kMicrosecond);
BENCHMARK(quicksort_bench_clctpl)->UseRealTime()->Unit(benchmark::kMicrosecond);
