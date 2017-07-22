#include <cassert>
#include <memory>

template <typename T>
class node;

template <typename T>
using node_ptr = std::shared_ptr<node<T>>;

template <typename T>
class node
{
public:
  node(T elt)
    : next_(nullptr), elt_(elt)
  {}

  void set_next(node_ptr<T> next)
  {
    next_ = next;
  }

  node_ptr<T> next()
  {
    return next_;
  }

  T val()
  {
    return elt_;
  }

private:
  node_ptr<T> next_;
  T elt_;
};

template <typename T>
class queue
{
public:
  queue()
    : head_(nullptr), tail_(nullptr)
  {}

  void push(T elt)
  {
    auto new_node = std::make_shared<node<T>>(elt);
    if (head_ == nullptr)
    {
      assert(tail_ == nullptr);
      head_ = new_node;
      tail_ = head_;
    }
    else
    {
      tail_->set_next(new_node);
      tail_ = new_node;
    }
  }

  T pop()
  {
    auto val = head_->val();
    head_ = head_->next();
    if (head_ == nullptr)
      tail_ = nullptr;
    return val;
  }

  bool empty()
  {
    return head_ == nullptr;
  }

private:
  node_ptr<T> head_;
  node_ptr<T> tail_;
};
