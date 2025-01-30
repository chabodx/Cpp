// 抽象化セグメント木

// 解説: https://yosupo.hatenablog.com/entry/2023/12/09/010658
// 解説: https://qiita.com/keymoon/items/0f929a19ed30f34ae6e8
// 解説: https://elliptic-shiho.github.io/segtree/segtree.pdf

// 用語
//   半群:     結合法則をみたすような, 集合 S と写像 f: S × S → S のペア
//   モノイド: 結合法則と単位元の存在をみたすような, 集合 S と写像 f: S × S → S と単位元の組
// 例
//   一次関数全体 {λx.ax+b | a,b∈Z} は, 関数合成によりモノイド (F, ・, id) をなす
//   二次関数全体 {λx.ax^2+bx+c | a,b,c∈Z} は, 関数合成により半群をなさない ∵ 二次関数の合成は二次関数とは限らない
//   同値関係 ~ を a ~ b ⇔ a ≡ b mod 1e9+7 とすると, (Z/~, +, 0) はモノイド ∵ a~b ∧ x~y ⇒ a+x ~ b+y

// 例: セグ木には圏が乗る
//   半開区間全体の集合 {[l,r) | l,r∈Z ∧ l<=r} は, 端点を共有する非交和 [l,m)∪[m,r) = [l,r) について閉じている
//   この集合と演算の組はモノイドでも半群でもないが, セグ木に乗せることができる

// モノイド
#include<concepts>
template<class T> concept Monoid = requires(T x, T::S s) {
  {x.e()}     -> std::same_as<typename T::S>;
  {x.f(s, s)} -> std::same_as<typename T::S>;
};
template<class T> concept ProductMM = requires(T x, T::L::S l, T::R::S r, T::Idx n) {
  requires Monoid<typename T::L>;
  requires Monoid<typename T::R>;
  {x.f(l, r, n)} -> std::same_as<typename T::L::S>;
};
// 実装
template<class T = long long> struct Sum {
  using S = T;
  static inline S e() {return S();};
  static inline S f(S l, S r) {return l + r;}
};
#include<numeric>
template<class T = long long> struct Min {
  using S = T;
  static inline S e() {return std::numeric_limits<S>::max();};
  static inline S f(S l, S r) {return l<r ? l : r;}
};
#include<numeric>
template<class T = long long> struct Max {
  using S = T;
  static inline S e() {return std::numeric_limits<S>::min();};
  static inline S f(S l, S r) {return l>r ? l : r;}
};
template<class T = long long> struct SumPMM {
  struct M {
    using S = T;
    static inline S e() {return S();};
    static inline S f(S l, S r) {return l + r;}
  };
  using Idx = T;
  struct L : M {};
  struct R : M {};
  static inline L::S f(L::S l, R::S r, Idx n) {return l + r * n;}
};
template<class T = long long> struct MaxPMM {
  using Idx = T;
  struct L {
    using S = T;
    static inline S e() {return std::numeric_limits<S>::min();};
    static inline S f(S l, S r) {return l>r ? l : r;}
  };
  struct R {
    using S = T;
    static inline S e() {return S();};
    static inline S f(S l, S r) {return l + r;}
  };
  static inline L::S f(L::S l, R::S r, Idx n) {return l + r;}
};

// セグメント木 (非再帰)
//   モノイド (S, f, e) を扱う
//   半開区間 [l, r) で操作
//   クエリは [0, n) の範囲
//   木の実装は 1-indexed
//   計算量:
//     構築       O(n)
//     一点更新   O(log n)
//     区間クエリ O(log n)
#include<vector>
template<Monoid M> class SegmentTree {
 public:
  using Idx = int;
  SegmentTree(Idx n) : n(geqb(n)), dat(geqb(n) << 1, M::e()) {}
  void update(Idx k, M::S x) {
    dat[k += n] = x;
    while((k = parent(k))) dat[k] = M::f(dat[lchild(k)], dat[rchild(k)]);
  }
  M::S query(Idx a, Idx b) const {
    auto vl = M::e(), vr = M::e();
    for(auto l = a + n, r = b + n; l < r; l = parent(l), r = parent(r)) {
      if(l&1) vl = M::f(vl, dat[l++]);
      if(r&1) vr = M::f(dat[--r], vr);
    }
    return M::f(vl, vr);
  }
 private:
  constexpr Idx geqb(Idx n) const {return n & (n - 1) ? geqb(n + (n & -n)) : n;}
  inline Idx parent(Idx k) const {return (k >> 1);}
  inline Idx lchild(Idx k) const {return (k << 1);}
  inline Idx rchild(Idx k) const {return (k << 1) | 1;}
  Idx n;
  std::vector<typename M::S> dat;
};

// 動的セグメント木
//   モノイド (S, fs, es) を扱う
//   半開区間 [l, r) で操作
//   クエリは [L, R) の範囲
//   計算量:
//     構築       O(1)
//     一点更新   O(log n)
//     区間クエリ O(log n)
#include<memory>
template<Monoid M> class SegmentTreeD {
 public:
  using Idx = long long;
  SegmentTreeD(Idx L, Idx R) : L(L), R(R), root(std::make_unique<Node>(M::e())) {}
  void update(Idx a, M::S x) {update(a, x, root, L, R);}
  M::S query(Idx a, Idx b) {return query(a, b, root, L, R);}
 private:
  struct Node {
    M::S val;
    std::unique_ptr<Node> lchild, rchild;
    Node(M::S val) : val(val), lchild(nullptr), rchild(nullptr) {}
  };
  using Ptr = std::unique_ptr<Node>;
  void update(Idx a, M::S x, const Ptr &n, Idx l, Idx r) {
    if(l+1 == r) {
      n->val = x;
      return;
    }
    if(n->lchild == nullptr) n->lchild = std::make_unique<Node>(M::e());
    if(n->rchild == nullptr) n->rchild = std::make_unique<Node>(M::e());
    auto m = (l + r) / 2;
    if(a < m) update(a, x, n->lchild, l, m);
    else      update(a, x, n->rchild, m, r);
    n->val = M::f(n->lchild->val, n->rchild->val);
  }
  M::S query(Idx a, Idx b, const Ptr &n, Idx l, Idx r) {
    if(n == nullptr)     return M::e();
    if(b <= l || r <= a) return M::e();
    if(a <= l && r <= b) return n->val;
    auto m = (l + r) / 2;
    auto vl = query(a, b, n->lchild, l, m);
    auto vr = query(a, b, n->rchild, m, r);
    return M::f(vl, vr);
  }
  Idx L, R;
  Ptr root;
};

// 遅延伝播セグメント木 (非再帰)
//   直積モノイド (S, fs, es) × (E, fe, ee) と, 作用 fa: S × E → S を扱う
//   半開区間 [l, r) で操作
//   クエリは [0, n) の範囲
//   木の実装は 1-indexed
//   計算量:
//     構築       O(n)
//     一点更新   O(log n)
//     区間更新   O(log n)
//     区間クエリ O(log n)
#include<vector>
template<ProductMM P> class SegmentTreeLP {
 public:
  using Idx = int;
  SegmentTreeLP(Idx n) : N(geqb(n)), H(lsb(geqb(n))), dat(geqb(n) << 1, P::L::e()), laz(geqb(n) << 1, P::R::e()) {}
  void set(Idx a, P::L::S x) {
    thrust(a += N);
    dat[a] = x;
    laz[a] = P::R::e();
    recalc(a);
  }
  void update(Idx a, Idx b, P::R::S x) {
    if(a >= b) return;
    thrust(a += N);
    thrust(b += N - 1);
    for(auto l = a, r = b + 1; l < r; l = parent(l), r = parent(r)) {
      if(l&1) laz[l] = P::R::f(laz[l], x), ++l;
      if(r&1) --r, laz[r] = P::R::f(laz[r], x);
    }
    recalc(a);
    recalc(b);
  }
  P::L::S query(Idx a, Idx b) {
    thrust(a += N);
    thrust(b += N - 1);
    auto vl = P::L::e(), vr = P::L::e();
    for(auto l = a, r = b + 1; l < r; l = parent(l), r = parent(r)) {
      if(l&1) vl = P::L::f(vl, reflect(l++));
      if(r&1) vr = P::L::f(reflect(--r), vr);
    }
    return P::L::f(vl, vr);
  }
 private:
  constexpr Idx geqb(Idx n) const {return n & (n - 1) ? geqb(n + (n & -n)) : n;}
  constexpr Idx leqb(Idx n) const {return n & (n - 1) ? leqb(n & (n - 1)) : n;}
  inline Idx lsb(Idx n) const {return n & 1 ? 0 : 1 + lsb(n >> 1);}
  inline Idx parent(Idx k) const {return (k >> 1);}
  inline Idx lchild(Idx k) const {return (k << 1);}
  inline Idx rchild(Idx k) const {return (k << 1) | 1;}
  inline P::L::S reflect(Idx k) const {return laz[k]==P::R::e() ? dat[k] : P::f(dat[k], laz[k], leqb(k)>>1);}
  inline void propagate(Idx k) {
    if(laz[k] == P::R::e()) return;
    laz[lchild(k)] = P::R::f(laz[lchild(k)], laz[k]);
    laz[rchild(k)] = P::R::f(laz[rchild(k)], laz[k]);
    dat[k] = reflect(k);
    laz[k] = P::R::e();
  }
  inline void thrust(Idx k) {for(auto i = H; i; --i) propagate(k >> i);}
  inline void recalc(Idx k) {while(k >>= 1) dat[k] = P::L::f(reflect(lchild(k)), reflect(rchild(k)));}
  Idx N, H;
  std::vector<typename P::L::S> dat;
  std::vector<typename P::R::S> laz;
};

// 遅延伝播動的セグメント木
//   直積モノイド (S, fs, es) × (E, fe, ee) と, 作用 fa: S × E → S を扱う
//   半開区間 [l, r) で操作
//   クエリは [L, R) の範囲
//   計算量:
//     構築       O(1)
//     一点更新   O(log n)
//     区間更新   O(log n)
//     区間クエリ O(log n)
//     二分探索   O(log^2 n)
#include<memory>
template<ProductMM P> class SegmentTreeDLP {
 public:
  using Idx = long long;
  SegmentTreeDLP(Idx L, Idx R) : L(L), R(R), root(std::make_unique<Node>(P::L::e(), P::R::e(), L, R)) {}
  void set(Idx a, P::L::S x) {set(a, x, root, L, R);}
  void update(Idx a, Idx b, P::R::S x) {update(a, b, x, root, L, R);}
  P::L::S query(Idx a, Idx b) {return query(a, b, root, L, R);}
 private:
  struct Node {
    P::L::S val;
    P::R::S laz;
    Idx L, R;
    std::unique_ptr<Node> lchild, rchild;
    Node(P::L::S val, P::R::S laz, Idx L, Idx R) : val(val), laz(laz), L(L), R(R), lchild(nullptr), rchild(nullptr) {}
  };
  using Ptr = std::unique_ptr<Node>;
  inline P::L::S reflect(const Ptr& n) {return n->laz==P::R::e() ? n->val : P::f(n->val, n->laz, n->R-n->L);}
  inline void propagate(const Ptr& n) {
    auto m = (n->L + n->R) / 2;
    if(not n->lchild) n->lchild = std::make_unique<Node>(P::L::e(), P::R::e(), n->L, m);
    if(not n->rchild) n->rchild = std::make_unique<Node>(P::L::e(), P::R::e(), m, n->R);
    if(n->laz == P::R::e()) return;
    n->lchild->laz = P::R::f(n->lchild->laz, n->laz);
    n->rchild->laz = P::R::f(n->rchild->laz, n->laz);
    n->val = reflect(n);
    n->laz = P::R::e();
  }
  inline void recalc(const Ptr& n) {n->val = P::L::f(reflect(n->lchild), reflect(n->rchild));}
  void set(Idx a, P::L::S x, const Ptr &n, Idx l, Idx r) {
    propagate(n);
    if(l+1 == r) {
      n->val = x;
      return;
    }
    auto m = (l + r) / 2;
    if(a < m) set(a, x, n->lchild, l, m);
    else      set(a, x, n->rchild, m, r);
    recalc(n);
  }
  void update(Idx a, Idx b, P::R::S x, const Ptr &n, Idx l, Idx r) {
    propagate(n);
    if(b <= l || r <= a) return;
    if(a <= l && r <= b) {
      n->laz = P::R::f(n->laz, x);
      propagate(n);
      return;
    }
    auto m = (l + r) / 2;
    update(a, b, x, n->lchild, l, m);
    update(a, b, x, n->rchild, m, r);
    recalc(n);
  }
  P::L::S query(Idx a, Idx b, const Ptr &n, Idx l, Idx r) {
    propagate(n);
    if(b <= l || r <= a) return P::L::e();
    if(a <= l && r <= b) return reflect(n);
    auto m = (l + r) / 2;
    auto vl = query(a, b, n->lchild, l, m);
    auto vr = query(a, b, n->rchild, m, r);
    return P::L::f(vl, vr);
  }
  Idx L, R;
  Ptr root;
};

#include<bits/stdc++.h>
using namespace std;

int main() {
  const int N = 8;
  const vector<int> v{10,7,13,9,11,8,12,10};
  // セグ木
  SegmentTree<Sum<>> RSQ(N);
  SegmentTree<Min<>> RMQ(N);
  // 動的セグ木
  SegmentTreeD<Max<>> DRMQ(0, N);
  // 遅延セグ木
  SegmentTreeLP<SumPMM<>> RSAQ(N);
  SegmentTreeLP<MaxPMM<>> RMAQ(N);
  // 遅延動的セグ木
  SegmentTreeDLP<SumPMM<>> DRSAQ(0, N);

  // 初期化
  cout << "Array:";
  for(auto i=0; i<N; ++i) {
    cout << " " << v[i];
    RSQ.update(i, v[i]);
    RMQ.update(i, v[i]);
    DRMQ.update(i, v[i]);
    RSAQ.set(i, v[i]);
    RMAQ.set(i, v[i]);
    DRSAQ.set(i, v[i]);
  }
  cout << "\n" << endl;

  // 区間加算
  for(auto p: vector<pair<int,int>>{{2,6}}) {
    RSAQ.update(p.first, p.second, 10);
    RMAQ.update(p.first, p.second, 10);
    DRSAQ.update(p.first, p.second, 10);
  }
  cout<<"Add: ";for(auto i=0; i<N; ++i) {cout<<" "<<RMAQ.query(i,i+1);} cout << "\n" << endl;

  // クエリ
  for(auto p: vector<pair<int,int>>{{0,N}, {0,N/2}, {N/2,N}}) {
    cout << "Query:   " << p.first << ", " << p.second    << "\n"
         << "  RSQ:   " << RSQ.query(p.first, p.second)   << "\n"
         << "  RMQ:   " << RMQ.query(p.first, p.second)   << "\n"
         << "  DRMQ:  " << DRMQ.query(p.first, p.second)  << "\n"
         << "  RSAQ:  " << RSAQ.query(p.first, p.second)  << "\n"
         << "  DRSAQ: " << DRSAQ.query(p.first, p.second) << "\n"
         << "  RMAQ:  " << RMAQ.query(p.first, p.second)  << "\n"
         << endl;
  }
}
