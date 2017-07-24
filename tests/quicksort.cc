#define CLFCTPL_DEFINE_STOP
#define DEBUG 0

#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <benchmark/benchmark.h>

#include "bench.hh"
#include "../CTPL/ctpl_stl.h"
#include "../src/clftpl.hxx"
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

void quicksort(int id, int* arr, const int left, const int right, const int sz)
{
  if (left >= right)
    return;
  int part = partition(arr, left, right);
  quicksort(id, arr, left, part - 1, sz);
  quicksort(id, arr, part + 1, right, sz);
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

static void quicksort1_bench(benchmark::State& state)
{
  int* data = get_data();
  while (state.KeepRunning())
    {
      ctpl::thread_pool p(2);
      p.push(quicksort, data, 0, size - 1, size);
    }
#if DEBUG
      print(data, size);
#endif
  delete data;
}
BENCHMARK(quicksort1_bench)->UseRealTime()->Unit(benchmark::kMicrosecond);

// FIXME: Function does not end.
static void quicksort2_bench(benchmark::State& state)
{
  int* data = get_data();
  using func_t = std::function<void(int)>;
  using queue_type_t = queue<func_t*>;
  while (state.KeepRunning())
    {
      clfctpl::thread_pool<queue_type_t> tp(2);
      tp.push(quicksort, data, 0, size - 1, size);
      tp.wait();
    }
#if DEBUG
      print(data, size);
#endif
  delete data;
}
// BENCHMARK(quicksort2_bench)->UseRealTime()->Unit(benchmark::kMicrosecond);
