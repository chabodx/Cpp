// unique_ptrを取り出せるヒープを作る
//   https://stackoverflow.com/questions/46994504/how-to-pull-a-unique-ptr-off-a-priority-queue-and-maintain-the-ownership-semanti

#include<bits/stdc++.h>
using namespace std;

struct MyStruct {int n; MyStruct(int n = 0) : n(n) {}};

int main() {
  std::vector<std::unique_ptr<MyStruct>> q1;

  auto push = [&q1](std::unique_ptr<MyStruct> p) {
      q1.push_back(std::move(p));
      std::push_heap(q1.begin(), q1.end());
  };

  auto pop = [&q1]() {
      std::pop_heap(q1.begin(), q1.end());
      auto result = std::move(q1.back());
      q1.pop_back();
      return result;
  };

  push(std::make_unique<MyStruct>(10));
  std::deque<std::unique_ptr<MyStruct>> q2;
  q2.push_back(pop());
}
