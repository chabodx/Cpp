// 隣接リストで表現された有向グラフ

// Weight を double にすると, 桁数が多いとき 1e9 のように出力されることに注意

// 用語-DAG:
//   反鎖: 全ての要素間について順序が成り立たない部分集合

#include<algorithm>
#include<functional>
#include<memory>
#include<numeric>
#include<queue>
#include<stack>
#include<vector>

using Weight = long long;
using Vertex = int;
struct Edge {Vertex from, to; Weight weight;};
using Graph = std::vector<std::vector<Edge>>;

// 比較演算子
// bool operator==(const Edge& lhs, const Edge& rhs) {return lhs.from==rhs.from && lhs.to==rhs.to && lhs.weight==rhs.weight;}

// 隣接行列 O(|V|^2 + |E|)
std::vector<std::vector<Vertex>> adjacent_matrix(const Graph& G) {
  std::vector<std::vector<Vertex>> A(G.size(), std::vector<Vertex>(G.size(), 0));
  for(const auto& list: G) for(const auto& e: list) ++A[e.from][e.to];
  return A;
}

// 逆辺 O(1)
Edge reverse(const Edge& e) {
  return {e.to, e.from, e.weight};
}

// 逆辺からなるグラフ O(|V| + |E|)
Graph reverse(const Graph& G) {
  Graph H(G.size());
  for(auto& list: G) for(auto& e: list) H[e.to].push_back({e.to, e.from, e.weight});
  return H;
}

// 補グラフ O(|V|^2 + |E|)
Graph complement(const Graph& G) {
  std::vector<std::vector<bool>> A(G.size(), std::vector<bool>(G.size()));
  for(auto& list: G) for(auto& e: list) A[e.from][e.to] = true;
  Graph H(G.size());
  for(Vertex u = 0; u < Vertex(G.size()); ++u) for(Vertex v = 0; v < Vertex(G.size()); ++v) if(u != v) if(!A[u][v]) H[u].push_back({u, v});
  return H;
}

// 帰りがけ順 O(|V| + |E|)
std::vector<Vertex> postorder(const Graph& G) {
  std::vector<bool> used(G.size());
  std::vector<Vertex> order;
  std::function<void(Vertex)> dfs = [&](Vertex v) {
    used[v] = true;
    for(const auto& e: G[v]) if(!used[e.to]) dfs(e.to);
    order.emplace_back(v);
  };
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(!used[v]) dfs(v);
  return order;
}

// トポロジカルソート O(|V| + |E|)
//   逆辺にしてから帰りがけ順を求める
//   ※逆辺にせずに帰りがけ順を求めて, その結果を reverse しても良い O(|V| log |V| + |E|)
std::vector<Vertex> topological_order(const Graph& G) {
  return postorder(reverse(G));
}

// トポロジカルソート O(|V| + |E|)
//   サイクルが存在する場合は空 vector を返す
//   queue の代わりに priority_queue を使うと辞書順最小などに対応できる
std::vector<Vertex> Kahn(const Graph& G) {
  std::vector<Vertex> order;
  std::vector<int> indeg(G.size());
  for(const auto& E: G) for(const auto& e: E) ++indeg[e.to];
  std::queue<Vertex> q;
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(indeg[v] == 0) q.emplace(v);
  while(!q.empty()) {
    for(const auto& e: G[q.front()]) if(--indeg[e.to] == 0) q.emplace(e.to);
    order.emplace_back(q.front());
    q.pop();
  }
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(indeg[v] != 0) return {};
  return order;
}

// 連結成分ごとに分解 O(|V|)
// ※無向グラフ限定
std::vector<std::vector<Vertex>> connected_component(const Graph& G) {
  std::vector<std::vector<Vertex>> cc;
  std::vector<bool> used(G.size());
  std::function<void(int)> dfs = [&](int v) {
    cc.back().push_back(v);
    used[v] = true;
    for(const auto& e: G[v]) if(!used[e.to]) dfs(e.to);
  };
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(!used[v]) {
    cc.push_back(std::vector<Vertex>());
    dfs(v);
  }
  return cc;
}

// すべての連結成分に対して関節点と橋を列挙 O(|V| + |E|)
// ※無向グラフ限定
// ※articulation には同じ頂点が複数個入る可能性があり, その個数は「分割後の成分数 - 1」個
void Tarjan(const Graph& G, std::vector<Vertex>& articulation, std::vector<Edge>& bridge) {
  std::vector<int> num(G.size()), low(G.size());
  int timer = 0;
  std::function<void(Vertex,Vertex)> dfs = [&](Vertex cur, Vertex pre) {
    num[cur] = low[cur] = ++timer;
    for(const auto& e: G[cur]) if(e.to != pre) {
      if(num[e.to]) {
        low[cur] = std::min(low[cur], num[e.to]);
      } else {
        dfs(e.to, cur);
        low[cur] = std::min(low[cur], low[e.to]);
        if(cur == pre) if(num[cur]+1 < num[e.to]) articulation.push_back(cur);
        if(cur != pre) if(num[cur] <= low[e.to])  articulation.push_back(cur);
        if(num[cur] < low[e.to])                  bridge.push_back(e);
      }
    }
  };
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(!num[v]) dfs(v, v);
}

// 二重辺連結成分分解 O(|V| + |E|)
// ※無向グラフ限定
// ※戻り値は選択
auto bridge(const Graph& G) {
  std::stack<Vertex> roots, S;
  std::vector<bool> inS(G.size());
  std::vector<int> num(G.size());
  std::vector<Edge> bridge;
  std::vector<std::vector<Vertex>> bcc;
  int timer = 0;
  // Tarjan
  std::function<void(const Edge&)> dfs = [&](const Edge& cur) {
    num[cur.to] = ++timer;
    S.push(cur.to); inS[cur.to] = true;
    roots.push(cur.to);
    for(const auto& nex: G[cur.to]) {
      if(!num[nex.to]) dfs(nex);
      else if(nex.to != cur.from && inS[nex.to]) while(num[roots.top()] > num[nex.to]) roots.pop();
    }
    if(roots.top() == cur.to) {
      bridge.push_back(cur);
      bcc.push_back(std::vector<Vertex>());
      while(true) {
        auto v = S.top(); S.pop(); inS[v] = false;
        bcc.back().push_back(v);
        if(v == cur.to) break;
      }
      roots.pop();
    }
  };
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(!num[v]) {
    dfs({v, v});
    bridge.pop_back();
  }
//   return bridge;
  return bcc;
}

// 強連結成分分解 O(|V| + |E|)
std::vector<std::vector<Vertex>> strongly_connected_component(const Graph& G) {
  std::vector<std::vector<Vertex>> scc;
  std::stack<Vertex> S;
  std::vector<bool> inS(G.size());
  std::vector<int> num(G.size()), low(G.size());
  int timer = 0;
  // Tarjan
  std::function<int(Vertex v)> dfs = [&](Vertex v) {
    num[v] = low[v] = ++timer;
    S.push(v); inS[v] = true;
    for(const auto& e: G[v]) {
      if(!num[e.to])     low[v] = std::min(low[v], dfs(e.to));
      else if(inS[e.to]) low[v] = std::min(low[v], num[e.to]);
    }
    if(num[v] == low[v]) {
      scc.push_back(std::vector<Vertex>());
      while(true) {
        auto w = S.top(); S.pop(); inS[w] = false;
        scc.back().push_back(w);
        if(v == w) break;
      }
    }
    return low[v];
  };
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(!num[v]) dfs(v);
  return scc;
}

// 単一始点最短経路 O(|V| |E|)
// ※重みを -1 倍したグラフに適用すると元のグラフの最長経路を得られる
// ※d[t] == -INF の場合は s から t への経路上に負閉路が存在する (ABC137E)
std::vector<Weight> BellmanFord(const Graph& G, Vertex s, Weight INF = 1e18) {
  std::vector<Weight> d(G.size(), INF);
  d[s] = 0;
  for(Vertex k = 0; k < 2*Vertex(G.size()); ++k) for(const auto& E: G) for(const auto& e: E) if(d[e.from] != INF) {
    if(d[e.to] > d[e.from] + e.weight) {
      d[e.to] = d[e.from] + e.weight;
      if(Vertex(G.size())-1 <= k) d[e.to] = -INF;
    }
  }
  return d;
}

// 単一始点最短経路 O(|E| + |V| log |V|)
// ※負辺を含むグラフには使えない
std::vector<Weight> Dijkstra(const Graph& G, Vertex s) {
  std::vector<Weight> d(G.size(), -1);
  std::priority_queue<Edge, std::vector<Edge>, std::function<bool(const Edge&, const Edge&)>> q([](const auto& lhs, const auto& rhs) {return lhs.weight > rhs.weight;});
  q.push({s, s, 0});
  while(!q.empty()) {
    auto cur = q.top();
    q.pop();
    if(d[cur.to] != -1) continue;
    d[cur.to] = cur.weight;
    for(const auto& nex: G[cur.to]) if(d[nex.to] == -1) q.push({cur.to, nex.to, cur.weight + nex.weight});
  }
  return d;
}

// 単一始点最短経路に含まれ得る辺の列挙 O(|E| + |V| log |V|)
// ※負辺を含むグラフには使えない
Graph shortest_DAG(const Graph& G, Vertex s) {
  Graph H(G.size());
  auto d = Dijkstra(G, s);
  for(const auto& E: G) for(const auto& e: E) if(d[e.from] + e.weight == d[e.to]) H[e.from].emplace_back(e);
  return H;
}

// 単一始点最短経路の個数 O(|E| + |V| log |V|)
// ※負辺を含むグラフには使えない
std::vector<int> shortest_path_number(const Graph& G, Vertex s) {
  std::vector<int> dp(G.size());
  dp[s] = 1;
  auto d = Dijkstra(G, s);
  for(const auto& E: G) for(const auto& e: E) if(d[e.from] + e.weight == d[e.to]) dp[e.to] += dp[e.from];
  return dp;
}

// 二頂点間最短経路 O(|E| + |V|^3)
// ※INF + INF がオバーフローを起こさないよう注意
std::vector<std::vector<Weight>> WarshallFloyd(const Graph& G, Weight INF = 1e9) {
  std::vector<std::vector<Weight>> d(G.size(), std::vector<Weight>(G.size(), INF));
  for(Vertex i = 0; i < Vertex(G.size()); ++i) d[i][i] = 0;
  for(const auto& list: G) for(const auto& e: list) d[e.from][e.to] = std::min(d[e.from][e.to], e.weight);
  for(Vertex i = 0; i < Vertex(G.size()); ++i) for(Vertex j = 0; j < Vertex(G.size()); ++j) for(Vertex k = 0; k < Vertex(G.size()); ++k) d[i][j] = std::min(d[i][j], d[i][k] + d[k][j]);
  return d;
}

// すべての u-v パスに対して u の直後に訪れる頂点の最小値/最大値 O(|V| log |V| + |V| |E|)
// ※有向グラフの迂回路判定に利用 (ARC092F)
std::vector<std::vector<std::vector<Vertex>>> follower(Graph G) {
  for(auto& E: G) sort(begin(E), end(E), [](const auto& l, const auto& r) {return l.to < r.to;});
  std::vector<std::vector<std::vector<Vertex>>> ok(2, std::vector<std::vector<Vertex>>(G.size(), std::vector<Vertex>(G.size(), -1)));
  std::function<void(int, Vertex, Vertex)> dfs = [&](auto b, auto r, auto v) {
    for(const auto& e: G[v]) if(ok[b][r][e.to] == -1) {
      ok[b][r][e.to] = ok[b][r][e.from];
      dfs(b, r, e.to);
    }
  };
  for(Vertex v = 0; v < Vertex(G.size()); ++v) for(auto b: {0, 1}) ok[b][v][v] = v;
  for(Vertex r = 0; r < Vertex(G.size()); ++r) for(auto i = 0; i < Vertex(G[r].size()); ++i) for(auto b: {0, 1}) {
    auto x = (b==0) ? i : G[r].size()-1-i;
    auto v = G[r][x].to;
    if(ok[b][r][v] == -1) {
      ok[b][r][v] = x;
      dfs(b, r, v);
    }
  }
  return ok;
}

// 頂点の C-彩色可能性 O(|V| + |E|)
using Color = int;
bool color(const Graph& G, Color C) {
  std::vector<int> color(G.size(), -1);
  std::function<bool(Vertex, Color)> dfs = [&](auto v, auto c) {
    color[v] = c;
    for(const auto& e: G[v]) {
      if(color[e.to] == color[e.from]) return false;
      if(color[e.to] != -1) continue;
      if(!dfs(e.to, (c + 1) % C)) return false;
    }
    return true;
  };
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(color[v] == -1) if(!dfs(v, 0)) return false;
  return true;
}

// サイクル検出 (ABC142F) O(|V| + |E|)
std::vector<Vertex> cycle(const Graph& G) {
  std::vector<Vertex> walk, cycle;
  std::vector<bool> ng(G.size()), used(G.size());
  std::function<bool(Vertex)> dfs = [&](auto v) {
    if(ng[v]) return false;
    if(used[v]) {walk.emplace_back(v); return true;}
    used[v] = true;
    for(const auto& e: G[v]) {
      if(dfs(e.to)) {walk.emplace_back(v); return true;}
      else          {ng[e.to] = true;}
    }
    ng[v] = true;
    used[v] = false;
    return false;
  };
  for(Vertex v = 0; v < Vertex(G.size()); ++v) if(!used[v]) if(dfs(v)) {
    for(auto v: walk) if(cycle.empty() || v != walk[0]) cycle.emplace_back(v);
    std::reverse(std::begin(cycle), std::end(cycle));
    break;
  }
  return cycle;
}

#include<bits/stdc++.h>
using namespace std;

int main() {
//   int V, E;
//   cin >> V >> E;
//   Graph G(V);
//   for(int i = 0; i < E; ++i) {
//     int s, t;
//     cin >> s >> t;
//     G[s].push_back({s, t, 0});
//     G[t].push_back({t, s, 0});
//   }
  // GRL_3_A
  // GRL_3_B
//   vector<int> cut_vertex;
//   vector<Edge> bridge;
//   for(auto S: connected_component(G)) lowlink(G, S.front(), S.front(), cut_vertex, bridge);
//   // Articulation vertex
//   set<int> S(begin(cut_vertex), end(cut_vertex));
//   for(auto v: S) cout << v << endl;
//   // Bridge
//   for(auto& e: bridge) if(e.from > e.to) swap(e.from, e.to);
//   set<Edge, function<bool(Edge&,Edge&)>> S(begin(bridge), end(bridge), [](Edge& lhs, Edge& rhs){
//     if(lhs.from != rhs.from) return lhs.from < rhs.from;
//     if(lhs.to != rhs.to) return lhs.to < rhs.to;
//     return lhs.weight < rhs.weight;
//   });
//   for(auto e: S) cout << e.from << " " << e.to << endl;
  // GRL_3_C
//   auto scc = strongly_connected_component(G);
//   vector<int> group(G.size());
//   for(int i = 0; i < scc.size(); ++i) for(auto v: scc[i]) group[v] = i;
//   int Q;
//   cin >> Q;
//   for(int i = 0; i < Q; ++i) {
//     int u, v;
//     cin >> u >> v;
//     cout << (group[u] == group[v] ? 1 : 0) << endl;
//   }
}
