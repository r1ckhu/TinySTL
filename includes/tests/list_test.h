
#include <list>
#include <string>

#include "list.h"
#include "rtest.h"

namespace test {
#define INIT_CONTAINER_SIZE 10
void list_test() {
   rtest::Tester::add_test(std::string("Compare content"), []() {
      std::list<int> std_list;
      tinystl::list<int> my_list;
      for (int i = 1; i <= INIT_CONTAINER_SIZE; ++i) {
         int j = rtest::Tester::get_random_int(-10, 10);
         my_list.push_back(j);
         std_list.push_back(j);
      }
      std::list<int>::iterator it;
      rtest::CONTAINER_EQUAL(std_list, my_list);
   });

   rtest::Tester::add_test(std::string("Push and pop"), []() {
      tinystl::list<int> my_list;
      for (int i = 1; i <= INIT_CONTAINER_SIZE; ++i) {
         int j = rtest::Tester::get_random_int(-10, 10);
         my_list.push_back(j);
      }
      std::cout << my_list.size() << '\n';
      my_list._traversal();
      my_list.pop_front();
      my_list.push_front(99);
      my_list.pop_back();
      my_list.push_back(99);
      std::cout << my_list.size() << '\n';
      my_list._traversal();

      my_list.push_back(99);
      my_list.push_back(99);
      std::cout << my_list.size() << '\n';
      my_list._traversal();
   });

   rtest::Tester::add_test(std::string("Sort"), []() {
      tinystl::list<int> my_list;
      for (int i = 1; i <= INIT_CONTAINER_SIZE; ++i) {
         int j = rtest::Tester::get_random_int(-10, 10);
         my_list.push_back(j);
      }
      my_list._traversal();
      my_list.sort(my_list.begin(), my_list.end());
      my_list._traversal();
   });

   rtest::Tester::add_test(std::string("splice"), []() {
      tinystl::list<int> one_list;
      tinystl::list<int> two_list;
      for (int i = 1; i <= INIT_CONTAINER_SIZE; ++i) {
         one_list.push_back(1);
         two_list.push_back(2);
      }
      auto temp_it = two_list.begin();
      ++(++(++temp_it));
      one_list.splice(one_list.begin(), two_list.begin(), temp_it);
      one_list._traversal();
      two_list._traversal();
      one_list.splice(one_list.end(), two_list);
      one_list._traversal();
      two_list._traversal();
   });

   rtest::Tester::add_test(std::string("Reverse, merge, remove and unique"), []() {
      tinystl::list<int> one_list;
      tinystl::list<int> two_list;
      for (int i = 1; i <= INIT_CONTAINER_SIZE; ++i) {
         one_list.push_back(1);
         two_list.push_back(2);
      }
      one_list.merge(two_list);
      one_list._traversal();
      
      one_list.reverse();
      one_list._traversal();

      one_list.remove(2);
      one_list._traversal();

      one_list.unique();
      one_list._traversal();
   });
   rtest::Tester::run();
}

}  // namespace test