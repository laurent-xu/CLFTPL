#include "quicksort.hh"
#include "../CTPL/ctpl_stl.h"

void quickSort(int id, int arr[], int left, int right)
{
  id = id + 1;
  int i = left, j = right;
  int tmp;
  int pivot = arr[(left + right) / 2];

  /* partition */
  while (i <= j)
    {
      while (arr[i] < pivot)
        i++;

      while (arr[j] > pivot)
        j--;

      if (i <= j)
        {
          tmp = arr[i];
          arr[i] = arr[j];
          arr[j] = tmp;
          i++;
          j--;
        }
    };

  /* recursion */
  if (left < j)
    quickSort(arr, left, j);
  if (i < right)
    quickSort(arr, i, right);
}

int main()
{
  ctpl::thread_pool p(2);

  int array[2000];

  for (int i=0; i < 2000; ++i)
    array[i] = (rand()%100)+1;

  p.push(quickSort, array, 0, 101);
  return 0;
}
