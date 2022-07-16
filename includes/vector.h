#include <algorithm>

#include "allocator.h"
#include "construct.h"
#include "iterator.h"
#include "type_traits.h"
#include "uninitialized.h"

namespace tinystl {
template <class T, class Alloc = tinystl::alloc>
class vector {
  public:
   typedef T value_type;
   typedef value_type* pointer;
   typedef value_type* iterator;
   typedef value_type& reference;
   typedef size_t size_type;
   typedef ptrdiff_t difference_type;

  protected:
   typedef simple_alloc<value_type, Alloc> data_allocator;

  protected:
   iterator start, finish, end_of_storage;

   void insert(iterator pos, size_type n, const T& x);
   void insert_aux(iterator pos, const T& x);
   void deallocate() {
      if (start) data_allocator::deallocate(start, end_of_storage - start);
   }
   iterator allocate_and_fill(size_type n, const T& x) {
      iterator result = data_allocator::allocate(n);
      uninitialized_fill_n(result, n, x);
      return result;
   }
   void fill_initialize(size_type n, const T& value) {
      start = allocate_and_fill(n, value);
      finish = start + n;
      end_of_storage = finish;
   }

  public:
   iterator begin() const { return start; }
   iterator end() const { return finish; }
   size_type size() const { return static_cast<size_type>(end() - begin()); }
   size_type capacity() const {
      return static_cast<size_type>(end_of_storage - begin());
   }
   bool empty() const { return begin() == end(); }
   reference operator[](size_type n) { return *(begin() + n); }

   vector() : start(0), finish(0), end_of_storage(0) {}
   vector(size_type n, const T& value) { fill_initialize(n, value); }
   vector(int n, const T& value) { fill_initialize(n, value); }
   vector(long long n, const T& value) { fill_initialize(n, value); }
   explicit vector(size_type n) { fill_initialize(n, T()); }
   ~vector() {
      destroy(start, finish);
      deallocate();
   }

   reference front() { return *(begin()); }
   reference back() { return *(end() - 1); }
   reference at(int pos) { return *(begin() + pos); }
   void push_back(const T& x) {
      if (finish != end_of_storage) {
         construct(finish, x);
         ++finish;
      } else {
         insert_aux(end(), x);
      }
   }
   void pop_back() {
      --finish;
      destroy(finish);
   }

   iterator erase(iterator pos) {
      if (pos + 1 != end()) std::copy(pos + 1, finish, pos);
      --finish;
      destroy(finish);
      return pos;
   }

   iterator erase(iterator first, iterator last) {
      iterator i = std::copy(last, finish, first);
      destroy(i, finish);
      finish = finish - (last - first);
      return first;
   }

   void resize(size_type new_size, const T& x) {
      if (new_size < size())
         erase(begin() + new_size, end());
      else
         insert(end(), new_size - size(), x);
   }

   void resize(size_type new_size) { resize(new_size, T()); }
   void clear() { erase(begin(), end()); }

   void _traversal() {
      for (iterator it = begin(); it != end(); ++it) {
         std::cout << (it == begin() ? "" : " ") << *it;
      }
      std::cout << '\n';
   }
   
};

template <class T, class Alloc>
void vector<T, Alloc>::insert_aux(iterator pos, const T& x) {
   if (finish != end_of_storage) {
      construct(finish, *(finish - 1));
      ++finish;
      T x_copy = x;
      std::copy_backward(pos, finish - 2, finish - 1);
      *pos = x_copy;
   } else {
      const size_type old_sz = size();
      // if old size is empty then new length is 1
      // else new length is double
      const size_type len = old_sz != 0 ? 2 * old_sz : 1;

      iterator new_start = data_allocator::allocate(len);
      iterator new_finish = new_start;
      try {
         new_finish = uninitialized_copy(start, pos, new_start);
         construct(new_finish, x);
         ++new_finish;
         new_finish = uninitialized_copy(pos, finish, new_finish);
      } catch (...) {
         // rollback
         destroy(new_start, new_finish);
         data_allocator::deallocate(new_start, len);
         throw;
      }

      destroy(begin(), end());
      deallocate();

      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
   }
}

template <class T, class Alloc>
void vector<T, Alloc>::insert(iterator pos, size_type n, const T& x) {
   if (n == 0) return;

   // already have enough space
   if (static_cast<size_type>(end_of_storage - finish) >= n) {
      T x_copy = x;
      const size_type elems_after = finish - pos;
      iterator old_finish = finish;
      if (elems_after > n) {
         uninitialized_copy(finish - n, finish, finish);
         finish += n;
         std::copy_backward(pos, old_finish, finish);
         finish += elems_after;
         std::fill(pos, old_finish, x_copy);
      } else {
         uninitialized_fill_n(finish, n - elems_after, x_copy);
         finish += n - elems_after;
         uninitialized_copy(pos, old_finish, finish);
         finish += elems_after;
         std::fill(pos, old_finish, x_copy);
      }

   } else {  // does not have enough space
      const size_type old_size = size();
      const size_type len = old_size + std::max(old_size, n);
      // allocator new space
      iterator new_start = data_allocator::allocate(len);
      iterator new_finish = new_start;
      try {
         new_finish = uninitialized_copy(start, pos, new_start);
         new_finish = uninitialized_fill_n(new_finish, n, x);
         new_finish = uninitialized_copy(pos, finish, new_finish);
      } catch (...) {
         // rollback
         destroy(new_start, new_finish);
         data_allocator::deallocate(new_start, len);
         throw;
      }
      destroy(start, finish);
      deallocate();

      start = new_start;
      finish = new_finish;
      end_of_storage = new_start + len;
   }
}

}  // namespace tinystl
