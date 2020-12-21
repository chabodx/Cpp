// スプレー木

// http://yosupo.hatenablog.com/entry/2014/12/09/222401

// unique_ptrで遊ぶ

// ライブラリ
#include<iostream>
#include<memory>
#include<utility>
// クラス
class SplayTree {
public:
  using Value = int;
  SplayTree();
  void insert(const Value& v);
private:
  struct Node;
  std::unique_ptr<Node> insert(const Value& v, std::unique_ptr<Node>& node);
  std::unique_ptr<Node> root_;
public:
  // デバッグ用
  void print() const;
  void print(const std::unique_ptr<Node>& node, int d) const;
};
// 実装
struct SplayTree::Node {
  Value value;
  Node* parent;
  std::unique_ptr<Node> left_child, right_child;
  Node(const Value& v) : value(v), left_child(nullptr), right_child(nullptr) {}
};
SplayTree::SplayTree() : root_(nullptr) {}
void SplayTree::insert(const Value& v) {root_ = insert(v, root_);}
std::unique_ptr<SplayTree::Node> SplayTree::insert(const Value& v, std::unique_ptr<Node>& node) {
  if(!node) return std::make_unique<Node>(v);
  if(v < node->value) node->left_child  = insert(v, node->left_child);
  else                node->right_child = insert(v, node->right_child);
  if(node->left_child)  node->left_child->parent  = node.get();
  if(node->right_child) node->right_child->parent = node.get();
  return std::move(node);
}

// デバッグ用
void SplayTree::print() const {if(root_) print(root_, 0);}
void SplayTree::print(const std::unique_ptr<Node>& node, int d) const {
  if(node->right_child)print(node->right_child,d+1);
  for(int i=0;i<d;++i)std::cout<<"        ";
  std::cout<<node->value;
  if(node->parent)std::cout<<":"<<node->parent->value;
  std::cout<<std::endl;
  if(node->left_child)print(node->left_child,d+1);
};

#include<bits/stdc++.h>
using namespace std;
int main() {
  SplayTree t;
  t.insert(5);
  t.insert(1);
  t.insert(3);
  t.insert(2);
  t.insert(4);
  t.print();
}

/*
// shared_ptrで遊ぶ

// ライブラリ
#include<iostream>
#include<memory>
#include<utility>
// 型
typedef int Value;
struct Node {
  Value value;
  std::weak_ptr<Node> parent;
  std::shared_ptr<Node> left_child, right_child;
  Node(const Value& v) : value(v), left_child(nullptr), right_child(nullptr) {}
};
// クラス
class SplayTree {
public:
  SplayTree();
  void insert(const Value& v);
// private:
  const std::shared_ptr<Node> insert(const Value& v, const std::shared_ptr<Node>& node);
  const std::shared_ptr<Node> zig(const std::shared_ptr<Node>& node);
  const std::shared_ptr<Node> zigzig(const std::shared_ptr<Node>& node);
  const std::shared_ptr<Node> zigzag(const std::shared_ptr<Node>& node);
  void splay(const std::shared_ptr<Node>& node);
  std::shared_ptr<Node> root_;
public:  // デバッグ用
  void print() {print(root_);}
  void print(const std::shared_ptr<Node>& node, int d = 0) {
    if(node->right_child)print(node->right_child,d+1);
    for(int i=0;i<d;++i)std::cout<<"        ";
    std::cout<<node->value;
    if(!(node->parent.expired()))std::cout<<":"<<node->parent.lock()->value;
    std::cout<<std::endl;
    if(node->left_child)print(node->left_child,d+1);
  }
};
// コンストラクタ
SplayTree::SplayTree() : root_(nullptr) {}
// 追加
void SplayTree::insert(const Value& v) {root_ = insert(v, root_);}
const std::shared_ptr<Node> SplayTree::insert(const Value& v, const std::shared_ptr<Node>& node) {
  if(!node) return std::move(std::make_shared<Node>(v));
  if(v < node->value) node->left_child  = insert(v, node->left_child);
  else                node->right_child = insert(v, node->right_child);
  if(node->left_child)  node->left_child->parent  = node;
  if(node->right_child) node->right_child->parent = node;
  return std::move(node);
}
// 回転
const std::shared_ptr<Node> SplayTree::zig(const std::shared_ptr<Node>& node) {
  if(node->parent.lock()->left_child == node) {
    // left zig
    auto& X = *node;
    auto& Y = *(node->parent.lock());
    Y.left_child.swap(X.right_child);
    if(Y.parent.expired())                                                          root_.swap(X.right_child);
    else if(node->parent.lock() == node->parent.lock()->parent.lock()->right_child) Y.parent.lock()->right_child.swap(X.right_child);
    else                                                                            Y.parent.lock()->left_child.swap(X.right_child);
    X.parent.swap(Y.parent);
    Y.parent.swap(Y.left_child->parent);
  } else {
    // right zig
    auto& Y = *node;
    auto& X = *(node->parent.lock());
    X.right_child.swap(Y.left_child);
    if(X.parent.expired())                                                          root_.swap(Y.left_child);
    else if(node->parent.lock() == node->parent.lock()->parent.lock()->right_child) X.parent.lock()->right_child.swap(Y.left_child);
    else                                                                            X.parent.lock()->left_child.swap(Y.left_child);
    Y.parent.swap(X.parent);
    X.parent.swap(X.right_child->parent);
  }
  if(node->parent.expired()) return root_;
  if(node->parent.lock()->parent.expired()) return root_;
  return node->parent.lock()->parent.lock();

}
const std::shared_ptr<Node> SplayTree::zigzig(const std::shared_ptr<Node>& node) {
  zig(node->parent.lock());
  return zig(node);
}
const std::shared_ptr<Node> SplayTree::zigzag(const std::shared_ptr<Node>& node) {
  return zig(zig(node));
}
void SplayTree::splay(const std::shared_ptr<Node>& node) {
  if(node->parent.expired()) return;
  if(node->parent.lock()->parent.expired()) {
    zig(node);
  } else if(node->parent.lock() == node->parent.lock()->parent.lock()->left_child) {
    if(node == node->parent.lock()->left_child) splay(zigzig(node));    // left-left
    else                                        splay(zigzag(node));    // left-right
  } else if(node->parent.lock() == node->parent.lock()->parent.lock()->right_child) {
    if(node == node->parent.lock()->right_child) splay(zigzig(node));   // right-right
    else                                         splay(zigzag(node));   // right-left
  }
}

#include<bits/stdc++.h>
using namespace std;
int main() {
  //左
  SplayTree treeL;
  treeL.insert(5);
  treeL.insert(5);
  treeL.insert(4);
  treeL.insert(4);
  treeL.insert(3);
  //左左
  SplayTree treeLL;
  treeLL.insert(8);
  treeLL.insert(9);
  treeLL.insert(5);
  treeLL.insert(3);
  treeLL.insert(1);
  treeLL.insert(4);
  treeLL.insert(6);
  //左右
  SplayTree treeLR;
  treeLR.insert(8);
  treeLR.insert(9);
  treeLR.insert(2);
  treeLR.insert(1);
  treeLR.insert(5);
  treeLR.insert(4);
  treeLR.insert(6);

  cout<<endl<<"zigL"<<endl;
  treeL.print();
  cout<<"ROTATE"<<endl;
  treeL.zig(treeL.root_->left_child);
  treeL.print();

  cout<<endl<<"zigLL"<<endl;
  treeLL.print();
  cout<<"ROTATE"<<endl;
  treeLL.splay(treeLL.root_->left_child->left_child);
  treeLL.print();

  cout<<endl<<"zigLR"<<endl;
  treeLR.print();
  cout<<"ROTATE"<<endl;
  treeLR.splay(treeLR.root_->left_child->right_child);
  treeLR.print();
}
*/
