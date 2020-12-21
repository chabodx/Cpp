// 領域木
// 二次元セグメント木

// 各ノードが矩形領域に対応するような完全二分木.
// 深さが偶数のノードは, 横に二分割した領域を子に対応させる.
// 深さが奇数のノードは, 縦に二分割した領域を子に対応させる.

// 領域木
//   モノイド (T, f), 単位元 e で構築する.
//   半開区間 [l, r) で操作する.
//   木の実装は 1-indexed.
//   計算量:
//     構築       O(X Y)
//     一点更新   O(log X log Y)
//     区間クエリ O(log^2 X log^2 Y)
#include<functional>
#include<vector>
template<typename T> class RangeTree {
 public:
  using Size = int;
  using F = std::function<T(T, T)>;
  RangeTree(Size X, Size Y, const T& e, F f) : X(X), Y(Y), dat(size(X, Y), e), e(e), f(f) {}
  void update(Size x, Size y, const T& v) {update(x, y, v, 0, 0, X, Y, 1, false);}
  T query(Size minx, Size miny, Size maxx, Size maxy) const {return query(minx, miny, maxx, maxy, 0, 0, X, Y, 1, false);}
 private:
  constexpr Size size(Size n) const {return n & (n - 1) ? size(n + (n & -n)) : n;}
  constexpr Size size(Size x, Size y) const {return (size(x) * size(y)) << 1;}
  inline Size lchild(Size n) const {return (n << 1) | 0;}
  inline Size rchild(Size n) const {return (n << 1) | 1;}
  T update(Size x, Size y, const T& v, Size lx, Size ly, Size hx, Size hy, Size n, bool odd) {
    if(x<lx || hx<=x || y<ly || hy<=y) return dat[n];
    if(1+lx==hx && 1+ly==hy)           return dat[n] = v;
    auto nlx=lx, nly=ly, nhx=hx, nhy=hy;
    if(odd) nly = nhy = (ly + hy) >> 1;
    else    nlx = nhx = (lx + hx) >> 1;
    auto l = update(x, y, v, lx, ly, nhx, nhy, lchild(n), !odd);
    auto h = update(x, y, v, nlx, nly, hx, hy, rchild(n), !odd);
    return dat[n] = f(l, h);
  }
  T query(Size minx, Size miny, Size maxx, Size maxy, Size lx, Size ly, Size hx, Size hy, Size n, bool odd) const {
    if(maxx<=lx || hx<=minx || maxy<=ly || hy<=miny) return e;
    if(minx<=lx && hx<=maxx && miny<=ly && hy<=maxy) return dat[n];
    auto nlx=lx, nly=ly, nhx=hx, nhy=hy;
    if(odd) nly = nhy = (ly + hy) >> 1;
    else    nlx = nhx = (lx + hx) >> 1;
    auto l = query(minx, miny, maxx, maxy, lx, ly, nhx, nhy, lchild(n), !odd);
    auto h = query(minx, miny, maxx, maxy, nlx, nly, hx, hy, rchild(n), !odd);
    return f(l, h);
  }
  Size X, Y;
  F f;
  T e;
  std::vector<T> dat;
};

#include<bits/stdc++.h>
using namespace std;

int main() {
  RangeTree<int> mintree(1000, 1000, 1e9, [](auto l, auto r){return min(l,r);});
  RangeTree<int> sumtree(1000, 1000, 0, [](auto l, auto r){return l+r;});
}
