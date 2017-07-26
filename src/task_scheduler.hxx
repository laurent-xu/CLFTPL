#pragma once
#include "clftpl.hxx"
#include "queue.hxx"

template <typename KeyType>
namespace task_scheduler
{
  public:
    task_scheduler(int nb_threads) : tp_(nb_threads)
    {
      tp_.pause();
    }

    void add_group(const KeyType& key)
    {
      _tasks.emplace(key, {});
    }

    void add_dependency(const KeyType& target, const KeytType& dependency)
    {
      auto target_it = tasks_.find(target);
      auto dependency_it = tasks_.find(dependency);
      if (target_it == tasks_.end() || dependency_it == tasks.end())
        std::abort();

      auto& taskGroup = target_it->second;
      taskGroup.dependencies.push_back(dependency);
    }

    template <class Function, class... Args>
    std::future<typename std::result_of<Function(int, Args...)>::type>
    push(const Key& key, Function&& f, Args&&... args)
    {
      
    }


    template <class Function>
    std::future<typename std::result_of<Function(int)>::type>
    push(const Key& key, Function&& f)
    {
      
    }

    void run()
    {
      tp.resume();
      for (auto& task_group: tasks_)
        if (tasks_.remaining_jobs == 0)
          for (auto& job: task_group.jobs)
            jobs();
    }

  private:
    using func_t = std::function<void(int)>;
    using queue_t = queue<func_t*>

    struct TaskGroup
    {
      std::vector<func_t> jobs = {};
      std::vector<KeyType> dependencies = {};
      std::atomic<int> remaining_dependencies = 0;
      std::atomic<int> remaining_jobs = 0;
    };

    thread_pool<queue_t> tp_;
    std::unordered_map<KeyType, TaskGroup> tasks_;
}
