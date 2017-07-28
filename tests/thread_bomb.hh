#pragma once
#include <future>
#include <functional>
#include <thread>

namespace thread_bomb
{
  class thread_pool
  {
    public:
      thread_pool(int) {}

      template <class Function, class... Args>
      std::future<typename std::result_of<Function(int, Args...)>::type>
      push(Function&& f, Args&&... args)
      {
        auto f_ = std::async(std::launch::async, f, 0,
                             std::forward<Args>(args)...);

        return f_;
      }

      void stop(bool) {}

      template <class Function>
      std::future<typename std::result_of<Function(int)>::type>
      push(Function&& f)
      {
        auto f_ = std::async(std::launch::async, f, 0);

        return f_;
      }

      template <class Function, class... Args>
      std::future<typename std::result_of<Function(Args...)>::type>
      push_no_index(Function&& f, Args&&... args)
      {
        auto f_ = std::async(std::launch::async, f,
                             std::forward<Args>(args)...);

        return f_;
      }

      template <class Function>
      std::future<typename std::result_of<Function()>::type>
      push_no_index(Function&& f)
      {
        auto f_ = std::async(std::launch::async, f);

        return f_;
      }
  };
}
