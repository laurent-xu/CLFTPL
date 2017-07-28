#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <unordered_map>
#include "../src/queue.hxx"
#include "../src/task_scheduler.hxx"
#include "../src/clftpl.hxx"
#include <benchmark/benchmark.h>
#include "../CTPL/ctpl_stl.h"
#include "single_thread.hh"
using func_t = std::function<void(int)>;
using queue_t = queue<func_t*>;
// using TP = clfctpl::thread_pool<queue_t>;
using TP = ctpl::thread_pool;
template <typename T>
using TS = task_scheduler<std::string, T>;

#define DEBUG

struct Target
{
    Target(const std::string& name, std::vector<std::string>& commands,
           std::vector<std::string>& dependencies)
      : name(name), commands(commands), dependencies(dependencies)
    {}
    std::string name;
    std::vector<std::string> commands;
    std::vector<std::string> dependencies;
    bool is_visited = false;
};

static bool is_empty(const std::string& str)
{
    return str.empty() || str[0] == '#';
}

// https://stackoverflow.com/questions/890164/how-can-i-split-a-string-by-a-delimiter-into-an-array
std::vector<std::string> explode(const std::string& str, const char& ch)
{
    std::string next;
    std::vector<std::string> result;

    // For each character in the string
    for (auto it = str.begin(); it != str.end(); it++)
    {
        // If we've hit the terminal character
        if (*it == ch)
        {
            // If we have some characters accumulated
            if (!next.empty())
            {
                // Add them to the result vector
                result.push_back(next);
                next.clear();
            }
        }
        else
        {
            // Accumulate the next character into the sequence
            next += *it;
        }
    }
    if (!next.empty())
         result.push_back(next);
    return result;
}

static Target parse_target(const std::string& first_line, std::ifstream& infile)
{
    auto p = explode(first_line, ':');
    if (p.size() > 2)
        std::abort();
    auto target_name_tmp = explode(p[0], ' ');
    if (target_name_tmp.size() != 1)
        std::abort();
    auto target_name = target_name_tmp[0];
    auto dependencies = p.size() == 2 ? explode(p[1], ' ') : std::vector<std::string>();

    std::vector<std::string> commands;
    std::string line;
    while (std::getline(infile, line))
    {
        if (!is_empty(line))
            commands.push_back(line);
        else
            break;
    }
    return Target(target_name, commands, dependencies);
}

void exec(const std::string& str)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(str.c_str(), "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get()))
    {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    std::cout << result;
}

int test()
{
    return 0;
}

template <typename TS>
static void make_run(const std::string& target, std::unordered_map<std::string, Target>& targets, TS& ts)
{
    ts.add_group(target);
    auto it = targets.find(target);
    if (it == targets.end())
        std::abort();
    if (!it->second.is_visited)
    {
        it->second.is_visited = true;
        for (const auto& dependency: it->second.dependencies)
        {
            make_run(dependency, targets, ts);
            ts.add_dependency(target, dependency);
        }
        for (const auto& command: it->second.commands)
        {
            (void)command;
            ts.push(target, exec, command);
        }
    }
}

template <typename TS>
void minimake(benchmark::State& state)
{
    std::string line;
    std::ifstream infile("make/Makefile");
    std::unordered_map<std::string, Target> targets;

    while (std::getline(infile, line))
    {
        if (!is_empty(line))
        {
            auto target = parse_target(line, infile);
            targets.emplace(target.name, target);
        }
    }

    while (state.KeepRunning())
    {
      TS ts(state.range(0));
      make_run("target1", targets, ts);
      ts.run();
    }
}

static void minimake_bench_ctpl(benchmark::State& state)
{
  minimake<TS<ctpl::thread_pool>>(state);
}

// static void minimake_bench_single(benchmark::State& state)
// {
//   minimake<TS<single_thread::thread_pool>>(state);
// }

// static void minimake_bench_bomb(benchmark::State& state)
// {
//   minimake<TS<ctpl::thread_pool>>(state);
// }

static void minimake_bench_clfctpl(benchmark::State& state)
{
  minimake<TS<clfctpl::thread_pool<queue_t>>>(state);
}

BENCHMARK(minimake_bench_ctpl)->UseRealTime()->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(5)->Arg(6) \
    ->Arg(7)->Arg(8)->Arg(9)->Arg(10)->Arg(11)->Arg(12)->Arg(13)->Arg(14) \
    ->Arg(15)->Arg(16)->Unit(benchmark::kMicrosecond);

// BENCHMARK(minimake_bench_single)->UseRealTime()->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(5)->Arg(6) \
//     ->Arg(7)->Arg(8)->Arg(9)->Arg(10)->Arg(11)->Arg(12)->Arg(13)->Arg(14) \
//     ->Arg(15)->Arg(16)->Unit(benchmark::kMicrosecond);

// BENCHMARK(minimake_bench_bomb)->UseRealTime()->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(5)->Arg(6) \
//     ->Arg(7)->Arg(8)->Arg(9)->Arg(10)->Arg(11)->Arg(12)->Arg(13)->Arg(14) \
//     ->Arg(15)->Arg(16)->Unit(benchmark::kMicrosecond);

BENCHMARK(minimake_bench_clfctpl)->UseRealTime()->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(5)->Arg(6) \
    ->Arg(7)->Arg(8)->Arg(9)->Arg(10)->Arg(11)->Arg(12)->Arg(13)->Arg(14) \
    ->Arg(15)->Arg(16)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN()
