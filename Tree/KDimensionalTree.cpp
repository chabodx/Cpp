// K-Dimensional Tree
// KD-Tree
// KD木

// テンプレート引数にはランダムアクセス可能で連続メモリを確保するようなコンテナを渡す.

// 実装メモ:
//   クエリは半開区間 [l, r) で操作する.
//   KDTree::Node
//     point             点の座標
//     lchild, rchild    子へのポインタ
//     size, low, high   部分木のサイズ, ノードに対応する領域の各方向成分の最小値, 最大値
// 計算量:
//   領域内の点の個数    O(K * log^K N)
//   領域内の点の列挙    O(列挙数 * K * log^K N)

#include<bits/stdc++.h>
using namespace std;

// ポイント出力用
#include<iostream>
#include<vector>
template<typename T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& p) {
  os << "(";
  for(auto i=0; i<p.size(); ++i) os << (i ? " " : "") << p[i];
  return os << ")";
}

// K-Dimensional Tree
#include<algorithm>
#include<functional>
#include<memory>
#include<vector>
template<typename Point> class KDTree {
 public:
  KDTree(int K, std::vector<Point> P) : K(K), ROOT(build(P, 0, P.size(), 0)) {
    // 個数用
    Point l(K), h(K);
    for(auto k=0; k<K; ++k) l[k] = (*std::min_element(begin(P), end(P), Compare(k)))[k];
    for(auto k=0; k<K; ++k) h[k] = (*std::max_element(begin(P), end(P), Compare(k)))[k];
    std::function<int(const std::unique_ptr<Node>&, Point, Point, int)> dfs = [&](const auto& n, auto l, auto h, auto k) {
      n->low=l; n->high=h; n->size=1;
      auto nl=l, nh=h; auto nk=(k+1)%K;
      nl[k] = nh[k] = n->point[k];
      if(n->lchild) n->size += dfs(n->lchild,l,nh,nk);
      if(n->rchild) n->size += dfs(n->rchild,nl,h,nk);
      return n->size;
    };
    dfs(ROOT, l, h, 0);
  }
  int query(const Point& a, const Point& b) const {return query(a, b, ROOT, 0);}
  void debug() const {
    std::function<void(const std::unique_ptr<Node>&, int)> dfs = [&](const auto& n, auto d) {
      if(n->rchild) dfs(n->rchild, d+1);
      for(auto i=0; i<d; ++i) std::cout<<"    ";
      std::cout << n->point << n->size << n->low << n->high << std::endl;
      if(n->lchild) dfs(n->lchild, d+1);
    };
    dfs(ROOT, 0);
  }
 private:
  struct Node {
    Point point;
    std::unique_ptr<Node> lchild, rchild;
    // 個数用
    int size; Point low, high;
  };
  struct Compare {
    Compare(int k) : k(k) {}
    bool operator()(const Point& lhs, const Point& rhs) const {return lhs[k] < rhs[k];}
    int k;
  };
  std::unique_ptr<Node> build(std::vector<Point>& P, int l, int h, int k) const {
    if(!(l < h)) return nullptr;
    auto m = (l + h) / 2;
    std::sort(begin(P)+l, begin(P)+h, Compare(k));
    auto res = std::make_unique<Node>();
    res->point  = P[m];
    res->lchild = build(P, l,   m, (k+1)%K);
    res->rchild = build(P, m+1, h, (k+1)%K);
    return res;
  };
  int query(const Point& a, const Point& b, const std::unique_ptr<Node>& n, int k) const {
    if(!n) return 0;
    // 個数用
    auto ok = true;
    for(auto k=0; k<K; ++k) if(Compare(k)(n->low,a) || Compare(k)(b,n->high)) ok = false;
    if(ok) return n->size;
    // 列挙用
    auto res = 1;
    for(auto k=0; k<K; ++k) if(Compare(k)(n->point,a) || !Compare(k)(n->point,b)) res = 0;
    if(a[k] <= n->point[k]) res += query(a, b, n->lchild, (k+1)%K);
    if(n->point[k] <= b[k]) res += query(a, b, n->rchild, (k+1)%K);
    return res;
  }
  int K;
  std::unique_ptr<Node> ROOT;
};

#include<bits/stdc++.h>
using namespace std;

// using Point = vector<int>;
struct Point {
  int id;
  vector<int> dat;
  int& operator[](int n) {return dat[n];}
  const int& operator[](int n) const {return dat[n];}
  Point() {}
  Point(int n) : dat(n) {}
  Point(const initializer_list<int>& init) : dat(init.begin(), init.end()) {}
  friend ostream& operator<<(ostream& os, const Point& p) {
    os << "(";
    for(auto i=0; i<p.dat.size(); ++i) os << (i ? " " : "") << p[i];
    return os << ")";
  }
};

int main() {
  vector<Point> p;
  p.push_back({2, 2});
  p.push_back({4, 4});
  p.push_back({1, 1});
  p.push_back({3, 3});
  KDTree<Point> kdtree(2, p);

  kdtree.debug();

  cout<<kdtree.query({0,0},{5,5})<<endl;
}
