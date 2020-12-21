// Skew Heap: ならしO(log N)で色々できるmeldable heap
// 計算量:
//   meld  O(log N)
//   push  O(log N)
//   pop   O(log N)

#include<memory>
#include<utility>

class SkewHeap {
 public:
  using Key = int;
  struct Node {
    Key key;
    std::unique_ptr<Node> lchild, rchild;
    Node(Key key) : key(key) {}
  };

  SkewHeap () {}

  std::unique_ptr<Node> make() {return nullptr;}
  std::unique_ptr<Node> make(Key key) {return std::move(std::make_unique<Node>(key));}

  std::unique_ptr<Node> meld(std::unique_ptr<Node>& lhs, std::unique_ptr<Node>& rhs) {
    if(!lhs) return std::move(rhs);
    if(!rhs) return std::move(lhs);
    if(lhs->key > rhs->key) std::swap(lhs, rhs);
    lhs->rchild = meld(lhs->rchild, rhs);
    std::swap(lhs->lchild, lhs->rchild);
    return std::move(lhs);
  }

  void push(std::unique_ptr<Node>& root, Key key) {
    auto n = std::make_unique<Node>(key);
    root = meld(root, n);
  }

  Key top(std::unique_ptr<Node>& root) {return root->key;}

  Key pop(std::unique_ptr<Node>& root) {
    auto key = root->key;
    root = meld(root->lchild, root->rchild);
    return key;
  }
};

#include<bits/stdc++.h>
using namespace std;

int main() {
  SkewHeap H;
  auto root = H.make();

  H.push(root, 6);
  cout<<H.top(root)<<endl;
  H.push(root, 4);
  cout<<H.top(root)<<endl;
  H.push(root, 5);
  cout<<H.top(root)<<endl;
  cout<<endl;

  cout<<H.pop(root)<<endl;
  cout<<H.pop(root)<<endl;
  cout<<H.pop(root)<<endl;
}
