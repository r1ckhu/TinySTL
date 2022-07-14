#ifndef _ALLOCTOR_H_
#define _ALLOCTOR_H_
#include "alloc.h"

template <class T, class Alloc>
class simple_alloc {  // a simple wrapper for _malloc_alloc and _default_alloc
  public:
   static T* allocate(size_t n) {
      return 0 == n ? 0 : static_cast<T*>(Alloc::allocate(n * sizeof(T)));
   }
   static T* allocate(void) {
      return static_cast<T*>(Alloc::allocate(sizeof(T)));
   }
   static void deallocate(T* p, size_t n) {
      if (0 != n) Alloc::deallocate(p, n * sizeof(T));
   }
   static void deallocate(T* p) { Alloc::deallocate(p, sizeof(T)); }
};

#endif