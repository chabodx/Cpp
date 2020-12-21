// Dynamic K-Dimensional Tree
// 動的 k-d tree

// https://topcoder.g.hatena.ne.jp/spaghetti_source/20120908/1347059626

// 頂点の追加 := 二分探索木の要領で追加
// 頂点の削除 := 削除フラグを立てるだけ

// バランスが崩れたときに部分木を再構築することで平衡化
//   バランスが崩れた 1 := 3 * (理想の高さ) < (実際の高さ) => その頂点を根とする部分木を再構築
//   バランスが崩れた 2 := 過半数の点が削除された          => 木全体を再構築

// 計算量:
//   再構築:               O(|V| log |V|)
//   ある空間内の点の列挙: O((空間内の点数) + log |V|)
// ならし計算量:
//   追加:                 O(log |V|)
//   削除:                 O(log |V|)

// 手抜き: search(low, high) において high < low だとバグる

#include<algorithm>
#include<cmath>
#include<memory>
#include<vector>

using Size = unsigned int;

template<typename T> class KDTree {
 public:
  using Point = std::vector<T>;

  KDTree(Size d, std::vector<Point> v);
  void insert(const Point& point);
  void erase(const Point& point);
  Size search(const Point& low, const Point& high) const;

 private:
  struct Node;
  struct Compare;

  std::unique_ptr<Node> build(std::vector<Point>& v, Size b, Size e, Size d);
  void rebuild(std::unique_ptr<Node>& node, Size d);
  void update(const std::unique_ptr<Node>& node);
  void flatten(const std::unique_ptr<Node>& node, std::vector<Point>& v);
  Size size(const std::unique_ptr<Node>& node) const;
  Size height(const std::unique_ptr<Node>& node) const;
  Size next(Size d) const;
  void insert(std::unique_ptr<Node>& node, const Point& point, Size d);
  void erase(std::unique_ptr<Node>& node, const Point& point, Size d);
  Size search(const Point& low, const Point& high, const std::unique_ptr<Node>& node, Size d) const;

  const Size D;
  Size removed_;
  std::unique_ptr<Node> root_;
};

template<typename T> struct KDTree<T>::Node {
  Point point;
  Size size;
  Size height;
  bool removed;
  std::unique_ptr<Node> l_child, r_child;
  explicit Node(const Point& point) : point(point), size(1), height(1), removed(false) {}
};

template<typename T> struct KDTree<T>::Compare {
  Size d;
  Compare(Size d) : d(d) {}
  bool operator()(const Point& lhs, const Point& rhs) const {return lhs[d] < rhs[d];}
};

template<typename T> KDTree<T>::KDTree(Size d, std::vector<Point> v) : D(d), removed_(0) {root_ = build(v, 0, v.size(), 0);}
template<typename T> void KDTree<T>::insert(const Point& point) {insert(root_, point, 0);}
template<typename T> void KDTree<T>::erase(const Point& point) {erase(root_, point, 0);}
template<typename T> Size KDTree<T>::search(const Point& low, const Point& high) const {return search(low, high, root_, 0);}

template<typename T> std::unique_ptr<typename KDTree<T>::Node> KDTree<T>::build(std::vector<Point>& v, Size b, Size e, Size d) {
  if(e <= b) return nullptr;
  auto m = (b + e) / 2;
  nth_element(std::begin(v) + b, std::begin(v) + m, std::begin(v) + e, Compare(d));
  auto n = std::make_unique<Node>(v[m]);
  n->l_child = build(v, b, m, next(d));
  n->r_child = build(v, m + 1, e, next(d)); 
  update(n);
  return n;
}
template<typename T> void KDTree<T>::rebuild(std::unique_ptr<Node>& node, Size d) {
  std::vector<Point> v;
  flatten(node, v);
  removed_ -= size(node) - v.size();
  node = build(v, 0, v.size(), d);
}
template<typename T> void KDTree<T>::update(const std::unique_ptr<Node>& node) {
  if(!node) return;
  node->size = 1 + size(node->l_child) + size(node->r_child);
  node->height = 1 + std::max(height(node->l_child), height(node->r_child));
}
template<typename T> void KDTree<T>::flatten(const std::unique_ptr<Node>& node, std::vector<Point>& v) {
  if(!node) return;
  flatten(node->l_child, v);
  if(!node->removed) v.emplace_back(node->point);
  flatten(node->r_child, v);
}
template<typename T> Size KDTree<T>::size(const std::unique_ptr<Node>& node) const {return node ? node->size : 0;}
template<typename T> Size KDTree<T>::height(const std::unique_ptr<Node>& node) const {return node ? node->height : 0;}
template<typename T> Size KDTree<T>::next(Size d) const {return (d + 1) % D;}
template<typename T> void KDTree<T>::insert(std::unique_ptr<Node>& node, const Point& point, Size d) {
  if(!node) {
    node = std::make_unique<Node>(point);
    return;
  }
  if(Compare(d)(point, node->point)) insert(node->l_child, point, next(d));
  else                               insert(node->r_child, point, next(d));
  update(node);
  if(3 * std::log2(size(node)) < height(node)) rebuild(node, d);
}
template<typename T> void KDTree<T>::erase(std::unique_ptr<Node>& node, const Point& point, Size d) {
  if(!node) return;
  if(!node->removed && node->point == point) {
    node->removed = true;
    ++removed_;
    if(2 * removed_ > size(root_)) rebuild(root_, 0);
    return;
  }
  if(Compare(d)(point, node->point)) erase(node->l_child, point, next(d));
  else                               erase(node->r_child, point, next(d));
}
template<typename T> Size KDTree<T>::search(const Point& low, const Point& high, const std::unique_ptr<Node>& node, Size d) const {
  if(!node) return 0;
  auto res = 1;
  if(node->removed) res = 0;
  else for(auto i = 0; i < D; ++i) if(Compare(d)(node->point, low) || Compare(d)(high, node->point)) res = 0;
  if(!Compare(d)(node->point, low))  res += search(low, high, node->l_child, next(d));
  if(!Compare(d)(high, node->point)) res += search(low, high, node->r_child, next(d));
  return res;
}

#include<bits/stdc++.h>
using namespace std;

// void traverse(auto& n, int d=0){
//   if(!n)return;
//   traverse(n->l_child, d+1);
//   for(auto i=0;i<2*d;++i)cout<<" ";
//   if(n->removed)cout<<"removed:";
//   cout<<n->point[0]<<","<<n->point[1]<<endl;
//   traverse(n->r_child, d+1);
// }

int main() {
  vector<vector<int>> p;
  p.push_back({2, 2});
  p.push_back({4, 4});
  p.push_back({1, 1});
  p.push_back({3, 3});
  KDTree<int> kdtree(2, p);

//   traverse(kdtree.root_);

  kdtree.erase({2,2});

  //   vector<vector<int>> v;
//   kdtree.flatten(kdtree.root_, v);
//   for(auto i: v) cout<<" ("<<i[0]<<","<<i[1]<<")";cout<<endl;

  cout<<kdtree.search({0,0},{5,5})<<endl;
}
