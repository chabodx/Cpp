// Trie

// 構築: O(|Si| + ... + |SN|)

#include<memory>
#include<unordered_map>
template<typename T> class Trie {
 public:
  void insert(const std::string& s, const T& value) {insert(root_, s, 0, value);}
  T find(const std::string& s) const {return find(root_, s, 0);}
 private:
  using Size = int;
  struct Node {
    T value;
    std::unordered_map<char, std::unique_ptr<Node>> child;
  };
  void insert(std::unique_ptr<Node>& node, const std::string& s, Size i, const T& v) {
    if(!node) node = std::make_unique<Node>();
    if(i == s.size()) node->value = v;
    else insert(node->child[s[i]], s, i + 1, v);
  }
  T find(const std::unique_ptr<Node>& node, const std::string& s, Size i) const {
    if(!node) return 0;
    if(i == s.size()) return node->value;
    return find(node->child[s[i]], s, i + 1);
  }
  std::unique_ptr<Node> root_;
};

#include<bits/stdc++.h>
using namespace std;

int main() {
}
