#ifndef QIAN_LIST_H_
#define QIAN_LIST_H_

#include "qian/stl/common.h"

namespace stl {
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

  // Disable copy constructors.
  List(const List&) = delete;
  List& operator=(const List&) = delete;

  // Move assignment.
  List& operator=(List&& li) {
    this->head_ = li->head_;
    this->cur_ = li->cur_;
    li->head_ = nullptr;
    li->cur_ = nullptr;
  }

  ListNode<T>* GetHead() const { return head_; }

  void AppendTail(T val) { AppendTail(new ListNode<T>{val, nullptr, nullptr}); }

  void AppendTail(ListNode<T>* val) {
    if (head_ == nullptr) {
      cur_ = head_ = val;
    } else {
      cur_->next = val;
      cur_ = cur_->next;
    }
  }

  bool Find(const T& val) {
    auto node = GetHead();
    while (node) {
      if (node->val == val) {
        return node->val;
      }
      node = node->next;
    }
    return false;
  }

 private:
  ListNode<T>* cur_;
  ListNode<T>* head_;
};
}  // namespace stl

#endif  // QIAN_LIST_H_
