#ifndef _UNINITIALIZED_H_
#define _UNINITIALIZED_H_
#include <wchar.h>

#include <algorithm>

#include "construct.h"
#include "iterator.h"
#include "type_traits.h"

namespace tinystl {
// declare
template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n,
                                              const T& x, T1*);

template <class ForwardIterator, class Size, class T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n,
                                            const T& x) {
   return __uninitialized_fill_n(first, n, x, value_type(first));
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill_n(ForwardIterator first, Size n,
                                              const T& x, T1*) {
   typedef typename _type_traits<T1>::is_POD_type is_POD;
   return __uninitialized_fill_n_aux(first, n, x, is_POD());
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                  const T& x, _true_type) {
   // TODO:
   return std::fill_n(first,n,x);
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n,
                                                  const T& x, _false_type) {
   ForwardIterator cur = first;
   // TODO: exception handling
   for (; n > 0; --n, ++cur) {
      construct(&*cur, x);
   }
   return cur;
}

// declare
template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first,
                                            InputIterator last,
                                            ForwardIterator result, T*);

template <class InputIterator, class ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first,
                                          InputIterator last,
                                          ForwardIterator result) {
   return __uninitialized_copy(first, last, result, value_type(result));
}

template <class InputIterator, class ForwardIterator, class T>
inline ForwardIterator __uninitialized_copy(InputIterator first,
                                            InputIterator last,
                                            ForwardIterator result, T*) {
   typedef typename _type_traits<T>::is_POD_type is_POD;
   return __uninitialized_copy_aux(first, last, result, is_POD());
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first,
                                                InputIterator last,
                                                ForwardIterator result,
                                                _true_type) {
   return std::copy(first, last, result);
}

template <class InputIterator, class ForwardIterator>
inline ForwardIterator __uninitialized_copy_aux(InputIterator first,
                                                InputIterator last,
                                                ForwardIterator result,
                                                _false_type) {
   ForwardIterator cur = result;
   // TODO: exception handling
   for (; first != last; ++first, ++cur) {
      construct(&*cur, *first);
   }
   return cur;
}

inline char* uninitialized_copy(const char* first, const char* last,
                                char* result) {
   memmove(result, first, last - first);
   return result + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last,
                                   wchar_t* result) {
   memmove(result, first, sizeof(wchar_t) * (last - first));
   return result + (last - first);
}

template <class ForwardIterator, class T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last,
                               const T& x) {
   __uninitialized_fill(first, last, x, value_type(first));
}

template <class ForwardIterator, class Size, class T, class T1>
inline ForwardIterator __uninitialized_fill(ForwardIterator first,
                                            ForwardIterator last, const T& x,
                                            T1*) {
   typedef typename _type_traits<T1>::is_POD_type is_POD;
   return __uninitialized_fill_aux(first, last, x, is_POD());
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_aux(ForwardIterator first,
                                                ForwardIterator last,
                                                const T& x, _true_type) {
   std::fill(first, last, x);
}

template <class ForwardIterator, class Size, class T>
inline ForwardIterator __uninitialized_fill_aux(ForwardIterator first,
                                                ForwardIterator last,
                                                const T& x, _false_type) {
   ForwardIterator cur = first;
   // TODO: exception handling
   for (; cur != last; ++cur) {
      construct(&*cur, x);
   }
}
};  // namespace tinystl

#endif