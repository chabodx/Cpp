// 抽象化セグメント木

// 解説: https://yosupo.hatenablog.com/entry/2023/12/09/010658
// 解説: https://qiita.com/keymoon/items/0f929a19ed30f34ae6e8
// 解説: https://elliptic-shiho.github.io/segtree/segtree.pdf

// ざっくり用語
//   半群:     結合法則をみたすような, 集合 S と関数 f: S × S → S のペア
//   モノイド: 結合法則と単位元の存在をみたすような, 集合 S と関数 f: S × S → S と単位元の 3 つ組
// 例
//   一次関数全体 {λx.ax+b | a,b∈Z} は, 関数合成によりモノイド (F, ◦, id) をなす
//   二次関数全体 {λx.ax^2+bx+c | a,b,c∈Z} は, 関数合成により半群をなさない ∵ 二次関数の合成は二次関数とは限らない
//   同値関係 ~ を a ~ b ⇔ a ≡ b mod 1e9+7 とすると, (Z/~, +, 0) はモノイド ∵ a~b ∧ x~y ⇒ a+x ~ b+y

// セグ木: 圏が乗る
//   半開区間全体の集合 {[l,r) | l,r∈Z ∧ l<=r} は, 端点を共有する非交和 [l,m)∪[m,r) = [l,r) について閉じている
//   この集合と演算の組はモノイドでも半群でもないが, セグ木に乗せることができる

// 双対セグ木: 区間更新と点取得. モノイドが乗る
//   直積モノイドはモノイドの圏における直積対象, モノイドの自由積はモノイドの圏における直和対象
//   クエリの併用のためには, セグ木は直積半群, 双対セグ木は自由積, を用いるので圏論的な双対関係にある

// 遅延セグ木: 区間一様代入更新と区間取得. 半群についての自己準同型モノイドへのモノイド準同型を使った構造が乗る
//   遅延伝播構造 (M, S, φ) := モノイド M と半群 S とモノイド準同型 φ: M → End(S) の組
//   M についての自己準同型モノイド := モノイド M の自己準同型の全体と関数合成によるモノイド (End(M), ◦, id)
//   S についての自己準同型モノイド :=     半群 S の自己準同型の全体と関数合成によるモノイド (End(S), ◦, id)
//   M による X へのモノイド作用 := モノイド (M, ・, e) と集合 X に対して, 以下をみたすような演算 ⋆: M × X → X
//     任意の f, g ∈ M と x ∈ X について, (f・g) ⋆ x = f ⋆ (g ⋆ x)
//     任意の x ∈ X について, e ⋆ x = x
//   遅延伝播構造は加群の一般化である
//     加群, ベクトル空間, 半環, 可換環, 体, は遅延セグ木に乗る
//     整数環を乗せれば, 区間一様乗算更新と区間和取得
//     トロピカル半環を乗せれば, 区間一様加算更新と区間最小値取得
//   遅延伝播構造 (M, S, φ) には常に区間の情報を付け加えることができる
//     半群 T, 関数 φ': M → End(S × T) s.t. φ'(f)(x, t) = (φ(f)(x), t) に対して, (M, S ✕ T, φ') は遅延伝播構造である
//     ※区間の情報を扱うのに, x ∈ S の自然数倍を繰り返し二乗法などで計算すると, クエリの計算量が悪化 → O(log^2 n)
//   正規な遅延伝播構造 := 遅延伝播構造 (M, S, φ) s.t. 商モノイド M/φ とモノイド M が同型
//   次のような (M1 ＋ M2, S, φ) は遅延伝播構造である
//     M1, M2 を M1 ∩ M2 = Φ をみたすモノイドとし, (M1, S, φ1), (M2, S, φ2) を遅延伝播構造とする
//     和集合 M1 ∪ M2 からの関数 φ': (M1 ∪ M2) → End(S) を次のように定義する
//       φ'(f)(x) = φ1(f)(x)   (f ∈ M1)
//                   φ2(f)(x)   (f ∈ M2)
//     自由積 M1 ＋ M2 からの関数 φ: (M1 ＋ M2) × S → S を次のように定義する
//       φ((f0, f1, ..., f_{k-1}))(x) = φ'(f0)(φ'(f1)(...(φ'(f_{k-1})(x))...))
//   遅延伝播積 M1 ⋈ M2 := ((M1 × M2) / φ⋈, ⊙, (e1, e2))
//     (M1, ・1, e1), (M2, ・2, e2) をモノイドとし, S を半群とし, (M1, S, φ1), (M2, S, φ2) を遅延伝播構造とする
//     任意の f ∈ M1 と g ∈ M2 に対して, f' ∈ M1 と g' ∈ M2 が存在して φ2(g) ◦ φ1(f) = φ1(f) ◦ φ2(g) をみたすとする
//       φ⋈: M1 × M2 → End(S) を φ⋈(f, g) = φ1(f) ◦ φ2(g) で定義
//       ⊙: (M1 × M2) / φ⋈ × (M1 × M2) / φ⋈ → (M1 × M2) / φ⋈ を [(f1, f2)] ⊙ [(g1, g2)] = [(f1 ・1 g1', f2' ・2 g2)] で定義
//     このとき, ((M1 × M2) / φ⋈, ⊙, [(e1, e2)]) はモノイドである

// 遅延セグ木: 区間一様代入更新と区間更新と区間取得. (M ⋈ L, S × N, φ') が乗る
//   遅延伝播構造 (M, S, φ) とし, S についての左零半群に単位元を付与してできるモノイドを L = (S ∪ {eL}, ・L, eL) とおく
//   S と自然数の加法についての半群 N との直積半群 S × N について,
//   関数 φ1: M → End(S × N) を φ1(f)(x, k) = (φ1(f), k) で定義し, 関数 φ2: L → End(S × N) を次のように定義する
//     φ2(g)(x, k) = (x, k)     (g = e)
//                    (k g, k)   (g ≠ e)
//   このとき, (M, S × N, φ1) と (L, S × N, φ2) は遅延伝播構造である
//   さらに, 遅延伝播積 M ⋈ L が存在し, 自然に定義される関数 φ' によって (M ⋈ L, S × N, φ') は遅延伝播構造である

// Segment Tree Beats: 区間 chminmax と区間 sum. 各頂点に持たせるのは, 区間和, 区間最大値, 最大値の個数, 二番目に大きな値

// 遅延セグ木: 復元クエリ. (M, S × S, φ) が乗る
//   M = ({0, 1}, +, 0) を自明な半群 {1} に単位元 0 を付け加えてできるモノイドとし, S を半群とする
//   φ: M → End(S × S) を次のように定義する
//     φ(f)(x, y) = (x, y)    (f = 0)
//                   (y, y)    (f = 1)
//   復元クエリ自体は, 双対セグ木の一様代入クエリで実現できる

// 遅延伝播構造の半群 S を (Z, +) とすると, End(S) = {λx.kx | k ∈ Z} が成り立つ
//   λx.x+k (k ≠ 0) は (Z, +) の準同型ではないため, 区間加算更新や区間和取得は不可能
// 遅延伝播構造の半群 S を (R^n, +) とすると, End(S) ≃ M が成り立つ
//    R を可換環とし, n を正整数とし, n 次正方行列の全体のなす環 Mn(R) の乗法についてのモノイド (Mn(R), ・, I) を M とする
// 遅延伝播構造の半群 S を (Z, max) とすると, End(S) ≃ {f: Z → Z | f は単調} が成り立つ
//    関数 f が単調である := x ≤ y ⇒ f(x) ≤ f(y) for ∀x, ∀y ∈ Z
// 遅延伝播構造の半群 S を (Z × (N ＼ {0}), ・) とすると, 最大値とその個数を数えるクエリを処理できる
//   演算 ・: (Z × (N ＼ {0})) × (Z × (N ＼ {0})) → Z × (N ＼ {0}) を次のように定義する
//     (x, k) ・ (y, k') = (x, k)        (x > y)
//                         (y, k')       (x < y)
//                         (x, k + k')   (x = y)
//   このとき, 任意の f: Z × (N ＼ {0}) → Z × (N ＼ {0}) に対して次が成り立つ
//     f は半群 S の準同型である ⇔ ∃単調関数 g: Z → N ＼ {0} s.t. f(x, k) = (g(x), k h(x)) for ∀(x, y) ∈ Z × (N ＼ {0})

// モノイド
#include<concepts>
template<class T> concept Monoid = requires(T x, T::S s) {
  {x.e()}     -> std::same_as<typename T::S>;
  {x.f(s, s)} -> std::same_as<typename T::S>;
};
template<class T> concept MonoidLP = requires(T x, T::L::S l, T::R::S r, T::Idx n) {
  requires Monoid<typename T::L>;
  requires Monoid<typename T::R>;
  {x.f(l, r, n)} -> std::same_as<typename T::L::S>;
};
// 実装
struct Sum {
  using S = long long;
  static inline S e() {return S();};
  static inline S f(S l, S r) {return l + r;}
};
#include<numeric>
struct Min {
  using S = long long;
  static inline S e() {return std::numeric_limits<S>::max();};
  static inline S f(S l, S r) {return l<r ? l : r;}
};
#include<numeric>
struct Max {
  using S = long long;
  static inline S e() {return std::numeric_limits<S>::min();};
  static inline S f(S l, S r) {return l>r ? l : r;}
};
struct SumLP {
  struct M {
    using S = long long;
    static inline S e() {return S();};
    static inline S f(S l, S r) {return l + r;}
  };
  using Idx = long long;
  using L = M;
  using R = M;
  static inline L::S f(L::S l, R::S r, Idx n) {
    if(r == R::e()) return l;
    if(l == L::e()) return r * n;
    return l + r * n;
  }
};
#include<numeric>
struct MaxLP {
  using Idx = long long;
  struct L {
    using S = long long;
    static inline S e() {return std::numeric_limits<S>::min();};
    static inline S f(S l, S r) {return l>r ? l : r;}
  };
  struct R {
    using S = long long;
    static inline S e() {return S();};
    static inline S f(S l, S r) {return l + r;}
  };
  static inline L::S f(L::S l, R::S r, Idx n) {
    if(r == R::e()) return l;
    if(l == L::e()) return r;
    return l + r;
  }
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
//   半群 (S, fs, es), モノイド (M, fm, em), モノイド準同型 fa: M → End(S) を扱う
//                                        ⇔ モノイド作用 fa: M × S → S
//   半開区間 [l, r) で操作
//   クエリは [0, n) の範囲
//   木の実装は 1-indexed
//   計算量:
//     構築       O(n)
//     一点更新   O(log n)
//     区間更新   O(log n)
//     区間クエリ O(log n)
#include<vector>
template<MonoidLP M> class SegmentTreeLP {
 public:
  SegmentTreeLP(M::Idx n) : N(geqb(n)), H(lsb(geqb(n))), dat(geqb(n) << 1, M::L::e()), laz(geqb(n) << 1, M::R::e()) {}
  void set(M::Idx a, M::L::S x) {
    thrust(a += N);
    dat[a] = x;
    laz[a] = M::R::e();
    recalc(a);
  }
  void update(M::Idx a, M::Idx b, M::R::S x) {
    if(a >= b) return;
    thrust(a += N);
    thrust(b += N - 1);
    for(auto l = a, r = b + 1; l < r; l = parent(l), r = parent(r)) {
      if(l&1) laz[l] = M::R::f(laz[l], x), ++l;
      if(r&1) --r, laz[r] = M::R::f(laz[r], x);
    }
    recalc(a);
    recalc(b);
  }
  M::L::S query(M::Idx a, M::Idx b) {
    thrust(a += N);
    thrust(b += N - 1);
    auto vl = M::L::e(), vr = M::L::e();
    for(auto l = a, r = b + 1; l < r; l = parent(l), r = parent(r)) {
      if(l&1) vl = M::L::f(vl, reflect(l++));
      if(r&1) vr = M::L::f(reflect(--r), vr);
    }
    return M::L::f(vl, vr);
  }
 private:
  constexpr M::Idx geqb(M::Idx n) const {return n & (n - 1) ? geqb(n + (n & -n)) : n;}
  constexpr M::Idx leqb(M::Idx n) const {return n & (n - 1) ? leqb(n & (n - 1)) : n;}
  inline M::Idx lsb(M::Idx n) const {return n & 1 ? 0 : 1 + lsb(n >> 1);}
  inline M::Idx parent(M::Idx k) const {return (k >> 1);}
  inline M::Idx lchild(M::Idx k) const {return (k << 1);}
  inline M::Idx rchild(M::Idx k) const {return (k << 1) | 1;}
  inline M::L::S reflect(M::Idx k) const {return M::f(dat[k], laz[k], leqb(k)>>1);}
  inline void propagate(M::Idx k) {
    if(laz[k] == M::R::e()) return;
    laz[lchild(k)] = M::R::f(laz[lchild(k)], laz[k]);
    laz[rchild(k)] = M::R::f(laz[rchild(k)], laz[k]);
    dat[k] = reflect(k);
    laz[k] = M::R::e();
  }
  inline void thrust(M::Idx k) {for(auto i = H; i; --i) propagate(k >> i);}
  inline void recalc(M::Idx k) {while(k >>= 1) dat[k] = M::L::f(reflect(lchild(k)), reflect(rchild(k)));}
  M::Idx N, H;
  std::vector<typename M::L::S> dat;
  std::vector<typename M::R::S> laz;
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
template<MonoidLP M> class SegmentTreeDLP {
 public:
  SegmentTreeDLP(M::Idx L, M::Idx R) : L(L), R(R), root(std::make_unique<Node>(M::L::e(), M::R::e(), L, R)) {}
  void set(M::Idx a, M::L::S x) {set(a, x, root, L, R);}
  void update(M::Idx a, M::Idx b, M::R::S x) {update(a, b, x, root, L, R);}
  M::L::S query(M::Idx a, M::Idx b) {return query(a, b, root, L, R);}
 private:
  struct Node {
    M::L::S val;
    M::R::S laz;
    M::Idx L, R;
    std::unique_ptr<Node> lchild, rchild;
    Node(M::L::S val, M::R::S laz, M::Idx L, M::Idx R) : val(val), laz(laz), L(L), R(R), lchild(nullptr), rchild(nullptr) {}
  };
  using Ptr = std::unique_ptr<Node>;
  inline M::L::S reflect(const Ptr& n) {return M::f(n->val, n->laz, n->R-n->L);}
  inline void propagate(const Ptr& n) {
    auto m = (n->L + n->R) / 2;
    if(not n->lchild) n->lchild = std::make_unique<Node>(M::L::e(), M::R::e(), n->L, m);
    if(not n->rchild) n->rchild = std::make_unique<Node>(M::L::e(), M::R::e(), m, n->R);
    if(n->laz == M::R::e()) return;
    n->lchild->laz = M::R::f(n->lchild->laz, n->laz);
    n->rchild->laz = M::R::f(n->rchild->laz, n->laz);
    n->val = reflect(n);
    n->laz = M::R::e();
  }
  inline void recalc(const Ptr& n) {n->val = M::L::f(reflect(n->lchild), reflect(n->rchild));}
  void set(M::Idx a, M::L::S x, const Ptr &n, M::Idx l, M::Idx r) {
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
  void update(M::Idx a, M::Idx b, M::R::S x, const Ptr &n, M::Idx l, M::Idx r) {
    propagate(n);
    if(b <= l || r <= a) return;
    if(a <= l && r <= b) {
      n->laz = M::R::f(n->laz, x);
      propagate(n);
      return;
    }
    auto m = (l + r) / 2;
    update(a, b, x, n->lchild, l, m);
    update(a, b, x, n->rchild, m, r);
    recalc(n);
  }
  M::L::S query(M::Idx a, M::Idx b, const Ptr &n, M::Idx l, M::Idx r) {
    propagate(n);
    if(b <= l || r <= a) return M::L::e();
    if(a <= l && r <= b) return reflect(n);
    auto m = (l + r) / 2;
    auto vl = query(a, b, n->lchild, l, m);
    auto vr = query(a, b, n->rchild, m, r);
    return M::L::f(vl, vr);
  }
  M::Idx L, R;
  Ptr root;
};

#include<bits/stdc++.h>
using namespace std;

int main() {
  const int N = 8;
  const vector<int> v{10,7,13,9,11,8,12,10};
  // セグ木
  SegmentTree<Sum> RSQ(N);
  SegmentTree<Min> RMQ(N);
  // 動的セグ木
  SegmentTreeD<Max> DRMQ(0, N);
  // 遅延セグ木
  SegmentTreeLP<SumLP> RSAQ(N);
  SegmentTreeLP<MaxLP> RMAQ(N);
  // 遅延動的セグ木
  SegmentTreeDLP<SumLP> DRSAQ(0, N);

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
