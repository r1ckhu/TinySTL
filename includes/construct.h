#ifndef _CONSTRUCT_H_
#define _CONSTRUCT_H_
#include <new>
#include "iterator.h"
#include "type_traits.h"

namespace tinystl {
template <class T1, class T2>
inline void construct(T1* p, T2& value) {
   new (p) T1(value);
}

template <class T>
inline void destroy(T* p) {
   p->~T();
}

// declare
template <class ForwardIterator, class T>
inline void _destroy(ForwardIterator first, ForwardIterator last, T*);

template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
   _destroy(first, last, value_type(first));
}

template <class ForwardIterator, class T>
inline void _destroy(ForwardIterator first, ForwardIterator last, T*) {
   typedef typename _type_traits<T>::has_trivial_destructor trival_destructor;
   _destroy_aux(first, last,
                trival_destructor());  // aux: auxiliary adj./v. = supplementary
}

template <class ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last,
                         _false_type) {  // value_type has non-trival destructor
   for (; first < last; ++first) {
      destroy(&*first);
   }
}

template <class ForwardIterator>
inline void _destroy_aux(ForwardIterator first, ForwardIterator last,
                         _true_type) {}  // value_type has trival destructor

// specialized version for char* and wchar_t*
inline void destroy(char*, char*) {}

inline void destroy(wchar_t*, wchar_t*) {}

}  // namespace tinystl

#endif