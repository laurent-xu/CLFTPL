#define CLFCTPL_DEFINE_STOP

#include <iostream>

#include "clftpl.hxx"
#include "queue.hxx"

using func_t = std::function<void(int)>;
using queue_type_t = queue<func_t*>;
static clfctpl::thread_pool<queue_type_t> tp(8);

static int print(int id)
{
  std::cout << "Hello from thread " << id << " there are " << tp.n_waiting()
            <<  " waiting threads" <<  std::endl;
  return id;
}

static int print2(int id, int remains)
{
  std::cout << "Hello from thread " << id << " still remains "
            << remains << "calls,  there are " << tp.n_waiting()
            <<  " waiting threads" << std::endl;
  if (remains > 0)
    tp.push(print2, remains - 1);
  return id;
}

// FIXME
// static int print3(int count)
// {
//   for (; count > 0; --count)
//     std::cout << "Count down " << count << " there are " << tp.n_waiting()
//               <<  " waiting threads" << std::endl;
//   //tp.push_no_index(print3, count);
//   return 0;
// }


// static int print4()
// {
//   std::cout << "Simple test" << std::endl;
//   return 0;
// }


int main()
{
  for (int i = 0; i < 50; ++i)
  {
    tp.push(print);
    tp.push(print2, 10);
  }
  tp.wait();
  tp.push(print2, 42);
  tp.wait();
  // FIXME
  //tp.push_no_index(print3, 42);
  //tp.stop();
  //tp.push_no_index(print4);
  //tp.wait();
  tp.push(print2, 1337);
  tp.pause();
  std::cout << "The threadpool is paused and now we are going to resume it."
            << std::endl;
  tp.resume();
  tp.push(print2, 99999);
  tp.push(print2, 88888);
  tp.push(print2, 77777);

  tp.stop();
  std::cout << "The threadpool is stopped." << std::endl;

  return 0;
}
