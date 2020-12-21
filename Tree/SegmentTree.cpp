// 抽象化セグメント木

// 解説: https://beet-aizu.hatenablog.com/entry/2019/11/27/125906
// 解説: https://qiita.com/ningenMe/items/bf66de877e3b97d35862
// 解説: https://algo-logic.info/segment-tree/

// 用語
//   半群:     結合法則をみたすような, 集合 S と写像 f: S × S → S のペア
//   モノイド: 結合法則と単位元の存在をみたすような, 集合 S と写像 f: S × S → S のペア

// セグメント木
//   モノイド (S, fs), 単位元 es で構築する.
//   半開区間 [l, r) で操作する.
//   クエリは [0, n) の範囲.
//   木の実装は 1-indexed.
//   計算量:
//     構築       O(n)
//     一点更新   O(log n)
//     区間クエリ O(log n)
#include<functional>
#include<vector>
template<typename S> class SegmentTree {
 public:
  using FS = std::function<S(S, S)>;
  SegmentTree(int n, FS fs, S es) : n(size(n)), fs(fs), es(es), dat(size(n) << 1, es) {}
  void update(int k, S x) {
    dat[k += n] = x;
    while((k = parent(k))) dat[k] = fs(dat[lchild(k)], dat[rchild(k)]);
  }
  S query(int a, int b) const {
    auto vl = es, vr = es;
    for(auto l = a + n, r = b + n; l < r; l = parent(l), r = parent(r)) {
      if(l&1) vl = fs(vl, dat[l++]);
      if(r&1) vr = fs(dat[--r], vr);
    }
    return fs(vl, vr);
  }
 private:
  constexpr int size(int n) const {return n & (n - 1) ? size(n + (n & -n)) : n;}
  inline int parent(int k) const {return (k >> 1);}
  inline int lchild(int k) const {return (k << 1);}
  inline int rchild(int k) const {return (k << 1) | 1;}
  int n;
  FS fs;
  S es;
  std::vector<S> dat;
};

// 動的セグメント木
//   モノイド (S, fs), 単位元 es で構築する.
//   半開区間 [l, r) で操作する.
//   クエリは [L, R) の範囲.
//   計算量:
//     構築       O(1)
//     一点更新   O(log n)
//     区間クエリ O(log n)
#include<functional>
#include<memory>
template<typename S> class SegmentTreeD {
 public:
  using Size = long long;
  using FS = std::function<S(S, S)>;
  SegmentTreeD(Size L, Size R, FS fs, S es) : L(L), R(R), fs(fs), es(es), root(std::make_unique<Node>(es)) {}
  void set(Size a, S x) {set(a, x, root, L, R);}
  S query(Size a, Size b) {return query(a, b, root, L, R);}
 private:
  struct Node {
    S val;
    std::unique_ptr<Node> lchild, rchild;
    Node(S val) : val(val), lchild(nullptr), rchild(nullptr) {}
  };
  using Ptr = std::unique_ptr<Node>;
  void set(Size a, S x, const Ptr &n, Size l, Size r) {
    if(l+1 == r) {
      n->val=x;
      return;
    }
    if(n->lchild == nullptr) n->lchild = std::make_unique<Node>(es);
    if(n->rchild == nullptr) n->rchild = std::make_unique<Node>(es);
    auto m = (l + r) / 2;
    if(a < m) set(a, x, n->lchild, l, m);
    else      set(a, x, n->rchild, m, r);
    n->val = fs(n->lchild->val, n->rchild->val);
  }
  S query(Size a, Size b, const Ptr &n, Size l, Size r) {
    if(n == nullptr)     return es;
    if(b <= l || r <= a) return es;
    if(a <= l && r <= b) return n->val;
    auto m = (l + r) / 2;
    auto vl = query(a, b, n->lchild, l, m);
    auto vr = query(a, b, n->rchild, m, r);
    return fs(vl, vr);
  }
  Size L, R;
  FS fs;
  S es;
  Ptr root;
};

// 遅延伝播セグメント木
//   モノイド (S, fs), (E, fe) と作用 fa: S × E → S で構築する.
//   半開区間 [l, r) で操作する.
//   クエリは [0, n) の範囲.
//   木の実装は 1-indexed.
//   計算量:
//     構築       O(n)
//     一点更新   O(log n)
//     区間更新   O(log n)
//     区間クエリ O(log n)
#include<functional>
#include<vector>
template<typename S, typename E = S> class SegmentTreeLP {
 public:
  using FS = std::function<S(S, S)>;
  using FA = std::function<S(S, E)>;
  using FE = std::function<E(E, E)>;
  SegmentTreeLP(int n, FS fs, FA fa, FE fe, S es, E ee) : n(size(n)), height(lsb(size(n))), fs(fs), fa(fa), fe(fe), es(es), ee(ee), dat(size(n) << 1, es), laz(size(n) << 1, ee) {}
  void set(int a, S x) {
    thrust(a += n);
    dat[a] = x;
    laz[a] = ee;
    recalc(a);
  }
  void update(int a, int b, E x) {
    if(a >= b) return;
    thrust(a += n);
    thrust(b += n - 1);
    for(auto l = a, r = b + 1; l < r; l = parent(l), r = parent(r)) {
      if(l&1) laz[l] = fe(laz[l], x), ++l;
      if(r&1) --r, laz[r] = fe(laz[r], x);
    }
    recalc(a);
    recalc(b);
  }
  S query(int a, int b) {
    thrust(a += n);
    thrust(b += n - 1);
    auto vl = es, vr = es;
    for(auto l = a, r = b + 1; l < r; l = parent(l), r = parent(r)) {
      if(l&1) vl = fs(vl, reflect(l++));
      if(r&1) vr = fs(reflect(--r), vr);
    }
    return fs(vl, vr);
  }
 private:
  constexpr int size(int n) const {return n & (n - 1) ? size(n + (n & -n)) : n;}
  inline int lsb(int n) {return n & 1 ? 0 : 1 + lsb(n >> 1);}
  inline int parent(int k) const {return (k >> 1);}
  inline int lchild(int k) const {return (k << 1);}
  inline int rchild(int k) const {return (k << 1) | 1;}
  inline S reflect(int k) const {return laz[k]==ee ? dat[k] : fa(dat[k], laz[k]);}
  inline void propagate(int k) {
    if(laz[k] == ee) return;
    laz[lchild(k)] = fe(laz[lchild(k)], laz[k]);
    laz[rchild(k)] = fe(laz[rchild(k)], laz[k]);
    dat[k] = reflect(k);
    laz[k] = ee;
  }
  inline void thrust(int k) {for(auto i = height; i; --i) propagate(k >> i);}
  inline void recalc(int k) {while(k >>= 1) dat[k] = fs(reflect(lchild(k)), reflect(rchild(k)));}
  int n, height;
  FS fs;
  FA fa;
  FE fe;
  S es;
  E ee;
  std::vector<S> dat;
  std::vector<E> laz;
};

// 遅延伝播動的セグメント木
//   モノイド (S, fs), (E, fe) と作用 fa: S × E → S で構築する.
//   半開区間 [l, r) で操作する.
//   クエリは [L, R) の範囲.
//   計算量:
//     構築       O(1)
//     一点更新   O(log n)
//     区間更新   O(log n)
//     区間クエリ O(log n)
//     二分探索   O(log^2 n)
#include<functional>
#include<iostream>
#include<memory>
template<typename S, typename E = S> class SegmentTreeDLP {
 public:
  using Size = long long;
  using FS = std::function<S(S, S)>;
  using FA = std::function<S(S, E)>;
  using FE = std::function<E(E, E)>;
  SegmentTreeDLP(Size L, Size R, FS fs, FA fa, FE fe, S es, E ee) : L(L), R(R), fs(fs), fa(fa), fe(fe), es(es), ee(ee), root(std::make_unique<Node>(es,ee)) {}
  void set(Size a, S x) {set(a, x, root, L, R);}
  void update(Size a, Size b, E x) {update(a, b, x, root, L, R);}
  S query(Size a, Size b) {return query(a, b, root, L, R);}
  Size lower_bound(Size a, Size b, const std::function<bool(S)>& check) {return lower_bound(a, b, check, root, L, R);}
  void debug() {std::cout << "segtree: size " << L << " to " << R << std::endl; traverse(root, 0, L, R);}
 private:
  struct Node {
    S val;
    E laz;
    std::unique_ptr<Node> lchild, rchild;
    Node(S val, E laz) : val(val), laz(laz), lchild(nullptr), rchild(nullptr) {}
  };
  using Ptr = std::unique_ptr<Node>;
  void traverse(const Ptr& node, Size d, Size l, Size r) {
    if(not node->lchild || not node->rchild) return;
    propagate(node);
    auto m = (l + r) / 2;
    if(l+1!=r) traverse(node->rchild, d + 1, m, r);
    for(auto i = 0; i < d; ++i) std::cout << "    ";
    std::cout << "[" << l << "," << r << ") ";
    std::cout << node->val << std::endl;
    if(l+1!=r) traverse(node->lchild, d + 1, l, m);
  }
  inline S reflect(const Ptr& n) {return n->laz==ee ? n->val : fa(n->val, n->laz);}
  inline void propagate(const Ptr& n) {
    if(not n->lchild) n->lchild = std::make_unique<Node>(es, ee);
    if(not n->rchild) n->rchild = std::make_unique<Node>(es, ee);
    if(n->laz == ee) return;
    n->lchild->laz = fe(n->lchild->laz, n->laz);
    n->rchild->laz = fe(n->rchild->laz, n->laz);
    n->val = reflect(n);
    n->laz = ee;
  }
  inline void recalc(const Ptr& n) {n->val = fs(reflect(n->lchild), reflect(n->rchild));}
  void set(Size a, S x, const Ptr &n, Size l, Size r) {
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
  void update(Size a, Size b, E x, const Ptr &n, Size l, Size r) {
    propagate(n);
    if(b <= l || r <= a) return;
    if(a <= l && r <= b) {
      n->laz = fe(n->laz, x);
      propagate(n);
      return;
    }
    auto m = (l + r) / 2;
    update(a, b, x, n->lchild, l, m);
    update(a, b, x, n->rchild, m, r);
    recalc(n);
  }
  S query(Size a, Size b, const Ptr &n, Size l, Size r) {
    propagate(n);
    if(b <= l || r <= a) return es;
    if(a <= l && r <= b) return reflect(n);
    auto m = (l + r) / 2;
    auto vl = query(a, b, n->lchild, l, m);
    auto vr = query(a, b, n->rchild, m, r);
    return fs(vl, vr);
  }
  Size lower_bound(Size a, Size b, const std::function<bool(S)>& check, const Ptr& node, Size l, Size r) {
    propagate(node);
    if(b <= l || r <= a) return R;
    if(!check(reflect(node))) return R;
    if(l + 1 == r) return l;
    auto m = (l + r) / 2;
    auto i = lower_bound(a, b, check, node->lchild, l, m);
    if(i != R) return i;
    return lower_bound(a, b, check, node->rchild, m, r);
  }
  Size L, R;
  FS fs;
  FA fa;
  FE fe;
  S es;
  E ee;
  Ptr root;
};

#include<bits/stdc++.h>
using namespace std;

using LL = long long;
struct S {
  LL v, size;
  S(LL v, LL size = 1) : v(v), size(size) {}
  bool operator==(const S& o) const {return v != o.v;}
  friend ostream& operator<<(ostream& os, const S& s) {return os << s.v;}
};
using E = LL;

void RangeMinimumQuery() {
  cout << endl << "**** Range Minimum Query ****" << endl << endl;
  SegmentTree<int> RMQ(10, [](auto l, auto r) {return min(l, r);}, 1e9);
  RMQ.update(0,1000); RMQ.update(3,10); RMQ.update(5,100);
  cout<<RMQ.query(1,5)<<endl<<endl;
  // 遅延セグ木は必要な要素を初期化しておく.
  auto fs = [](auto l, auto r) {return min(l, r);};
  auto sum = [](auto l, auto r) {return l + r;};
  SegmentTreeLP<long long> LP(10, fs,sum,sum,1e18,0);
  for(auto i=0; i<10; ++i) LP.set(i, 0);
  LP.update(0,10,10); LP.update(2,8,100); LP.update(4,6,1);
  for(auto i=0; i<8; ++i) {
    cout<<"min("<<i<<", j):  ";
    for(auto j=i+1; j<=8; ++j) cout<<" "<<LP.query(i, j);
    cout<<endl;
  }
  cout<<endl;
  SegmentTreeDLP<long long> DLP(0, 10, fs,sum,sum,1e18,0);
  for(auto i=0; i<10; ++i) DLP.set(i, 0);
  DLP.update(0,10,10); DLP.update(2,8,100); DLP.update(4,6,1);
  for(auto i=0; i<8; ++i) {
    cout<<"min("<<i<<", j):  ";
    for(auto j=i+1; j<=8; ++j) cout<<" "<<DLP.query(i, j);
    cout<<endl;
  }
}

void RangeSumAddQuery() {
  cout << endl << "**** Range Sum/Add Query ****" << endl << endl;
  auto fs = [](S l, S r) {return S{l.v + r.v, l.size + r.size};};
  auto fa = [](S l, E r) {return S{l.v + r * l.size, l.size};};
  auto fe = [](E l, E r) {return l + r;};
  auto es = S(0, 1);
  auto ee = E(0);
  SegmentTreeLP<S,E> LP(8, fs,fa,fe,es,ee);
  LP.update(0,8,1000); LP.update(1,6,100); LP.update(2,7,10); LP.update(3,5,1);
  for(auto i=0; i<8; ++i) {
    cout<<"sum("<<i<<", j):  ";
    for(auto j=i+1; j<=8; ++j) cout<<" "<<LP.query(i, j).v;
    cout<<endl;
  }
  cout<<endl;
  // 動的セグ木で区間sumを使う場合は必要な要素を初期化しておく.
  SegmentTreeDLP<S, E> DLP(0, 8, fs,fa,fe,es,ee);
  for(auto i=0; i<8; ++i) DLP.update(i, i+1, ee);
  DLP.update(0,8,1000); DLP.update(1,6,100); DLP.update(2,7,10); DLP.update(3,5,1);
  for(auto i=0; i<8; ++i) {
    cout<<"sum("<<i<<", j):  ";
    for(auto j=i+1; j<=8; ++j) cout<<" "<<DLP.query(i, j).v;
    cout<<endl;
  }
  cout<<"lower_bound(0,8), 1110< x: "<<DLP.lower_bound(0,8,[](auto x){return 1110<x.v;})<<endl;
  cout<<"lower_bound(0,8), 1110<=x: "<<DLP.lower_bound(0,8,[](auto x){return 1110<=x.v;})<<endl;
  DLP.debug();
}

void RangeSumUpdateQuery() {
  cout << endl << "**** Range Sum/Update Query ****" << endl << endl;
  auto fs = [](S l, S r) {return S{l.v + r.v, l.size + r.size};};
  auto fa = [](S l, E r) {return S{r * l.size, l.size};};
  auto fe = [](E l, E r) {return r;};
  auto es = S(0, 1);
  auto ee = E(1e18);
  SegmentTreeLP<S,E> LP(8, fs,fa,fe,es,ee);
  LP.update(1,4,1); LP.update(2,5,-2);
  for(auto i=0; i<8; ++i) {
    cout<<"sum("<<i<<", j):  ";
    for(auto j=i+1; j<=8; ++j) cout<<" "<<LP.query(i, j).v;
    cout<<endl;
  }
  cout<<endl;
  // 動的セグ木で区間sumを使う場合は必要な要素を初期化しておく.
  SegmentTreeDLP<S, E> DLP(0, 8, fs,fa,fe,es,ee);
  for(auto i=0; i<8; ++i) DLP.update(i, i+1, ee);
  DLP.update(1,4,1); DLP.update(2,5,-2);
  for(auto i=0; i<8; ++i) {
    cout<<"sum("<<i<<", j):  ";
    for(auto j=i+1; j<=8; ++j) cout<<" "<<DLP.query(i, j).v;
    cout<<endl;
  }
  DLP.debug();
}

int main() {
  RangeMinimumQuery();
  RangeSumAddQuery();
  RangeSumUpdateQuery();
}
