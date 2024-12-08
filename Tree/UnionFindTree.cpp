// Union Find Tree

// 概要: データ構造をマージする一般的なテク + 経路圧縮
// 解説:
//   http://algorithms.blog55.fc2.com/blog-entry-46.html
//   http://www.prefield.com/algorithm/container/union_find.html
//   重み付き: https://qiita.com/drken/items/cce6fc5c579051e64fab
// 計算量:
//   O(a(n)) where a is the inverse of the Ackermann function.
// テクニック:
//   parent(root) を nil や -1 ではなく, -(要素数) を持たせると, 配列 1 個で構築可能.
// 注意:
//   map を使うと要素数が 8e6 個くらいで MLE (c.f. AOJ0602)

// 木の深さでマージ
#include<unordered_map>
#include<utility>
template<typename T> class UnionFind {
 public:
  void unite(const T& a, const T& b) {
    auto x = find(a), y = find(b);
    if(rank_[x] < rank_[y]) std::swap(x, y);
    if(rank_[x] == rank_[y]) ++rank_[x];
    parent_[y] = x;
  }
  T find(const T& x) {
    if(!parent_.count(x)) rank_[x] = 1, parent_[x] = x;
    return (parent_[x] == x) ? x : (parent_[x] = find(parent_[x]));
  }
 private:
  std::unordered_map<T, T> parent_;
  std::unordered_map<T, int> rank_;
};

// 要素数でマージ
#include<unordered_map>
#include<utility>
template<typename T> class UnionFindBySize {
 public:
  void unite(const T& a, const T& b) {
    T x = find(a), y = find(b);
    if(size_[x] < size_[y]) std::swap(x, y);
    if(x != y) parent_[y] = x, size_[x] += size_[y];
  }
  const T find(const T& x) {
    if(!parent_.count(x)) size_[x] = 1, parent_[x] = x;
    return (parent_[x] == x) ? x : (parent_[x] = find(parent_[x]));
  }
  const int size(const T& x) {
    return size_[find(x)];
  }
 private:
  std::unordered_map<T, T> parent_;
  std::unordered_map<T, int> size_;
};

// 省メモリ, 要素数でマージ (x が根 ⇒ p_[x] = -(要素数))
#include<vector>
#include<utility>
class UnionFindLight {
 public:
  UnionFindLight(int n) : p_(n, -1) {}
  void unite(int a, int b) {
    auto x = find(a), y = find(b);
    if(x == y) return;
    if(-p_[x] < -p_[y]) std::swap(x, y);
    p_[x] += p_[y];
    p_[y] = x;
  }
  int find(int x) {return p_[x] < 0 ? x : p_[x] = find(p_[x]);}
  int size(int x) {return -p_[find(x)];}
 private:
  std::vector<int> p_;
};

// 重み付き: W はアーベル群
#include<cassert>
#include<unordered_map>
#include<utility>
template<typename T, typename W> class WeightedUnionFind {
 public:
  void unite(const T& a, const T& b, const W& w) {
    auto x = find(a), y = find(b);
    if(rank_[x] < rank_[y]) std::swap(x, y), weight_[y] = weight(b) - weight(a) - w;
    else                                     weight_[y] = weight(a) - weight(b) + w;
    if(rank_[x] == rank_[y]) ++rank_[x];
    parent_[y] = x;
  }
  T find(const T& x) {
    if(!parent_.count(x)) rank_[x] = 1, parent_[x] = x;
    if(parent_[x] == x) return x;
    auto root = find(parent_[x]);
    weight_[x] += weight_[parent_[x]];
    return parent_[x] = root;
  }
  W weight(const T& x) {
    find(x);
    return weight_[x];
  }
  W diff(const T& a, const T& b) {
    assert(find(a) == find(b));
    return weight(b) - weight(a);
  }
 private:
  std::unordered_map<T, T> parent_;
  std::unordered_map<T, W> weight_;
  std::unordered_map<T, int> rank_;
};

// 部分永続 Union-Find: AGC002D
//   要素数でマージ (x が根 ⇒ p_[x] = -(要素数))
//   経路圧縮を捨てることで unite 履歴を保存
//   now_  := unite 回数
//   t_[x] := 頂点 x が子になったときの unite 回数
//   s_[x] := (t 回目の unite 後の, 根 x に連結な頂点数)
#include<algorithm>
#include<utility>
#include<vector>
class PartiallyPersistentUnionFind {
 public:
  PartiallyPersistentUnionFind(int n) : now_(0), p_(n, -1), t_(n, -1), s_(n, std::vector<std::pair<int, int>>(1, std::make_pair(-1, -1))) {}
  int unite(int a, int b) {
    ++now_;
    auto x = find(now_, a), y = find(now_, b);
    if(x == y) return now_;
    if(-p_[x] < -p_[y]) std::swap(x, y);
    p_[x] += p_[y];
    p_[y] = x;
    s_[x].emplace_back(now_, p_[x]);
    return t_[y] = now_;
  }
  int find(int t, int x) {return (t_[x] == -1 || t < t_[x]) ? x : find(t, p_[x]);}
  int size(int t, int x) {
    auto root = find(t, x);
    return -std::prev(std::upper_bound(std::begin(s_[root]), std::end(s_[root]), std::make_pair(t, 0)))->second;
  }
 private:
  int now_;
  std::vector<int> p_, t_;
  std::vector<std::vector<std::pair<int, int>>> s_;
};

#include<iostream>
using namespace std;
int main() {
  using LL = long long;
  int n, q;
  cin >> n >> q;
  WeightedUnionFind<int, LL> UF;
  for(auto i=0; i<q; ++i) {
    int c, x, y, z;
    cin >> c >> x >> y;
    if(c) {
      if(UF.find(x) == UF.find(y)) cout << UF.diff(x, y) << endl;
      else                         cout << "?" << endl;
    } else {
      cin >> z;
      UF.unite(x, y, z);
    }
  }
}
