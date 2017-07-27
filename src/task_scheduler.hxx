#pragma once
#include <unordered_map>
#include <functional>
#include <vector>
#include <atomic>
#include <future>
#include <iostream>

template <typename KeyType, typename TP>
class task_scheduler
{
  private:
    using func_t = std::function<void(int)>;

    struct task_group
    {
      task_group()
      {
          remaining_jobs = 0;
          remaining_dependencies = 0;
      }
      task_group(task_group&& other) :
          jobs(std::move(other.jobs)),
          targets(std::move(other.targets))
        {
            remaining_jobs = 0;
            remaining_dependencies = 0;
        }
      std::vector<func_t> jobs = {};
      std::vector<KeyType> targets = {};
      std::atomic<int> remaining_dependencies;
      std::atomic<int> remaining_jobs;
    };

  using task_mapper_t = std::unordered_map<KeyType, std::shared_ptr<task_group>>;

  public:
    task_scheduler(int nb_threads) : tp_(nb_threads)
    {
    }

    ~task_scheduler()
    {
        tp_.stop(true);
    }

    void add_group(const KeyType& key)
    {
      tasks_.emplace(key, std::make_shared<task_group>());
    }

    void add_dependency(const KeyType& target, const KeyType& dependency)
    {
      auto target_it = tasks_.find(target);
      auto dependency_it = tasks_.find(dependency);
      if (target_it == tasks_.end() || dependency_it == tasks_.end())
        std::abort();

      dependency_it->second->targets.push_back(target);
      ++target_it->second->remaining_dependencies;
    }

    template <class Function, class... Args>
    std::future<typename std::result_of<Function(Args...)>::type>
    push(const KeyType& key, Function&& f, Args&&... args)
    {
      if (is_running_)
        std::abort();
      auto group_it = tasks_.find(key);
      if (group_it == tasks_.end())
        std::abort();
      using return_t = typename std::result_of<Function(Args...)>::type;
      auto f_ = std::make_shared<std::packaged_task<return_t(Args...)>>(
          std::forward<Function>(f));
      auto job = [key, f_, this, args...] (int) {
          (*f_)(std::forward<Args>(args)...);
          release_job(this->tasks_, &this->tp_, key);
      };
      group_it->second->jobs.push_back(job);
      ++group_it->second->remaining_jobs;
      return f_->get_future();
    }


    template <class Function>
    std::future<typename std::result_of<Function()>::type>
    push(const KeyType& key, Function&& f)
    {
      if (is_running_)
        std::abort();
      auto group_it = tasks_.find(key);
      if (group_it == tasks_.end())
        std::abort();
      using return_t = typename std::result_of<Function()>::type;
      auto f_ = std::make_shared<std::packaged_task<return_t()>>(
          std::forward<Function>(f));
      auto job = [key, f_, this] (int) {
          (*f_)();
          release_job(this->tasks_, &this->tp_, key);
      };
      group_it->second->jobs.push_back(job);
      ++group_it->second->remaining_jobs;
      return f_->get_future();
    }

    void run()
    {
      is_running_ = true;
      for (auto& task_group: tasks_)
        if (0 == task_group.second->remaining_dependencies)
          run_group(tasks_, &tp_, task_group.first);
    }

  private:

    static void run_group(task_mapper_t& tasks,
                          TP* tp,
                          const KeyType& key)
    {
      auto& task_group = tasks[key];
      if (0 == task_group->jobs.size())
        release_targets(tasks, tp, key);
      else
        for (auto& job: task_group->jobs)
          tp->push(job);
    }

    static void release_targets(task_mapper_t& tasks,
                                TP* tp,
                                const KeyType& key)
    {
      auto& task_group = tasks[key];
      for (auto& target_key: task_group->targets)
      {
          auto& target_group = tasks[target_key];
          int remaining_dependencies =
            std::atomic_fetch_sub_explicit(
                    &target_group->remaining_dependencies, 1,
                    std::memory_order_relaxed) - 1;
          if (remaining_dependencies == 0)
            run_group(tasks, tp, target_key);
      }
    }

    static void release_job(task_mapper_t& tasks,
                            TP* tp,
                            const KeyType& key)
    {
      auto& current_group = tasks[key];
      int remaining_jobs =
          std::atomic_fetch_sub_explicit(&current_group->remaining_jobs, 1,
                                         std::memory_order_relaxed) - 1;
      if (0 == remaining_jobs)
        release_targets(tasks, tp, key);
    }

    TP tp_;
    task_mapper_t tasks_;
    bool is_running_ = false;
};
