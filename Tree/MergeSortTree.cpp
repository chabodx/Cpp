// Merge-Sort Tree

// マージソート木
//   マージソートの過程を頂点とする完全二分木(ほぼセグメント木):
//     葉はサイズ 1 の集合
//     節は左右の子をマージした集合
//   数列 {Ai} に対して, 以下のようなクエリを高速に処理可能
//     例: {Al, ..., Ar} のうち x 以下の要素の個数を求める (集合を二分探索 → 添字を参照)
//     例: {Al, ..., Ar} のうち x 以下の要素の和を求める (集合を二分探索 → 構築時に計算した累積和を参照)

// 静的マージソート木
//   半開区間 [l, r) で操作
//   クエリは [0, n) の範囲
//   木の実装は 1-indexed
//   AC: ABC339G
//   空間計算量:   O(N log N)
//   計算量:
//     構築:       O(N log N)
//     一点更新:   O(N log N)
//     区間クエリ: O(log^2 N)
#include<algorithm>
#include<vector>
template<typename S> class MergeSortTree {
 public:
  MergeSortTree(int n, S es=0) : n(size(n)), es(es) {build();}
  MergeSortTree(const std::vector<S>& ds, S es=0) : es(es) {build(ds);}
  void build() {
    for(auto *ptr: {&dat, &sum}) ptr->assign(n << 1, {});
    for(auto k=1; k<(n<<1); ++k) {
      dat[k].assign((1 << (depth(n)-depth(k))), es);
      sum[k].assign((1 << (depth(n)-depth(k))) + 1, es);
    }
  }
  void build(const std::vector<S>& ds) {
    n = size(std::ssize(ds));
    build();
    for(auto i=0; i<n; ++i) dat[n+i][0] = sum[n+i][1] = ds[i];
    for(auto k=n-1; 0<k; --k) merge(k);
  }
  void update(int k, S x) {
    k += n;
    dat[k][0] = sum[k][1] = x;
    while(k = parent(k)) merge(k);
  }
  S query(int a, int b, S x) const {
    auto vl = es, vr = es;
    for(auto l = a + n, r = b + n; l < r; l = parent(l), r = parent(r)) {
      if(l&1) vl = fs(vl, fa(l++, x));
      if(r&1) vr = fs(fa(--r, x), vr);
    }
    return fs(vl, vr);
  }
 private:
  constexpr int size(int n) const {return n & (n - 1) ? size(n + (n & -n)) : n;}
  int depth(int n) const {return n ? (1 + depth(n >> 1)) : 0;}
  inline int parent(int k) const {return (k >> 1);}
  inline int lchild(int k) const {return (k << 1);}
  inline int rchild(int k) const {return (k << 1) | 1;}
  inline S fs(S lhs, S rhs) const {return lhs + rhs;}
  inline S fa(int k, S x) const {
    const auto& d = dat[k];
    auto i = std::distance(std::begin(d), std::upper_bound(std::begin(d), std::end(d), x));
    // return i;   // 個数
    return sum[k][i]; // 総和
  }
  inline void merge(int k) {
    auto lk = lchild(k), rk = rchild(k);
    if(2*n <= lk) return;
    for(auto i=0, l=0, r=0; i<std::ssize(dat[k]); ++i) {
      if(     l == std::ssize(dat[lk])) dat[k][i] = dat[rk][r++];
      else if(r == std::ssize(dat[rk])) dat[k][i] = dat[lk][l++];
      else dat[k][i] = (dat[lk][l]<dat[rk][r]) ? dat[lk][l++] : dat[k][i] = dat[rk][r++];
      sum[k][i+1] = sum[k][i] + dat[k][i];
    }
  }
  int n;
  S es;
  std::vector<std::vector<S>> dat, sum;
};

// 動的マージソート木
//   静的版の5.5倍くらい遅い
//   生ポインタに変えればほぼ倍速になるはず → TLE はぎりぎり回避できそう
//   空間計算量:   O(N log N)
//   計算量:
//     構築:       O(1)
//     全体更新:   O(N log N)
//     一点更新:   O(Σ_{i: 0 to log N} N / 2^i) = O(N (2 - 2^{-log N})) = O(N (2 - 2^log{1/N})) <= O(N)
//     区間クエリ: O(log^2 N)
//   用意するテンプレート引数:
//     V                                      入力として与えられる配列の要素の型
//     Result                                 クエリが返すオブジェクトの型
//     S                                      各頂点が持つ集合を表す型
//     Size                                   区間上の点を表す数値型
//   用意する仮想関数:
//     inline S es()                          S の単位元を返す関数
//     inline S s(const V&)                   update で葉に格納する S の要素を返す関数
//     inline S fs(const S&, const S&)        update で左右の子の集合をマージした結果を返す関数
//     inline Result fa(const V&, const S&)   query で集合の x 以下の要素に対するスコアを返す関数
//     inline Result fv(const V&, const V&)   query で左右の子のスコアをマージした結果を返す関数

#include<functional>
#include<memory>
#include<vector>
template<typename V, typename Result, typename S, typename Size>
class DynamicMergeSortTree {
 public:
  void update(Size a, const V& x) {update(a, x, ROOT, L, R);}
  Result query(Size a, Size b, const V& x) {return query(x, a, b, ROOT, L, R);}
  void build(Size L, Size R, const std::vector<V>& xs);
 protected:
  DynamicMergeSortTree(Size L, Size R) : L(L), R(R), ROOT(std::make_unique<Node>(es())) {}
  virtual inline S es() {return S();};
  virtual inline S s(const V& x) = 0;
  virtual inline S fs(const S& lhs, const S& rhs) = 0;
  virtual inline Result fa(const V& x, const S& p) = 0;
  virtual inline Result fv(const V& lhs, const V& rhs) = 0;
  struct Node {
    S val;
    std::unique_ptr<Node> lchild, rchild;
    Node(S val) : val(val), lchild(nullptr), rchild(nullptr) {}
  };
  using Ptr = std::unique_ptr<Node>;
  void update(Size a, const V& x, const Ptr &n, Size l, Size r) {
    if(l+1 == r) {n->val = s(x); return;}
    if(n->lchild == nullptr) n->lchild = std::make_unique<Node>(es());
    if(n->rchild == nullptr) n->rchild = std::make_unique<Node>(es());
    auto m = (l + r) / 2;
    if(a < m) update(a, x, n->lchild, l, m);
    else      update(a, x, n->rchild, m, r);
    n->val = fs(n->lchild->val, n->rchild->val);
  }
  Result query(const V& x, Size a, Size b, const Ptr &n, Size l, Size r) {
    if(n == nullptr)     return fa(x, es());
    if(b <= l || r <= a) return fa(x, es());
    if(a <= l && r <= b) return fa(x, n->val);
    auto m = (l + r) / 2;
    auto vl = query(x, a, b, n->lchild, l, m);
    auto vr = query(x, a, b, n->rchild, m, r);
    return fv(vl, vr);
  }
  const Size L, R;
  const Ptr ROOT;
};
template<typename V, typename Result, typename S, typename Size>
void DynamicMergeSortTree<V, Result, S, Size>::build(Size L, Size R, const std::vector<V>& xs) {
  std::function<void(const Ptr&, Size, Size)> dfs = [&](const auto& n, auto l, auto r) {
    if(l<0 || std::ssize(xs)<=l) return;
    if(l+1 == r) {n->val = s(xs[l]); return;}
    if(n->lchild == nullptr) n->lchild = std::make_unique<Node>(es());
    if(n->rchild == nullptr) n->rchild = std::make_unique<Node>(es());
    auto m = (l + r) / 2;
    dfs(n->lchild, l, m);
    dfs(n->rchild, m, r);
    n->val = fs(n->lchild->val, n->rchild->val);
  };
  dfs(ROOT, L, R);
}

// 数列 {Aa, ..., Ab} のうち x 以下の要素の個数
#include<algorithm>
#include<vector>
template<typename V = long long, typename Result = int, typename S = std::vector<V>, typename Size = long long>
class CNT : public DynamicMergeSortTree<V, Result, S, Size> {
 public:
  CNT(Size L, Size R) : DynamicMergeSortTree<V, Result, S, Size>(L, R) {}
 protected:
  inline S es() {return {};}
  inline S s(const V& x) {return {x};}
  inline S fs(const S& lhs, const S& rhs) {
    S res(std::size(lhs) + std::size(rhs));
    for(auto i=0, l=0, r=0; i < std::ssize(res); ++i) {
      if(     l == std::ssize(lhs)) res[i] = rhs[r++];
      else if(r == std::ssize(rhs)) res[i] = lhs[l++];
      else                          res[i] = (lhs[l] < rhs[r]) ? lhs[l++] : rhs[r++];
    }
    return res;
  }
  inline Result fa(const V& x, const S& s) {return std::distance(std::begin(s), std::upper_bound(std::begin(s), std::end(s), x));}
  inline Result fv(const V& lhs, const V& rhs) {return lhs + rhs;}
};

// 数列 {Aa, ..., Ab} のうち x 以下の要素の総和 (ABC339G: TLE)
#include<algorithm>
#include<utility>
#include<vector>
template<typename V = long long, typename Result = long long, typename S = std::pair<std::vector<V>, std::vector<Result>>, typename Size = long long>
class SUM : public DynamicMergeSortTree<V, Result, S, Size> {
 public:
  SUM(Size L, Size R) : DynamicMergeSortTree<V, Result, S, Size>(L, R) {}
 protected:
  inline S es() {return {{-1}, {0, 0}};}
  inline S s(const V& x) {return{{x}, {0, x}};}
  inline S fs(const S& lhs, const S& rhs) {
    S res{std::vector<V>(std::size(lhs.first) + std::size(rhs.first)), std::vector<Result>(std::size(lhs.first) + std::size(rhs.first) + 1)};
    for(auto i=0, l=0, r=0; i<std::ssize(res.first); ++i) {
      if(     l == std::ssize(lhs.first)) res.first[i] = rhs.first[r++];
      else if(r == std::ssize(rhs.first)) res.first[i] = lhs.first[l++];
      else                                res.first[i] = (lhs.first[l] < rhs.first[r]) ? lhs.first[l++] : rhs.first[r++];
      res.second[i+1] = res.first[i] + res.second[i];
    }
    return res;
  }
  inline Result fa(const V& x, const S& p) {
   const auto& s = p.first, sum = p.second;
   return sum.at(std::distance(std::begin(s), std::upper_bound(std::begin(s), std::end(s), x)));
  }
  inline Result fv(const V& lhs, const V& rhs) {return lhs + rhs;}
};

#include<bits/stdc++.h>
using namespace std;

int main() {
}
