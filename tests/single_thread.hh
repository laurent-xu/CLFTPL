#pragma once
#include <future>
#include <functional>
#include <thread>

namespace single_thread
{
  class thread_pool
  {
    public:
      thread_pool(int) {}

      template <class Function, class... Args>
      std::future<typename std::result_of<Function(int, Args...)>::type>
      push(Function&& f, Args&&... args)
      {
        using return_t = typename std::result_of<Function(int, Args...)>::type;
        auto f_ = std::make_shared<std::packaged_task<return_t(int)>>(
            std::bind(std::forward<Function>(f),
                      std::placeholders::_1,
                      std::forward<Args>(args)...));
        (*f_)(0);
        auto future = f_->get_future();
        future.get();
        return future;
      }

      void stop(bool) {}

      template <class Function>
      std::future<typename std::result_of<Function(int)>::type>
      push(Function&& f)
      {
        using return_t = typename std::result_of<Function(int)>::type;
        auto f_ = std::make_shared<std::packaged_task<return_t(int)>>(
            std::forward<Function>(f));
        (*f_)(0);
        auto future = f_->get_future();
        future.get();
        return future;
      }

      template <class Function, class... Args>
      std::future<typename std::result_of<Function(Args...)>::type>
      push_no_index(Function&& f, Args&&... args)
      {
        auto wrap = [f] (int, Args&&... args) {
          return f(std::forward<Args>(args)...);
        };
        return push(wrap, std::forward<Args>(args)...);
      }

      template <class Function>
      std::future<typename std::result_of<Function()>::type>
      push_no_index(Function&& f)
      {
        auto wrap = [f] (int) {
          return f();
        };
        return push(wrap);
      }
  };
}
