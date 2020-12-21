// ハフマン符号化

// ハフマン木の構築: 貪欲法 O(n log n)
// (1) 各文字の出現回数をカウント
// (2) 出現回数を重みとして, 各文字に対応するノードをSに追加
// (3) Sから重み最低のノードを2回取り出す
//     それらを子とする新しい親ノードを作りSに追加
//     新しい親ノードの重みは子の重みの総和とする
// (4) Sのサイズが1になるまで3の処理を繰り返す

#include<algorithm>
#include<iostream>
#include<map>
#include<memory>
#include<vector>

template<typename Value> class HuffmanTree {
 public:
  using Container = std::vector<Value>;
  using Weight = int;
  using Code = std::string;
  const Code L = "0";
  const Code R = "1";

  HuffmanTree(const Container& container) {
    build(container);
    traverse(root_, "");
  }

  Code encode(const Value& v) const {return encode_.at(v);}

  Value decode(const Code& v) const {return decode_.at(v);}

  void debug() const {debug(root_, 0);}
  void debug(const auto& n, auto d) const {
    if(n->rchild) debug(n->rchild, d + 1);
    for(auto i = 0; i < d; ++i) std::cout << "    "; std::cout << n->value << std::endl;
    if(n->lchild) debug(n->lchild, d + 1);
  }

 private:
  struct Node {
    Value value;
    Weight weight;
    std::unique_ptr<Node> lchild, rchild;
    Node(const Value& v, const Weight& w) : value(v), weight(w) {}
  };

  void build(const Container& container) {
    std::vector<std::unique_ptr<Node>> q;
    auto push = [&q](std::unique_ptr<Node> p) {
      q.emplace_back(std::move(p));
      std::push_heap(std::begin(q), std::end(q), [](const auto& l, const auto& r) {return l->weight > r->weight;});
    };
    auto pop = [&q]() {
      std::pop_heap(std::begin(q), std::end(q));
      auto result = std::move(q.back());
      q.pop_back();
      return result;
    };

    std::map<Value, Weight> count;
    for(const auto& i: container) ++count[i];

    for(const auto& p: count) push(std::make_unique<Node>(p.first, p.second));
    while(q.size() != 1) {
      auto n = std::make_unique<Node>(Value(), Weight());
      n->lchild = pop();
      n->rchild = pop();
      n->weight = n->lchild->weight + n->rchild->weight;
      push(std::move(n));
    }
    root_ = pop();
  }

  void traverse(const std::unique_ptr<Node>& n, const Code& c) {
    if(n->lchild) traverse(n->lchild, c + L);
    if(n->rchild) traverse(n->rchild, c + R);
    if(!n->lchild && !n->rchild) encode_[n->value] = c;
    if(!n->lchild && !n->rchild) decode_[c] = n->value;
  }

  std::unique_ptr<Node> root_;
  std::map<Value, Code> encode_;
  std::map<Code, Value> decode_;
};

#include<bits/stdc++.h>
using namespace std;
int main() {
  string s="123341";
  vector<int> v(begin(s),end(s));
  HuffmanTree<int> T(v);
  T.debug();cout<<endl;
  for(auto i: s)cout<<T.encode(i)<<endl;
}
