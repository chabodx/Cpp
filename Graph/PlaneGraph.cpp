// 平面グラフ

// 解説: https://xuzijian629.hatenablog.com/entry/2019/12/14/163726
// 定義:
//   頂点数 := n
//   辺数   := e
//   面数   := f
//   グラフ G に以下の操作を行いグラフ H と同型にできるとき, H は G のマイナーであるという.
//     (1) 辺の除去
//     (2) 辺の縮約
//     (3) 孤立点の除去
//   もとのグラフと任意のマイナーに共通する性質を minor-closed であるという.
//   グラフ G の頂点集合の分割 A, B, C について, 以下をみたすような C を G のセパレータであるという.
//     (1) G には A, B を直接結ぶ辺が存在しない.
//     (2) A, B の頂点重みはそれぞれ G の頂点重みの 2/3 以下である.
// オイラーの公式: n - e + f = 2
// 定理: 3 頂点以上の単純な平面グラフでは e <= 3 * n - 6
// 定理: 平面的であることは minor-closed な性質である.
// 定理: 平面グラフの木幅は O(sqrt(n)).
// 定理: 平面グラフの辺は O(n) * O(n) グリッド上に頂点を配置することで線分のみで描画可能.
// 定理: 平面グラフが同型かどうかは線形時間で判定可能.
// 定理: ある k について最小交差数が k 以下かどうかは線形時間で判定可能.
// 定理 (Boyer): あるグラフが平面的グラフかどうかは O(n) で判定可能.
// 定理 (Kuratowski): 平面グラフである ⇔ 完全グラフ K5 と完全二部グラフ K3,3 をマイナーに含まない.
// 定理 (Lipton & Tarjan): 平面グラフは sqrt(n) 個のセパレータを含み, それは O(n) で構築可能.
// 定理 (A simple MAX-CUT algorithm for planar graphs): 最大カットを O(n^1.5 log n) で計算可能.

// insert(vertex)  頂点を追加 O(1)
// insert(u,v)     辺を追加 O(1)
// dualgraph()     双対グラフを作成 O(|V| + |E| log |E|)
//                   解説: http://kanetai.hatenablog.com/entry/20140321/1395414246
//                   双対グラフの頂点の座標は多角形の重心にしてある.
//                   隣接リストをソートするので他の関数との兼ね合いに注意.
//                   外側領域を求めたい場合:
//                     (1) 最小 x 座標の頂点 v を求める.
//                     (2) G[v] を偏角順にソート.
//                     (3) PI < |arg(E[G[v][i+1]] - arg(E[G[v][i]])| であるような E[i] が外側領域の一部である.

#include<algorithm>
#include<cassert>
#include<cmath>
#include<functional>
#include<iostream>
#include<unordered_map>
#include<unordered_set>
#include<vector>

struct Vertex {double x, y;};

struct Edge {
  int from, to, rev;
  Edge(int from, int to, int rev) : from(from), to(to), rev(rev) {}
};

class PlaneGraph {
 public:
  int size() const {return V.size();}
  Vertex& vertex(int i) {return V.at(i);}
  void insert(const Vertex& v) {
    V.emplace_back(v);
    G.resize(V.size());
  }
  void insert(int u, int v) {
    assert(u<V.size() && v<V.size());
    G[u].emplace_back(E.size());
    G[v].emplace_back(E.size() + 1);
    E.emplace_back(u, v, E.size() + 1);
    E.emplace_back(v, u, E.size() - 1);
  }
  void debug() const {
    std::cout<<"*Vertices*"<<std::endl;
    for(auto i=0; i<V.size(); ++i) {std::cout<<i<<": ("<<V[i].x<<", "<<V[i].y<<")"<<std::endl;}
    std::cout<<"*Edges*"<<std::endl;
    for(auto i=0; i<E.size(); ++i) {auto e=E[i]; std::cout<<i<<": "<<e.from<<" to " <<e.to<<", rev = "<<e.rev<<", arg = "<<arg(e)<<std::endl;}
    std::cout<<"*Adjacency List*"<<std::endl;
    for(auto i=0; i<G.size(); ++i) {std::cout<<i<<":"; for(auto j: G[i]) std::cout<<" "<<j; std::cout<<std::endl;}
  }
  PlaneGraph dualgraph() {
    // Sort edges by angle.
    std::vector<int> pre(E.size());
    for(auto& list: G) {
      std::sort(std::begin(list), std::end(list), [&](auto l, auto r) {return arg(E[l]) < arg(E[r]);});
      for(auto i=0; i<list.size(); ++i) pre[list[(i+1)%list.size()]] = list[i];
    }
    // Color edges.
    std::vector<std::unordered_map<int, int>> color(G.size());
    int c = 0;
    std::function<void(const Edge&)> dfs = [&](const auto& e) {
      color[e.from][e.to] = c;
      const auto& nex = E[pre[e.rev]];
      if(!color[nex.from].count(nex.to)) dfs(nex);
    };
    for(const auto& list: G) for(auto i: list) if(!color[E[i].from].count(E[i].to)) dfs(E[i]), ++c;
    // Construct the dualgraph.
    PlaneGraph H;
    for(auto i=0; i<c; ++i) H.insert(Vertex());
    std::vector<int> cnt(c);
    for(const auto& e: E) {
      auto i = color[e.from][e.to];
      H.vertex(i).x += V[e.from].x;
      H.vertex(i).y += V[e.from].y;
      ++cnt[i];
    }
    for(auto i=0; i<c; ++i) H.vertex(i).x /= cnt[i], H.vertex(i).y /= cnt[i];
    std::vector<std::unordered_set<int>> used(c);
    for(const auto& list: G) for(auto i: list) {
      auto u = color[E[i].from][E[i].to], v = color[E[i].to][E[i].from];
      if(u == v || used[u].count(v) || used[v].count(u)) continue;
      used[u].emplace(v);
      used[v].emplace(u);
      H.insert(u, v);
    }
    return H;
  }
 private:
  inline double arg(const Edge& e) const {return std::atan2(V[e.to].y - V[e.from].y, V[e.to].x - V[e.from].x);}
  std::vector<Vertex> V;
  std::vector<Edge> E;
  std::vector<std::vector<int>> G;
};

#include<bits/stdc++.h>
using namespace std;
int main() {
  PlaneGraph G;
  G.insert({0, 0});
  G.insert({1, 0});
  G.insert({1, 1});
  G.insert({1, 2});
  G.insert({2, 0});
  G.insert(0, 1);
  G.insert(0, 2);
  G.insert(0, 3);
  G.insert(1, 4);
  G.insert(2, 4);
  G.insert(3, 4);
  auto H = G.dualgraph();
  G.debug();
  cout<<endl<<"-------- dualgraph --------"<<endl<<endl;
  H.debug();
}
