#pragma once
#include <atomic>
#include <future>
#include <functional>
#include <thread>
#include <vector>
#include <iostream>
#define CLFCTPL_DEFINE_STOP

namespace clfctpl
{
  template <typename QueueType>
  class thread_pool
  {
    public:
      using func_t = std::function<void(int)>;

      thread_pool(int n_threads) : n_threads_(n_threads) {init();}
      ~thread_pool()
      {
        if (!is_done_)
          wait();
      }

      int n_waiting()
      {
        return threads_.size() - n_working_;
      }

      template <class Function, class... Args>
      std::future<typename std::result_of<Function(int, Args...)>::type>
      push(Function&& f, Args&&... args)
      {
        using return_t = typename std::result_of<Function(int, Args...)>::type;
        auto f_ = std::make_shared<std::packaged_task<return_t(int)>>(
            std::bind(std::forward<Function>(f),
                      std::placeholders::_1,
                      std::forward<Args>(args)...));
        todo_queue_.push(new func_t([f_](int thread_id){(*f_)(thread_id);}));
        return f_->get_future();
      }

      template <class Function>
      std::future<typename std::result_of<Function(int)>::type>
      push(Function&& f)
      {
        using return_t = typename std::result_of<Function(int)>::type;
        auto f_ = std::make_shared<std::packaged_task<return_t(int)>>(
            std::forward<Function>(f));
        todo_queue_.push(new func_t([f_](int thread_id){(*f_)(thread_id);}));
        return f_->get_future();
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

      void wait()
      {
        while (!todo_queue_.empty())
          continue;
        while (n_working_ > 0)
        {
          continue;
        }
#ifdef CLFCTPL_DEFINE_STOP
        pause();
#endif
      }

#ifdef CLFCTPL_DEFINE_STOP
      void pause()
      {
        is_done_ = true;
        for (auto& t: threads_)
          t.join();
        threads_.clear();
      }

      void resume()
      {
        is_done_ = false;
        init();
      }

      void stop(bool isWait = false)
      {
        if (isWait)
          wait();
        else
        {
          pause();
          func_t* task;
          while(todo_queue_.pop(task))
            continue;
        }
      }
#endif

    private:
      void init()
      {
        threads_.reserve(n_threads_);
        n_working_ = 0;
        for (int i = 0; i < n_threads_; ++i)
        {
          auto tmp = std::thread(thread_run, std::ref(todo_queue_), i,
                                 std::ref(is_done_), std::ref(n_working_));
          threads_.push_back(std::move(tmp));
        }
      }

      static void thread_run(QueueType& todo_queue, int thread_id,
                             bool& is_done, std::atomic<int>& n_working)
      {
        (void) is_done;
        func_t* todo;
        bool is_waiting = true;
        while (true)
        {
#ifdef CLFCTPL_DEFINE_STOP
          if (is_done)
          {
            if (!is_waiting)
              --n_working;
            return;
          }
#endif
          if (is_waiting)
          {
            if (!todo_queue.empty())
            {
              is_waiting = false;
              ++n_working;
            }
          }

          if (!is_waiting)
          {
            if (todo_queue.empty())
            {
              --n_working;
              is_waiting = true;
            }
            else if (todo_queue.pop(todo))
            {
              (*todo)(thread_id);
              delete todo;
            }
          }
        }
      }

      std::atomic<int> n_working_;  // how many threads are waiting
      std::vector<std::thread> threads_;
      QueueType todo_queue_;
      int n_threads_;
      bool is_done_ = false;
  };
}
