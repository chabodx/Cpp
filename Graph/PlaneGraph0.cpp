// PlaneGraph

/*
使い方:
  前準備:
    LabelとWeightを定義
      typedef int Label;
      typedef double Weight;
    頂点と辺の構造体を定義
      struct Vertex {Label label; double x, y;};
      struct Edge {Label from, to; Weight weight;};
  使用方法:
    頂点と辺をtemplate引数として宣言
      PlaneGraph<Vertex, Edge> G;
    頂点を追加
      G.insert(const Vertex&);
    辺を追加
      G.insert(const Edge&);
    双対グラフを作成
      auto D = G.dualgraph(const Weight&);
    単一始点最短距離を計算
      auto D = G.Dijkstra(const Label&);
注意点:
  Label:
    デフォルトコンストラクタ・operator<・前置インクリメントが必要
  Weight:
    デフォルトコンストラクタ・operator<・operator+が必要
  Vertex:
    publicなデータメンバとしてLabel label; double x, y;が必要
  Edge:
    publicなデータメンバとしてLabel from, to; Weight weight;が必要
*/

////////////////////////////////////////
// Type.
typedef int Label;
typedef double Weight;
struct Vertex {Label label; double x, y;};
struct Edge {Label from, to; Weight weight;};
////////////////////////////////////////
// Library.
#include<iostream>
#include<cassert>
#include<algorithm>
#include<cmath>
#include<map>
#include<queue>
#include<vector>
#include<functional>
// Class.
template<typename Vertex, typename Edge>
class PlaneGraph {
public:
  void insert(const Vertex& v);
  void insert(Edge e);
  const PlaneGraph<Vertex, Edge> dualgraph(const Weight& weight = Weight()) const;
  const std::map<Label, Weight> Dijkstra(const Label& start) const;
  // For debug.
  void print() {
    std::cout << "edges (from-to:weight)" << std::endl;
    for(const auto& list: E) {
      for(const auto& e: list.second) std::cout << " " << e.from << "-" << e.to << ":" << e.weight;
      std::cout << std::endl;
    }
  }
private:
  void dualgraph_color(std::map<Label, std::map<Label, Label>>& color, const Label& c, const Edge& e, const std::map<Label, std::vector<Edge>>& edge) const;
  std::map<Label, Vertex> V;
  std::map<Label, std::vector<Edge>> E;
};
// Implement.
template<typename Vertex, typename Edge>
void PlaneGraph<Vertex, Edge>::insert(const Vertex& v) {
  V[v.label] = v;
}
template<typename Vertex, typename Edge>
void PlaneGraph<Vertex, Edge>::insert(Edge e) {
  assert(V.find(e.from) != std::end(V));
  assert(V.find(e.to) != std::end(V));
  E[e.from].push_back(e);
  std::swap(e.from, e.to);
  E[e.from].push_back(e);
}
template<typename Vertex, typename Edge>
const PlaneGraph<Vertex, Edge> PlaneGraph<Vertex, Edge>::dualgraph(const Weight& weight) const {
  // Sort edges counterclockwise.
  auto edge = E;
  for(auto& list: edge) if(!list.second.empty()) {
    std::sort(std::begin(list.second), std::end(list.second), [&](const Edge& lhs, const Edge& rhs) {
      double l = std::atan2(V.at(lhs.to).y - V.at(lhs.from).y, V.at(lhs.to).x - V.at(lhs.from).x);
      double r = std::atan2(V.at(rhs.to).y - V.at(rhs.from).y, V.at(rhs.to).x - V.at(rhs.from).x);
      return l < r;
    });
    list.second.push_back(list.second.front());
  }
  // Color edges.
  std::map<Label, std::map<Label, Label>> color;
  Label c = Label();
  for(const auto& list: edge) for(const auto& e: list.second) if(!color[e.from].count(e.to)) {
    dualgraph_color(color, c, e, edge);
    ++c;
  }
  // Construct the dualgraph.
  PlaneGraph<Vertex, Edge> result;
  std::vector<std::vector<bool>> used(c, std::vector<bool>(c, false));
  for(Label i = Label(); i < c; ++i) result.insert(Vertex({i}));
  for(const auto& list: E) for(const auto& e: list.second) if(!used[color[e.from][e.to]][color[e.to][e.from]]) if(color[e.from][e.to] != color[e.to][e.from]) {
    used[color[e.from][e.to]][color[e.to][e.from]] = true;
    used[color[e.to][e.from]][color[e.from][e.to]] = true;
    result.insert(Edge({color[e.from][e.to], color[e.to][e.from], weight}));
  }
  return result;
}
template<typename Vertex, typename Edge>
void PlaneGraph<Vertex, Edge>::dualgraph_color(std::map<Label, std::map<Label, Label>>& color, const Label& c, const Edge& e, const std::map<Label, std::vector<Edge>>& edge) const {
  color[e.from][e.to] = c;
  for(auto it = edge.at(e.to).rbegin(); it != edge.at(e.to).rend(); ++it) if(it->to == e.from) {
    ++it;
    if(!color[it->from].count(it->to)) dualgraph_color(color, c, *it, edge);
    break;
  }
}
template<typename Vertex, typename Edge>
const std::map<Label, Weight> PlaneGraph<Vertex, Edge>::Dijkstra(const Label& start) const {
  std::map<Label, Weight> result;
  std::priority_queue<Edge, std::vector<Edge>, std::function<bool(const Edge&, const Edge&)>> q([](const Edge& lhs, const Edge& rhs) {return lhs.weight > rhs.weight;});
  Edge s;
  s.from = s.to = start;
  s.weight = Weight();
  q.push(s);
  while(!q.empty()) {
    auto cur = q.top();
    q.pop();
    if(result.count(cur.to)) continue;
    result[cur.to] = cur.weight;
    for(const auto& e: E.at(cur.to)) if(!result.count(e.to)) {
      Edge nex;
      nex.from = e.from;
      nex.to = e.to;
      nex.weight = cur.weight + e.weight;
      q.push(nex);
    }
  }
  return result;
}
////////////////////////////////////////
int main() {
  PlaneGraph<Vertex, Edge> G;
  Vertex a({1,  0,  0}), b({2, 1,  0}), c({3,  0, 1});
  Vertex d({4, -1, -1}), e({5, 2, -1}), f({6, -1, 2});
  G.insert(a);
  G.insert(b);
  G.insert(c);
  G.insert(d);
  G.insert(e);
  G.insert(f);
  G.insert(Edge({1, 2, 1}));
  G.insert(Edge({2, 3, 1}));
  G.insert(Edge({3, 1, 1}));
  G.insert(Edge({4, 5, 1}));
  G.insert(Edge({5, 6, 1}));
  G.insert(Edge({6, 4, 1}));
  G.insert(Edge({1, 4, 1}));
  auto D = G.dualgraph(1);
  D.print();
}
