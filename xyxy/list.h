#ifndef QIAN_LIST_H_
#define QIAN_LIST_H_

#include <cassert>

#include "xyxy/base.h"

namespace xyxy {

template <class T>
struct ListNode {
  T value;
  ListNode* prev;
  ListNode* next;
};

template <class T, class Compare = void>
class List {
 public:
  List() : head_(nullptr) {}

  ~List() {
    cur_ = head_;
    while (cur_) {
      ListNode<T>* next = cur_->next;
      delete cur_;
      cur_ = next;
    }
  }

  List(const List&) = delete;
  List& operator==(const List&) = delete;

  ListNode<T>* GetHead() {
    return head_;
  }

  T GetNext() {
    assert(cur_);
    T ret = cur_->value;
    cur_ = cur_->next;
    return ret;
  }

  // ListNode<T>* GetNext() {
  //   assert(cur_);
  //   ListNode<T>* ret = cur_;
  //   cur_ = cur_->next;
  //   return ret;
  // }

  void AppendTail(T val) { AppendTail(new ListNode<T>{val, nullptr, nullptr}); }

  void AppendTail(ListNode<T>* val) {
    if (cur_ == nullptr) {
      head_ = cur_ = val;
    }
    else {
      assert(cur_);
      cur_->next = val;
      cur_ = cur_->next;
    }
  }

 private:
  ListNode<T>* cur_;
  ListNode<T>* head_;
};

}  // namespace xyxy

#endif  // QIAN_LIST_H_
