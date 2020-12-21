// 二分木

// AOJ 1001
// Binary Tree Intersection And Union

// 構文解析 + 二分木同士の演算
//   i ((,),(,)) ((,(,)),) => ((,),)
//   u ((,),(,)) ((,(,)),) => ((,(,)),(,))

// library
#include<iostream>
#include<memory>
// class
class BinaryTree {
private:
  struct Node {
    std::unique_ptr<Node> left_child, right_child;
    Node() : left_child(nullptr), right_child(nullptr) {}
  };
public:
  BinaryTree(const std::string& expression = "");
  std::string string() const;
private:
  std::unique_ptr<Node> parse(const std::string& expression, int& index);
  std::string string(const std::unique_ptr<Node>& node) const;
  std::unique_ptr<Node> root_;
  friend const BinaryTree operator*(const BinaryTree& lhs, const BinaryTree& rhs);
  friend const BinaryTree operator+(const BinaryTree& lhs, const BinaryTree& rhs);
  friend std::unique_ptr<Node> union_binary_trees(const std::unique_ptr<Node>& left, const std::unique_ptr<Node>& right);
  friend std::unique_ptr<Node> intersection_binary_trees(const std::unique_ptr<Node>& left, const std::unique_ptr<Node>& right);
};
// method
BinaryTree::BinaryTree(const std::string& expression) {
  int index = 0;
  root_ = parse(expression, index);
}
std::unique_ptr<BinaryTree::Node> BinaryTree::parse(const std::string& expression, int& index) {
  if(expression[index] != '(') return nullptr;
  std::unique_ptr<Node> result(new Node);
  result->left_child = parse(expression, ++index);
  result->right_child = parse(expression, ++index);
  ++index;
  return result;
}
std::string BinaryTree::string() const {
  return string(root_);
}
std::string BinaryTree::string(const std::unique_ptr<Node>& node) const {
  return !node ? "" : "(" + string(node->left_child) + "," + string(node->right_child) + ")";
}
// operator
const BinaryTree operator*(const BinaryTree& lhs, const BinaryTree& rhs) {
  BinaryTree result;
  result.root_ = intersection_binary_trees(lhs.root_, rhs.root_);
  return result;
}
std::unique_ptr<BinaryTree::Node> intersection_binary_trees(const std::unique_ptr<BinaryTree::Node>& left, const std::unique_ptr<BinaryTree::Node>& right) {
  if(!left || !right) return nullptr;
  std::unique_ptr<BinaryTree::Node> result(new BinaryTree::Node);
  result->left_child = intersection_binary_trees(left->left_child, right->left_child);
  result->right_child = intersection_binary_trees(left->right_child, right->right_child);
  return result;
}
const BinaryTree operator+(const BinaryTree& lhs, const BinaryTree& rhs) {
  BinaryTree result;
  result.root_ = union_binary_trees(lhs.root_, rhs.root_);
  return result;
}
std::unique_ptr<BinaryTree::Node> union_binary_trees(const std::unique_ptr<BinaryTree::Node>& left, const std::unique_ptr<BinaryTree::Node>& right) {
  if(!left && !right) return nullptr;
  std::unique_ptr<BinaryTree::Node> result(new BinaryTree::Node);
  result->left_child = union_binary_trees(left ? left->left_child : left, right ? right->left_child : right);
  result->right_child = union_binary_trees(left ? left->right_child : left, right ? right->right_child : right);
  return result;
}
// input
inline std::istream& operator>>(std::istream& is, BinaryTree& bt) {
  std::string s;
  is >> s;
  bt = BinaryTree(s);
  return is;
}
// output
inline std::ostream& operator<<(std::ostream& os, const BinaryTree& bt) {
  os << bt.string();
  return os;
}

#include<bits/stdc++.h>
using namespace std;
int main() {
  string op;
  BinaryTree lhs, rhs;
  while(cin >> op >> lhs >> rhs) cout << ((op == "u") ? (lhs + rhs) : (lhs * rhs)) << endl;
}
