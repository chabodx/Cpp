// Treap

// 平衡二分探索木

// TODO:
//   Key をテンプレート化
//   RBST.cpp を超える (または Treap でやってたことを RBST に引き継ぐ)

#include<iostream>
#include<ctime>
using namespace std;

// merge-split ベース
//   キー (添字) を見ると二分探索木
//   優先度を見るとヒープ
// 添字は 0-indexed
//   insert(int k, int v)          添字が k のノードを挿入
//   erase(int k)                  添字が k のノードを削除
//   split(int k)                  「k 個のノードからなる木 + それ以外のノードからなる木」に分割
//   find(int k)                   添字が k のノードの値を取得
//   sum(int l, int r)             [l, r) の総和を取得
//   min(int l, int r)             [l, r) の最小値を取得
//   circular_shift(int l, int r)  [l, r] のノードをローテーション

#include<algorithm>
#include<climits>
#include<memory>
#include<random>

using Size = signed int;
using Priority = unsigned int;

template<typename T> class Treap {
 public:
  Size size() const;
  void insert(Size index, const T& value);
  void erase(Size index);
  void circular_shift(Size left, Size right);
  T find(Size index) const;
  T sum(Size left, Size right) const;
  T min(Size left, Size right) const;

 private:
  static const T INF = INT_MAX;         // for min()
  struct Node;

  Size size(const std::unique_ptr<Node>& node) const;
  T sum(const std::unique_ptr<Node>& node) const;
  T min(const std::unique_ptr<Node>& node) const;
  T find(const std::unique_ptr<Node>& node, Size index) const;
  std::unique_ptr<Node> update(std::unique_ptr<Node>& node);
  std::unique_ptr<Node> merge(std::unique_ptr<Node>& left, std::unique_ptr<Node>& right);
  std::pair<std::unique_ptr<Node>, std::unique_ptr<Node>> split(std::unique_ptr<Node>& node, Size k);
  T sum(const std::unique_ptr<Node>& node, Size left, Size right) const;
  T min(const std::unique_ptr<Node>& node, Size left, Size right) const;

  std::unique_ptr<Node> root_;

  public:
  void debug() const;
 private:
  void debug(const std::unique_ptr<Node>& node, Size d) const;
};

// struct
template<typename T> struct Treap<T>::Node {
  T value;
  T sum;
  T min;
  Size size;
  Priority priority;
  std::unique_ptr<Node> l_child, r_child;
  Node(const T& v) :value(v), sum(v), min(v), size(1), priority(std::random_device()()), l_child(nullptr), r_child(nullptr) {}
};

// public
template<typename T> Size Treap<T>::size() const {return size(root_);}
template<typename T> void Treap<T>::insert(Size index, const T& value) {
  auto s = split(root_, index);
  auto n = std::make_unique<Node>(value);
  root_ = merge(s.first, n);
  root_ = merge(root_, s.second);
}
template<typename T> void Treap<T>::erase(Size index) {
  auto s1 = split(root_, index);
  auto s2 = split(s1.second, 1);
  root_ = merge(s1.first, s2.second);
}
template<typename T> void Treap<T>::circular_shift(Size left, Size right) {
  auto s1 = split(root_, right + 1);
  auto s2 = split(s1.first, right);
  auto s3 = split(s2.first, left);
  root_ = merge(s3.first, s2.second);
  root_ = merge(root_, s3.second);
  root_ = merge(root_, s1.second);
}
template<typename T> T Treap<T>::find(Size index) const {return find(root_, index);}
template<typename T> T Treap<T>::sum(Size left, Size right) const {return sum(root_, left, right);}
template<typename T> T Treap<T>::min(Size left, Size right) const {return min(root_, left, right);}

// private
template<typename T> Size Treap<T>::size(const std::unique_ptr<Node>& node) const {return node ? node->size : 0;}
template<typename T> T Treap<T>::sum(const std::unique_ptr<Node>& node) const {return node ? node->sum : 0;}
template<typename T> T Treap<T>::min(const std::unique_ptr<Node>& node) const {return node ? node->min : INF;}
template<typename T> T Treap<T>::find(const std::unique_ptr<Node>& node, Size index) const {
  if(index == size(node->l_child)) return node->value;
  if(index < size(node->l_child))  return find(node->l_child, index);
  else                             return find(node->r_child, index - size(node->l_child) - 1);
}
template<typename T> std::unique_ptr<typename Treap<T>::Node> Treap<T>::update(std::unique_ptr<Node>& node) {
  node->size = size(node->l_child) + size(node->r_child) + 1;
  node->sum = sum(node->l_child) + sum(node->r_child) + node->value;
  node->min = std::min({min(node->l_child), min(node->r_child), node->value});
  return std::move(node);
}
template<typename T> std::unique_ptr<typename Treap<T>::Node> Treap<T>::merge(std::unique_ptr<Node>& left, std::unique_ptr<Node>& right) {
  if(!left || !right) return std::move(left ? left : right);
  if(left->priority > right->priority) {
    left->r_child = merge(left->r_child, right);
    return update(left);
  } else {
    right->l_child = merge(left, right->l_child);
    return update(right);
  }
}
template<typename T> std::pair<std::unique_ptr<typename Treap<T>::Node>, std::unique_ptr<typename Treap<T>::Node>> Treap<T>::split(std::unique_ptr<Node>& node, Size k) {
  if(!node) return std::make_pair(nullptr, nullptr);
  if(k <= size(node->l_child)) {
    auto s = split(node->l_child, k);
    node->l_child = std::move(s.second);
    return std::make_pair(std::move(s.first), update(node));
  } else {
    auto s = split(node->r_child, k - size(node->l_child) - 1);
    node->r_child = std::move(s.first);
    return std::make_pair(update(node), std::move(s.second));
  }
}
template<typename T> T Treap<T>::sum(const std::unique_ptr<Node>& node, Size left, Size right) const {
  if(!node) return 0;
  if(right < 0 || size(node) < left) return 0;
  if(left <= 0 && size(node) <= right) return node->sum;
  auto result = sum(node->l_child, left, right) + sum(node->r_child, left - size(node->l_child) - 1, right - size(node->l_child) - 1);
  if(left <= size(node->l_child) && size(node->l_child) <= right) return result + node->value;
  return result;
}
template<typename T> T Treap<T>::min(const std::unique_ptr<Node>& node, Size left, Size right) const {
  if(!node) return INF;
  if(right < 0 || size(node) < left) return INF;
  if(left <= 0 && size(node) <= right) return node->min;
  auto result = std::min(min(node->l_child, left, right), min(node->r_child, left - size(node->l_child) - 1, right - size(node->l_child) - 1));
  if(left <= size(node->l_child) && size(node->l_child) <= right) return std::min(result, node->value);
  return result;
}

// debug
template<typename T> void Treap<T>::debug() const {debug(root_, 0);}
template<typename T> void Treap<T>::debug(const std::unique_ptr<Node>& node, int d) const {
  if(!node) return;
  debug(node->r_child, d+1);
  for(auto i=0; i<d; ++i) std::cout<<"    "; std::cout<<node->value<<std::endl;
  debug(node->l_child, d+1);
}

#include<bits/stdc++.h>
using namespace std;

int main() {
  int n, q;
  cin >> n >> q;

  Treap<int> treap;
  for(int i = 0; i < n; ++i) {
    int a;
    cin >> a;
    treap.insert(i, a);
  }

  for(int i = 0; i < q; ++i) {
    int x, y, z;
    cin >> x >> y >> z;
    if(x == 0) {
      treap.circular_shift(y, z);
    } else if(x == 1) {
      cout << treap.min(y, z) << endl;
    } else {
      treap.erase(y);
      treap.insert(y, z);
    }
  }
}

/*
// merge-split ベース
// テンプレート無し

// ノードの添字は 0-indexed
// insert(int k, int v) 添字が k のノードを挿入
// erase(int k)         添字が k のノードを削除
// split(int k)         「k 個のノードからなる木 + それ以外のノードからなる木」に分割

#include<algorithm>
#include<climits>
#include<cstdlib>
#include<memory>
#include<vector>

class Treap {
public:
    int size() const;
    void insert(const int index, const int value);
    void erase(const int index);
    void circular_shift(const int left, const int right);
    int find(const int index) const;
    int sum(const int left, const int right) const;
    int min(const int left, const int right) const;

private:
    static const int INF = INT_MAX;     // for min()
    struct Node;

    int size(const std::unique_ptr<Node>& node) const;
    int sum(const std::unique_ptr<Node>& node) const;
    int min(const std::unique_ptr<Node>& node) const;
    int find(const std::unique_ptr<Node>& node, const int index) const;
    std::unique_ptr<Node> update(std::unique_ptr<Node>& node);
    std::unique_ptr<Node> merge(std::unique_ptr<Node>& left, std::unique_ptr<Node>& right);
    std::vector<std::unique_ptr<Node>> split(std::unique_ptr<Node>& node, const int k);
    int sum(const std::unique_ptr<Node>& node, const int left, const int right) const;
    int min(const std::unique_ptr<Node>& node, const int left, const int right) const;

    std::unique_ptr<Node> root_;
};

// struct
struct Treap::Node {
    int value;
    int sum;
    int min;
    int size;
    double priority;
    std::unique_ptr<Node> left_child, right_child;
    Node(int v) :value(v), sum(v), min(v), size(1), priority(std::rand()), left_child(nullptr), right_child(nullptr) {}
};

// public
int Treap::size() const {return size(root_);}
void Treap::insert(const int index, const int value) {
    auto t = split(root_, index);
    std::unique_ptr<Node> n(new Node(value));
//     auto n = std::make_unique<Node>(value);
    root_ = merge(t[0], n);
    root_ = merge(root_, t[1]);
}
void Treap::erase(const int index) {
    auto t1 = split(root_, index);
    auto t2 = split(t1[1], 1);
    root_ = merge(t1[0], t2[1]);
}
void Treap::circular_shift(const int left, const int right) {
    auto t1 = split(root_, right + 1);
    auto t2 = split(t1[0], right);
    auto t3 = split(t2[0], left);
    root_ = merge(t3[0], t2[1]);
    root_ = merge(root_, t3[1]);
    root_ = merge(root_, t1[1]);
}
int Treap::find(const int index) const {return find(root_, index);}
int Treap::sum(const int left, const int right) const {return sum(root_, left, right);}
int Treap::min(const int left, const int right) const {return min(root_, left, right);}

// private
int Treap::size(const std::unique_ptr<Node>& node) const {return node ? node->size : 0;}
int Treap::sum(const std::unique_ptr<Node>& node) const {return node ? node->sum : 0;}
int Treap::min(const std::unique_ptr<Node>& node) const {return node ? node->min : INF;}
int Treap::find(const std::unique_ptr<Node>& node, const int index) const {
    if(index < size(node->left_child))          return find(node->left_child, index);
    else if(index == size(node->left_child))    return node->value;
    else                                        return find(node->right_child, index - size(node->left_child) - 1);
}
std::unique_ptr<Treap::Node> Treap::update(std::unique_ptr<Node>& node) {
    node->size = size(node->left_child) + size(node->right_child) + 1;
    node->sum = sum(node->left_child) + sum(node->right_child) + node->value;
    node->min = std::min({min(node->left_child), min(node->right_child), node->value});
    return std::move(node);
}
std::unique_ptr<Treap::Node> Treap::merge(std::unique_ptr<Node>& left, std::unique_ptr<Node>& right) {
    if(!left || !right) return std::move(left ? left : right);
    if(left->priority > right->priority) {
        left->right_child = merge(left->right_child, right);
        return update(left);
    } else {
        right->left_child = merge(left, right->left_child);
        return update(right);
    }
}
std::vector<std::unique_ptr<Treap::Node>> Treap::split(std::unique_ptr<Node>& node, const int k) {
    std::vector<std::unique_ptr<Node>> result(2);
    if(!node) return result;
    if(k <= size(node->left_child)) {
        result = split(node->left_child, k);
        node->left_child = std::move(result[1]);
        result[1] = update(node);
    } else {
        result = split(node->right_child, k - size(node->left_child) - 1);
        node->right_child = std::move(result[0]);
        result[0] = update(node);
    }
    return result;
}
int Treap::sum(const std::unique_ptr<Node>& node, const int left, const int right) const {
    if(!node) return 0;
    if(right < 0 || size(node) < left) return 0;
    if(left <= 0 && size(node) <= right) return node->sum;
    int result = sum(node->left_child, left, right) + sum(node->right_child, left - size(node->left_child) - 1, right - size(node->left_child) - 1);
    if(left <= size(node->left_child) && size(node->left_child) <= right) return result + node->value;
    return result;
}
int Treap::min(const std::unique_ptr<Node>& node, const int left, const int right) const {
    if(!node) return INF;
    if(right < 0 || size(node) < left) return INF;
    if(left <= 0 && size(node) <= right) return node->min;
    int result = std::min(min(node->left_child, left, right), min(node->right_child, left - size(node->left_child) - 1, right - size(node->left_child) - 1));
    if(left <= size(node->left_child) && size(node->left_child) <= right) return std::min(result, node->value);
    return result;
}

int main() {
    int n, q;
    cin >> n >> q;

    Treap treap;
    for(int i = 0; i < n; ++i) {
        int a;
        cin >> a;
        treap.insert(i, a);
    }

    for(int i = 0; i < q; ++i) {
        int x, y, z;
        cin >> x >> y >> z;
        if(x == 0) {
            treap.circular_shift(y, z);
        } else if(x == 1) {
            cout << treap.min(y, z) << endl;
        } else {
            treap.erase(y);
            treap.insert(y, z);
        }
    }
}
*/

/*
// merge-split ベース
// ポインタ剥き出し

#include<vector>
#include<algorithm>
#include<memory>

struct Node {
    int value;
    double priority;
    std::unique_ptr<Node> left_child, right_child;
    int size;
    int sum;
    Node(int v, double p) :value(v), priority(p), left_child(nullptr), right_child(nullptr), size(1), sum(v) {}
};

int size(std::unique_ptr<Node>& node) {return node ? node->size : 0;}
int sum(std::unique_ptr<Node>& node) {return node ? node->sum : 0;}
std::unique_ptr<Node> update(std::unique_ptr<Node>& node) {
    node->size = size(node->left_child) + size(node->right_child) + 1;
    node->sum = sum(node->left_child) + sum(node->right_child) + node->sum;
    return std::move(node);
}
std::unique_ptr<Node> merge(std::unique_ptr<Node>& left, std::unique_ptr<Node>& right) {
    if(!left || !right) return std::move(left ? left : right);
    if(left->priority > right->priority) {
        left->right_child = merge(left->right_child, right);
        return update(left);
    } else {
        right->left_child = merge(left, right->left_child);
        return update(right);
    }
}
std::vector<std::unique_ptr<Node>> split(std::unique_ptr<Node>& node, int k) {
    std::vector<std::unique_ptr<Node>> result(2);
    if(!node) return result;
    if(k <= size(node->left_child)) {
        result = split(node->left_child, k);
        node->left_child = std::move(result[1]);
        result[1] = update(node);
        return result;
    } else {
        result = split(node->right_child, k - size(node->left_child) - 1);
        node->right_child = std::move(result[0]);
        result[0] = update(node);
        return result;
    }
    return result;
}
std::unique_ptr<Node> insert(std::unique_ptr<Node>& node, int k, int v) {
    auto t = split(node, k);
    auto n = std::unique_ptr<Node>(new Node(v, 0));
    t[0] = merge(t[0], n);
    return merge(t[0], t[1]);
}
std::unique_ptr<Node> erase(std::unique_ptr<Node>& node, int k) {
    auto t1 = split(node, k);
    auto t2 = split(t1[1], 1);
    return merge(t1[0], t2[1]);
}

int main() {
    std::unique_ptr<Node> n1(new Node(10,9));
    std::unique_ptr<Node> n2(new Node(14,8));
    std::unique_ptr<Node> n3(new Node(12,10));
    cout<< size(n1)<<endl;
    n1 = merge(n1, n2);
    cout<< size(n1)<<endl;
    n1 = insert(n1, 1, 11);
    cout<< size(n1)<<endl;
//     auto s = split(n1, 3);
//     cout<< size(s[0])<<","<<size(s[1])<<endl;
}
*/
