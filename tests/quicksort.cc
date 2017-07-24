#include <iostream>
#include <algorithm>

#include "../CTPL/ctpl_stl.h"

#define DEBUG 0

void print(int *a, int n)
{
    int i = 0;
    while(i < n){
        std::cout << a[i] << ",";
        i++;
    }
    std::cout << "\n";
}

int partition(int *arr, const int left, const int right) {
    const int mid = left + (right - left) / 2;
    const int pivot = arr[mid];
    // move the mid point value to the front.
    std::swap(arr[mid],arr[left]);
    int i = left + 1;
    int j = right;
    while (i <= j) {
        while(i <= j && arr[i] <= pivot) {
            i++;
        }

        while(i <= j && arr[j] > pivot) {
            j--;
        }

        if (i < j) {
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i - 1],arr[left]);
    return i - 1;
}

void quicksort(int id, int *arr, const int left, const int right, const int sz){

    if (left >= right) {
        return;
    }

    int part = partition(arr, left, right);
#if DEBUG
    std::cout << "QSC:" << left << "," << right << " part=" << part << "\n";
    print (arr, sz);
#endif

    quicksort(id, arr, left, part - 1, sz);
    quicksort(id, arr, part + 1, right, sz);
}

int main()
{
  ctpl::thread_pool p(2);

  int array[2000];
  for (int i=0; i < 2000; ++i)
    array[i] = (rand()%100)+1;

#if DEBUG
  print (array, 2000);
#endif

  std::future<void> res = p.push(quicksort, array, 0, 1999, 2000);

#if DEBUG
  print (array, 2000);
#endif

  return 0;
}
