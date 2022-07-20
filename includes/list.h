#ifndef _LIST_H_
#define _LIST_H_
#include <cassert>
#include <type_traits>

#include "allocator.h"
#include "construct.h"
#include "iterator.h"

namespace tinystl {
template <class T>
struct __list_node {
   typedef __list_node<T>* node_pointer;
   node_pointer prev;
   node_pointer next;
   T data;
};

template <class T>
struct __list_iterator : public iterator<bidirectional_iterator_tag, T> {
   typedef __list_iterator<T> self;
   typedef __list_node<T>* link_type;
   typedef iterator<bidirectional_iterator_tag, T> iterator;

   link_type node;

   __list_iterator(link_type x) : node(x){};
   __list_iterator() : node(nullptr){};
   __list_iterator(const self& it) : node(it.node){};

   bool operator==(const self& x) const { return node == x.node; }
   bool operator!=(const self& x) const { return !(*this == x); }

   typename iterator::reference operator*() const { return (*node).data; }
   typename iterator::pointer operator->() const { return &(operator*()); }
   self& operator++() {
      node = node->next;
      return *this;
   }
   self operator++(int) {
      self temp = *this;
      ++*this;
      return temp;
   }
   self& operator--() {
      node = node->prev;
      return *this;
   }
   self operator--(int) {
      self temp = *this;
      --*this;
      return *this;
   }
};

template <class T, class Alloc = alloc>
class list {
  protected:
   typedef __list_node<T> list_node;
   typedef simple_alloc<list_node, Alloc> list_node_allocator;
   typedef list<T, Alloc> self;

  public:
   typedef list_node* link_type;
   typedef __list_iterator<T> iterator;
   typedef T value_type;
   typedef value_type* pointer;
   typedef value_type& reference;
   typedef typename iterator::difference_type difference_type;
   typedef size_t size_type;

  protected:
   link_type node;

  public:
   list() { empty_init(); }
   iterator begin() const { return node->next; }
   iterator end() const { return node; }
   bool empty() const { return node->next == node; }
   size_type size() const {
      size_type result = 0;
      result = static_cast<size_type>(distance(begin(), end()));
      return result;
   }
   reference front() { return *begin(); }
   reference back() { return *(--end()); }

  protected:
   link_type get_node() { return list_node_allocator::allocate(); }
   void put_node(link_type p) { list_node_allocator::deallocate(p); }
   link_type create_node(const T& x) {
      link_type p = get_node();
      construct(&(p->data), x);
      return p;
   }
   void destroy_node(link_type p) {
      destroy(&(p->data));
      put_node(p);
   }

  public:
   iterator insert(iterator pos, const T& x) {
      link_type new_node = create_node(x);
      new_node->prev = pos.node->prev;
      new_node->next = pos.node;
      pos.node->prev->next = new_node;
      pos.node->prev = new_node;
      return iterator(new_node);
   }

   void push_front(const T& x) { insert(begin(), x); }
   void push_back(const T& x) { insert(end(), x); }

   iterator erase(iterator pos) {
      link_type next_node = pos.node->next;
      link_type prev_node = pos.node->prev;
      prev_node->next = next_node;
      next_node->prev = prev_node;
      destroy_node(pos.node);
      return iterator(next_node);
   }

   void pop_front() { erase(begin()); }
   void pop_back() {
      iterator temp = end();
      erase(--temp);
   }

   void clear() {
      link_type cur = begin().node;
      while (cur != node) {
         link_type temp = cur;
         cur = cur->next;
         destroy_node(temp);
      }
      node->next = node;
      node->prev = node;
   }

   void remove(const T& value) {
      iterator first = begin();
      iterator last = end();
      while (first != last) {
         iterator next = first;
         ++next;
         if (*first == value) {
            erase(first);
         }
         first = next;
      }
   }

   void unique() {
      iterator first = begin();
      iterator last = end();
      if (first == last) {
         return;
      }
      iterator next = first;
      while (++next != last) {
         if (*first == *next) {
            erase(next);
         } else {
            first = next;
         }
         next = first;
      }
   }

   void splice(iterator pos, self& x) {
      assert(("x is different from *this", &x != this));
      if (!x.empty()) transfer(pos, x.begin(), x.end());
   }

   void splice(iterator pos, iterator i) {
      iterator j = i;
      ++j;
      if (pos == i || pos == j) {
         return;
      }
      transfer(pos, i, j);
   }

   void splice(iterator pos, iterator first, iterator last) {
      // pos can not in [first,last)
      if (first != last) {
         transfer(pos, first, last);
      }
   }

   void merge(self& x) {
      iterator first1 = begin();
      iterator last1 = end();
      iterator first2 = x.begin();
      iterator last2 = x.end();
      while (first1 != last1 && first2 != last2) {
         if (*first2 < *first1) {
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
         } else {
            ++first1;
         }
      }
      if (first1 != first2) {
         transfer(last1, first2, last2);
      }
   }

   void reverse() {
      if (node->next == node || node->next->next == node) return;
      iterator first = begin();
      ++first;
      while (first != end()) {
         iterator old = first;
         ++first;
         transfer(begin(), old, first);
      }
   }

   void sort(iterator first, iterator last) {
      if (first != last && first.node->next != last.node) {
         iterator p = partition(first, last);
         sort(first, p);
         sort(++p, last);
      }
   }

   void _traversal() {
      for (iterator it = begin(); it != end(); ++it) {
         std::cout << (it == begin() ? "" : " ") << *it;
      }
      std::cout << std::endl;
   }

  protected:
   void empty_init() {
      node = get_node();
      node->next = node;
      node->prev = node;
   }

   void transfer(iterator pos, iterator first, iterator last) {
      if (pos != last) {
         last.node->prev->next = pos.node;
         first.node->prev->next = last.node;
         pos.node->prev->next = first.node;
         link_type temp = pos.node->prev;
         pos.node->prev = last.node->prev;
         last.node->prev = first.node->prev;
         first.node->prev = temp;
      }
   }

   iterator partition(iterator first, iterator last) {
      iterator pivot = last;
      --pivot;
      iterator i = first;
      --i;
      for (iterator j = first; j != pivot; ++j) {
         if (j.node->data < pivot.node->data) {
            ++i;
            std::swap(j.node->data, i.node->data);
         }
      }
      ++i;
      std::swap(i.node->data, pivot.node->data);
      return i;
   }
};
#endif
}  // namespace tinystl