#ifndef _ALLOC_H_
#define _ALLOC_H_
#include <cassert>
#include <cstdlib>

#if 0
#include <new>
#define __THROW_BAD_ALLOC throw bad_allocsize_t
#elif !defined(__THROW_BAD_ALLOC)
#include <iostream>
#define __THROW_BAD_ALLOC                     \
   std::cerr << "out of memory" << std::endl; \
   exit(1);
#endif
namespace tinystl {
template <int inst>
class __malloc_alloc_template {
  private:
   // these functions are used to handle oom situation
   // oom: out of memory
   static void* oom_malloc(size_t);
   static void* oom_realloc(void*, size_t);
   static void (*__malloc_alloc_oom_handler)();

  public:
   static void* allocate(size_t n) {
      void* result = std::malloc(n);
      if (0 == result) result = oom_malloc(n);
      return result;
   }
   static void deallocate(void* p, size_t) { std::free(p); }

   static void* reallocate(void* p, size_t, size_t new_sz) {
      void* result = std::realloc(p, new_sz);
      if (0 == result) result = oom_realloc(p, new_sz);
      return result;
   }

   static void (*set_malloc_handler(void (*f)()))() {
      void (*old)() = __malloc_alloc_oom_handler;
      __malloc_alloc_oom_handler = f;
      return (old);
   }
};

template <int inst>
void (*__malloc_alloc_template<inst>::__malloc_alloc_oom_handler)() = 0;

template <int inst>
void* __malloc_alloc_template<inst>::oom_malloc(size_t n) {
   void (*my_malloc_handler)();
   void* result;

   for (;;) {
      my_malloc_handler = __malloc_alloc_oom_handler;
      if (0 == my_malloc_handler) {
         __THROW_BAD_ALLOC;
      }
      (*my_malloc_handler)();
      result = std::malloc(n);
      if (result) return (result);
   }
}

template <int inst>
void* __malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
   void (*my_malloc_handler)();
   void* result;

   for (;;) {
      my_malloc_handler = __malloc_alloc_oom_handler;
      if (0 == my_malloc_handler) {
         __THROW_BAD_ALLOC;
      }
      (*my_malloc_handler)();
      result = std::realloc(p, n);
      if (result) return result;
   }
}

typedef __malloc_alloc_template<0> malloc_alloc;

enum { __ALIGN = 8 };
enum { __MAX_BYTES = 128 };
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };

template <bool threads, int inst>
class __default_alloc_template {
  private:
   // raise bytes to the multiple of 8
   static size_t ROUND_UP(size_t bytes) {
      return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1));
   }

  private:
   union obj {
      union obj* free_list_link;
      char client_date[1];  // 1 byte
   };

  private:
   // 16 free lists
   // block size: 8,16,24,32,40,48,56,64,72,80,88,96,104,112,120,128 bytes
   static obj* volatile free_list[__NFREELISTS];
   // according to bytes, determine which free-list to use
   static size_t FREELIST_INDEX(size_t bytes) {
      return (((bytes) + __ALIGN - 1) / __ALIGN - 1);
   }
   // return a obj of size n, and add other chunks of size n to free-list
   static void* refill(size_t n);
   // allocate a chunk of space that can store nobjs chunks of size n
   // if it is not possible, nobjs may decrese
   static char* chunk_alloc(size_t n, int& nobjs);

   // chunk allocation state
   static char* start_free;  // the start of the memory pool, only be changed in
                             // chunck_alloc()
   static char* end_free;    // the end of the memory pool, only be changed in
                             // chunck_alloc()
   static size_t heap_size;

  public:
   static void* allocate(size_t n);
   static void deallocate(void* p, size_t n);
   static void* reallocate(void* p, size_t old_sz, size_t new_sz);
};
template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::start_free = 0;

template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::end_free = 0;

template <bool threads, int inst>
size_t __default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
typename __default_alloc_template<threads, inst>::
    obj* volatile __default_alloc_template<threads,
                                           inst>::free_list[__NFREELISTS] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

// allocate space from free-list[FREELIST_INDEX(n)]
template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::allocate(size_t n) {
   assert(n > 0);
   obj* volatile* my_free_list;
   obj* result;
   if (n > static_cast<size_t>(__MAX_BYTES)) {
      return (malloc_alloc::allocate(n));
   }
   my_free_list = free_list + FREELIST_INDEX(n);
   result = *my_free_list;
   if (result == 0) {
      void* r = refill(ROUND_UP(n));
      return r;
   }
   *my_free_list = result->free_list_link;
   return (result);
}

// return the space to free-list
template <bool threads, int inst>
void __default_alloc_template<threads, inst>::deallocate(void* p, size_t n) {
   obj* q = static_cast<obj*>(p);
   obj* volatile* my_free_list;

   if (n > static_cast<size_t>(__MAX_BYTES)) {
      malloc_alloc::deallocate(p, n);
      return;
   }
   my_free_list = free_list + FREELIST_INDEX(n);
   q->free_list_link = *my_free_list;
   *my_free_list = q;
}

template <bool threads, int inst>
void* __default_alloc_template<threads, inst>::refill(size_t n) {
   int nobjs = 20;
   // chunck_alloc() will try to get nobjs chunks of size n as new nodes for
   // free-list
   // notice: nobjs is passed by reference
   char* chunk = chunk_alloc(n, nobjs);
   obj* volatile* my_free_list;
   obj* result;
   obj *current_obj, *next_obj;
   int i;
   // if only get one chunk then return
   if (1 == nobjs) return chunk;
   // else prepare to put new chunks into free-list
   my_free_list = free_list + FREELIST_INDEX(n);

   result = static_cast<obj*>(chunk);
   // the next chunk will be the head of my_free_list
   *my_free_list = next_obj = static_cast<obj*>(chunk + n);
   for (i = 1;; i++) {
      current_obj = next_obj;
      next_obj = static_cast<obj*>(static_cast<char*>(next_obj) + n);
      if (nobjs - 1 == i) {
         current_obj->free_list_link = 0;
         break;
      } else {
         current_obj->free_list_link = next_obj;
      }
   }
   return (result);
}

template <bool threads, int inst>
char* __default_alloc_template<threads, inst>::chunk_alloc(size_t size,
                                                           int& nobjs) {
   char* result;
   size_t total_bytes = size * nobjs;  // the bytes required
   size_t bytes_left =
       end_free - start_free;  // the bytes left in the memory pool
   if (bytes_left >= total_bytes) {
      // there are enough bytes in the memory pool
      result = start_free;
      start_free += total_bytes;
      return (result);
   } else if (bytes_left >= size) {
      // can only statisfy limited number of chunks
      nobjs = bytes_left / size;
      total_bytes = size * nobjs;
      result = start_free;
      start_free += total_bytes;
      return (result);
   } else {
      // no enough space for even one chunk
      size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
      // try to put the remain bytes into free-list
      if (bytes_left > 0) {
         obj* volatile* my_free_list = free_list + FREELIST_INDEX(bytes_left);
         static_cast<obj*>(start_free)->free_list_link = *my_free_list;
         *my_free_list = static_cast<obj*>(start_free);
      }

      // get some new space
      start_free = static_cast<char*>(std::malloc(bytes_to_get));
      if (0 == start_free) {
         // no enough space in heap
         int i;
         obj* volatile* my_free_list;
         obj* p;

         // try to search a free-list that have enough space and available
         for (i = size; i <= __MAX_BYTES; i += __ALIGN) {
            my_free_list = free_list + FREELIST_INDEX(i);
            p = *my_free_list;
            if (0 != p) {
               *my_free_list = p->free_list_link;
               start_free = static_cast<char*>(p);
               end_free = start_free + i;
               // recurisve to fix nobjs
               return (chunk_alloc(size, nobjs));
            }
         }
      }
      // no memory to use in anywhere
      end_free = 0;
      // try the oom_allocate in malloc_alloc
      start_free = static_cast<char*>(malloc_alloc::allocate(bytes_to_get));
      // this will either throw exception or solve the oom problem
      heap_size += bytes_to_get;
      end_free = start_free + bytes_to_get;
      return (chunk_alloc(size, nobjs));
   }
}
}  // namespace tinystl

#endif
