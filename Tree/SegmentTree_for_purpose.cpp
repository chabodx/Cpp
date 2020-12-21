// 特定用途セグメント木

// Range Sum Query
//   遅延伝播セグメント木で実装.
//   半開区間 [l, r) で操作する.
//   木の実装は 1-indexed.
//   計算量:
//     構築       O(n)
//     区間加算   O(log n)
//     区間和     O(log n)
#include<vector>
template<typename T> class SegmentTree {
 public:
  SegmentTree(int n) : n(size(n)), dat(size(n) << 1), laz(size(n) << 1) {}
  void add(int a, int b, T v) {add(a, b, v, 1, 0, n);}
  T sum(int a, int b) {return sum(a, b, 1, 0, n);}
 private:
  constexpr int size(int n) const {return n & (n - 1) ? size(n + (n & -n)) : n;}
  inline int lchild(int k) const {return (k << 1);}
  inline int rchild(int k) const {return (k << 1) | 1;}
  inline void propagate(int k, int l, int r) {
    if(k < n) {
      laz[lchild(k)] += laz[k];
      laz[rchild(k)] += laz[k];
    }
    dat[k] += (r - l) * laz[k];
    laz[k] = 0;
  }
  T add(int a, int b, T v, int k, int l, int r) {
    propagate(k, l, r);
    if(b <= l || r <= a) return dat[k];
    if(a <= l && r <= b) return dat[k] + (r - l) * (laz[k] = v);
    auto m = (l + r) >> 1;
    auto vl = add(a, b, v, lchild(k), l, m);
    auto vr = add(a, b, v, rchild(k), m, r);
    return dat[k] = vl + vr;
  }
  T sum(int a, int b, int k, int l, int r) {
    propagate(k, l, r);
    if(b <= l || r <= a) return 0;
    if(a <= l && r <= b) return dat[k];
    auto m = (l + r) >> 1;
    auto vl = sum(a, b, lchild(k), l, m);
    auto vr = sum(a, b, rchild(k), m, r);
    return vl + vr;
  }
  int n;
  std::vector<T> dat, laz;
};

// Range Sum Query
//   遅延伝播動的セグメント木で実装.
//   半開区間 [l, r) で操作する.
//   木の実装は 1-indexed.
//   実行時間の目安 (ABC136F):
//     0.88s 遅延伝播セグメント木 + 座標圧縮
//     1.65s 遅延伝播動的セグメント木 (生ポインタ)
//     2.25s 遅延伝播動的セグメント木 (unique_ptr)
//     1.70s 抽象化動的セグメント木
//     2.09s 抽象化遅延伝播動的セグメント木 (生ポインタ)
//   計算量:
//     構築       O(1)
//     一点更新   O(1)
//     区間加算   O(log n)
//     区間和     O(log n)
#include<memory>
template<typename S, typename E = S> class SegmentTreeDLP {
 public:
  using Size = long long;
  SegmentTreeDLP(Size L, Size R, S es=0, E ee=0) : L(L), R(R), es(es), ee(ee), root(std::make_unique<Node>(es,ee)) {}
  void set(Size a, S x) {set(a, x, root, L, R);}
  void add(Size a, Size b, E x) {add(a, b, x, root, L, R);}
  S query(Size a, Size b) {return query(a, b, root, L, R);}
 private:
  struct Node {
    S val; E laz;
    Node *lchild, *rchild;
    Node(S val, E laz) : val(val), laz(laz), lchild(nullptr), rchild(nullptr) {}
  };
  inline S fs(S l, S r) const {return l + r;}
  inline S fa(S l, E r, Size len) const {return l + r * len;}
  inline E fe(E l, E r) const {return l + r;}
  inline S reflect(const auto& n, Size len) {return n->laz==ee ? n->val : fa(n->val, n->laz, len);}
  inline void propagate(const auto& n, Size len) {
    if(n->lchild == nullptr) n->lchild = new Node(es, ee);
    if(n->rchild == nullptr) n->rchild = new Node(es, ee);
    if(n->laz == ee) return;
    n->lchild->laz = fe(n->lchild->laz, n->laz);
    n->rchild->laz = fe(n->rchild->laz, n->laz);
    n->val = reflect(n, len);
    n->laz = ee;
  }
  inline void recalc(const auto& n, Size len) {n->val = fs(reflect(n->lchild, len), reflect(n->rchild, len));}
  void set(Size a, E x, const auto &n, Size l, Size r) {
    propagate(n, r-l);
    if(l+1 == r) {
      n->val = x;
      return;
    }
    auto m = (l + r) / 2;
    if(a < m) set(a, x, n->lchild, l, m);
    else      set(a, x, n->rchild, m, r);
    recalc(n, r-l);
  }
  void add(Size a, Size b, E x, const auto &n, Size l, Size r) {
    propagate(n, r-l);
    if(b <= l || r <= a) return;
    if(a <= l && r <= b) {
      n->laz = fe(n->laz, x);
      propagate(n, r-l);
      return;
    }
    auto m = (l + r) / 2;
    add(a, b, x, n->lchild, l, m);
    add(a, b, x, n->rchild, m, r);
    recalc(n, r-l);
  }
  S query(Size a, Size b, const auto &n, Size l, Size r) {
    propagate(n, r-l);
    if(b <= l || r <= a) return es;
    if(a <= l && r <= b) return reflect(n, r-l);
    auto m = (l + r) / 2;
    auto vl = query(a, b, n->lchild, l, m);
    auto vr = query(a, b, n->rchild, m, r);
    return fs(vl, vr);
  }
  Size L, R;
  std::unique_ptr<Node> root;
  S es; E ee;
};

#include<bits/stdc++.h>
using namespace std;
int main() {
}
