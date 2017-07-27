#pragma once
#include <atomic>
#include <cassert>
#include <memory>

/*
** lock-free queue using the algorithms described in "Simple, Fast and
** Practical Non-Blocking and Blocking Concurrent Queue Algorithms", by
** Michael, M. M. and Scott, M. L.
*/

template <typename T>
struct node_t;

template <typename T>
struct ptr_t
{
  ptr_t(node_t<T>* node = nullptr, unsigned count = 0)
    : ptr(node), count(count)
  {}

  bool operator==(const ptr_t& o) const
  {
    return ptr == o.ptr && count == o.count;
  }

  node_t<T>* ptr;
  unsigned count = 0;
};

template <typename T>
struct node_t
{
  node_t(T value = T())
    : next(nullptr), value(value)
  {}

  std::atomic<ptr_t<T>> next;
  int count;
  T value;
};

template <typename T>
class queue
{
public:
  queue()
    : q_head(ptr_t<T>()), q_tail(ptr_t<T>())
  {
    ptr_t<T> sentinel(new node_t<T>());
    q_head.store(sentinel);
    q_tail.store(sentinel);
  }

  void push(const T& elt)
  {
    node_t<T>* node = new node_t<T>(elt);
    for (;;)
    {
      auto tail = q_tail.load(std::memory_order_acquire);
      auto next = tail.ptr->next.load(std::memory_order_acquire);
      if (tail == q_tail.load(std::memory_order_acquire))
      {
        if (next.ptr == nullptr)
        {
          ptr_t<T> new_tail_next(node, next.count + 1);
          if (tail.ptr->next.compare_exchange_weak(next, new_tail_next))
          {
            ptr_t<T> new_tail(node, tail.count + 1);
            q_tail.compare_exchange_strong(tail, new_tail);
            break;
          }
        }
        else
        {
          ptr_t<T> new_tail(next.ptr, tail.count + 1);
          q_tail.compare_exchange_strong(tail, new_tail);
        }
      }
    }
  }

  bool pop(T& val)
  {
    for (;;)
    {
      auto head = q_head.load(std::memory_order_acquire);
      auto tail = q_tail.load(std::memory_order_acquire);
      auto next = head.ptr->next.load(std::memory_order_acquire);
      if (head == q_head.load(std::memory_order_acquire))
      {
        if (head.ptr == tail.ptr)
        {
          if (next.ptr == nullptr)
            return false;
          ptr_t<T> new_tail(next.ptr, tail.count + 1);
          q_tail.compare_exchange_strong(tail, new_tail);
        }
        else
        {
          val = next.ptr->value;
          ptr_t<T> new_head(next.ptr, head.count + 1);
          if (q_head.compare_exchange_weak(head, new_head))
          {
            free(head.ptr);
            break;
          }
        }
      }
    }
    return true;
  }

  bool empty()
  {
    return q_head.load().ptr == q_tail.load().ptr;
  }

private:
  std::atomic<ptr_t<T>> q_head;
  std::atomic<ptr_t<T>> q_tail;
};
