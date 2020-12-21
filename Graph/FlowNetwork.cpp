// フローネットワーク

// 無向グラフの最小費用流を計算する場合は頂点数を2倍にして対応する
//   例: 元々の無向グラフ
//         u - v
//   例: 頂点数を2倍にした有向グラフ
//           → xx ←
//         u    ↓    v
//           ← yy →

#include<algorithm>
#include<cfloat>
#include<functional>
#include<queue>
#include<vector>

using Vertex = int;
using Flow   = long double;
using Cost   = long double;

const Flow FLOW_INF = LDBL_MAX;
const Cost COST_INF = LDBL_MAX;

class FlowNetwork {
 public:
  explicit FlowNetwork(int);
  void insert(Vertex, Vertex, Flow, Cost);
  Flow maximum_flow(Vertex, Vertex) const;
  Cost minimum_cost_flow(Vertex, Vertex, Flow) const;
 private:
  struct Edge {
    Vertex from, to;
    Flow capacity;
    Cost cost;
    int rev;
    Edge(Vertex from, Vertex to, Flow capacity, Cost cost, int rev) : from(from), to(to), capacity(capacity), cost(cost), rev(rev) {}
  };
  int size_;
  std::vector<std::vector<Edge>> edge_;
};

// コンストラクタ
FlowNetwork::FlowNetwork(int n) : size_(n), edge_(n) {}

// 辺を追加; 容量0コスト-cの逆辺も追加する
void FlowNetwork::insert(Vertex from, Vertex to, Flow capacity = 0, Cost cost = 0) {
  edge_.at(from).emplace_back(from, to, capacity, cost, edge_.at(to).size());
  edge_.at(to).emplace_back(to, from, 0, -cost, edge_.at(from).size() - 1);
}

// 最大流: Dinic O(|E| |V|^2)
Flow FlowNetwork::maximum_flow(Vertex source, Vertex sink) const {
  std::vector<int> level;   // 各頂点に対するsourceからの距離
  std::vector<int> itr;     // 各頂点に対して、itr[v]番目以前の辺は使っても無駄
  auto residue = edge_;     // 残余グラフ
  // sourceから各頂点までの距離を計算し、sinkまでの距離を返す
  auto bfs = [&]() {
    level.assign(size_, -1);
    level.at(source) = 0;
    std::queue<int> q;
    q.push(source);
    while(!q.empty()) {
      auto v = q.front();
      q.pop();
      for(const auto& e: residue.at(v)) if(!~level.at(e.to)) if(0 < e.capacity) {
        level.at(e.to) = level.at(e.from) + 1;
        q.push(e.to);
      }
    }
    return level.at(sink);
  };
  // バックトラックを利用してフローを流す
  std::function<Flow(Vertex, Flow)> dfs = [&](Vertex v, Flow cur) {
    if(v == sink) return cur;
    for(auto& i = itr.at(v); i < residue.at(v).size(); ++i) {
      auto& e = residue.at(v).at(i);
      if(level.at(e.from) < level.at(e.to)) if(0 < e.capacity) {
        auto f = dfs(e.to, std::min(cur, e.capacity));
        if(f == 0) continue;
        e.capacity -= f;
        residue.at(e.to).at(e.rev).capacity += f;
        return f;
      }
    }
    return Flow(0);
  };
  // Dinic
  Flow result = 0;
  while(~bfs()) {
    itr.assign(size_, 0);
    while(auto f = dfs(source, FLOW_INF)) result += f;
  }
  return result;
}

// 最小費用流: Primal-Dual O(F |E| log |V|)
Cost FlowNetwork::minimum_cost_flow(Vertex source, Vertex sink, Flow f) const {
  std::vector<Cost> h(size_, 0);  // 残余グラフにおけるsourceから各頂点までの最短距離
  auto residue = edge_;           // 残余グラフ
  Flow result = 0;
  // 負辺を含むグラフに対応するため, ポテンシャルの初期値を計算
  for(const auto& E: edge_) for(const auto& e: E) if(0 < e.capacity) h[e.to] = std::min(h[e.to], h[e.from] + e.cost);
  while(0 < f) {
    // sourceからDijkstra
    using Node = std::tuple<Cost, Vertex>;
    std::vector<Cost> dist(size_, COST_INF);
    dist.at(source) = 0;
    std::vector<Edge*> prev(size_, nullptr);
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> q;
    q.emplace(0, source);
    while(!q.empty()) {
      Cost c; Vertex v;
      std::tie(c, v) = q.top();
      q.pop();
      if(dist.at(v) < c) continue;
      for(auto& e: residue.at(v)) {
        if(e.capacity <= 0) continue;
        if(dist.at(e.to) <= dist.at(e.from) + e.cost + h.at(e.from) - h.at(e.to)) continue;
        dist.at(e.to) = dist.at(e.from) + e.cost + h.at(e.from) - h.at(e.to);
        prev.at(e.to) = &e;
        q.emplace(dist.at(e.to), e.to);
      }
    }
    // sinkまでのパスが存在しなければ終了
    if(dist.at(sink) == COST_INF) return COST_INF;
    // 最短距離を更新
    for(int v = 0; v < size_; ++v) if(dist.at(v) != COST_INF) h.at(v) += dist.at(v);
    // 流せるだけ流す
    Flow add = f;
    for(Vertex v = sink; v != source; v = prev.at(v)->from) add = std::min(add, prev.at(v)->capacity);
    f -= add;
    result += add * h.at(sink);
    for(Vertex v = sink; v != source; v = prev.at(v)->from) {
      prev.at(v)->capacity -= add;
      residue.at(prev.at(v)->to).at(prev.at(v)->rev).capacity += add;
    }
  }
  return result;
}

#include<bits/stdc++.h>
using namespace std;
int main() {
//   // GRL_6_A
//   int V, E;
//   cin >> V >> E;
//   FlowNetwork G(V);
//   for(int i = 0; i < E; ++i) {
//     Vertex u, v; Flow c;
//     cin >> u >> v >> c;
//     G.insert(u, v, c);
//   }
//   cout << G.maximum_flow(0, V - 1) << endl;
  // GRL_6_B
  int V, E, F;
  cin >> V >> E >> F;
  FlowNetwork G(V);
  for(int i = 0; i < E; ++i) {
    Vertex u, v; Flow c; Cost d;
    cin >> u >> v >> c >> d;
    G.insert(u, v, c, d);
  }
  auto ans = G.minimum_cost_flow(0, V - 1, F);
  cout << (ans != COST_INF ? ans : -1) << endl;
}
