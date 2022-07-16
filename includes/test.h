#include <random>
#include <string>
#include <vector>

#include "rtest.h"
#include "vector.h"

namespace test {
#define INIT_CONTAINER_SIZE 10
void vector_test() {
   rtest::Tester::add_test(std::string("Compare content"), []() {
      std::vector<int> std_vector;
      tinystl::vector<int> my_vector;

      for (int i = 1; i <= INIT_CONTAINER_SIZE; ++i) {
         int x = rtest::Tester::get_random_int(-100, 100);
         std_vector.push_back(x);
         my_vector.push_back(x);
      }
      rtest::CONTAINER_EQUAL(std_vector, my_vector);
   });

   rtest::Tester::add_test(std::string("Show content"), []() {
      tinystl::vector<int> my_vector;
      for (int i = 1; i <= INIT_CONTAINER_SIZE; ++i) {
         int x = rtest::Tester::get_random_int(-10, 10);
         my_vector.push_back(x);
      }
      std::cout << "vector size: " << my_vector.size() << '\n';
      std::cout << "vector capacity: " << my_vector.capacity() << '\n';
      my_vector._traversal();

      std::cout << "\npop back and erase front\n";
      my_vector.pop_back();
      my_vector.erase(my_vector.begin());
      std::cout << "vector size: " << my_vector.size() << '\n';
      std::cout << "vector capacity: " << my_vector.capacity() << '\n';
      my_vector._traversal();

      std::cout << "\nerase first three elems\n";
      my_vector.erase(my_vector.begin(), my_vector.begin() + 3);
      std::cout << "vector size: " << my_vector.size() << '\n';
      std::cout << "vector capacity: " << my_vector.capacity() << '\n';
      my_vector._traversal();
   });

   rtest::Tester::add_test(std::string("Erase"), []() {
      tinystl::vector<int> my_vector;
      for (int i = 1; i <= INIT_CONTAINER_SIZE; ++i) {
         int x = rtest::Tester::get_random_int(-10, 10);
         my_vector.push_back(x);
      }
      my_vector.erase(my_vector.begin(), my_vector.end());
      std::cout << "vector size: " << my_vector.size() << '\n';
      std::cout << "vector capacity: " << my_vector.capacity() << '\n';
   });

   rtest::Tester::add_test(std::string("Init Size"), [](){
      int size = 5;
      tinystl::vector<int> my_vector(size);
      rtest::EQUAL(my_vector.size(), (unsigned long long)size);
   });

   rtest::Tester::add_test(std::string("Fill init"), [](){
      int size = 5;
      tinystl::vector<int> my_vector(size,10);
      my_vector._traversal();
   });
   rtest::Tester::run();
}

}  // namespace test