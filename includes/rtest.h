#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>

namespace rtest {
class Tester {
  private:
   static std::vector<std::pair<std::string, std::function<void(void)>>>
       test_cases;

  public:
   static void run() {
      int n = 0;
      for (auto test_case : test_cases) {
         std::cout << "Test case " << (++n) << ": " << test_case.first << '\n';
         test_case.second();
         std::cout << '\n';
      }
   }
   static void add_test(std::string name, std::function<void(void)> test_case) {
      test_cases.emplace_back(name, test_case);
   }

   static int get_random_int(int first, int last) {
      static std::random_device rd;
      static std::default_random_engine generator(rd());
      std::uniform_int_distribution<int> distribution(first, last);
      return distribution(generator);
   }
};
std::vector<std::pair<std::string, std::function<void(void)>>> Tester::test_cases;

template <class T>
void EQUAL(T x, T y) {
   std::cout << '(' << x << ' ' << y << ')';
   if (x == y)
      std::cout << "......Equal\n";
   else
      std::cout << "......Not Equal\n";
}

template <class T1, class T2>
void CONTAINER_EQUAL(T1& con1, T2& con2) {
   auto first1 = std::begin(con1), last1 = std::end(con1);
   auto first2 = std::begin(con2), last2 = std::end(con2);
   for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
      if (*first1 != *first2) {
         std::cout << "......Not Equal\n";
      }
   }
   std::cout << "......Equal\n";
}

};  // namespace rtest
