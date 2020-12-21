// 隣接リストで表現された無向グラフに対して
// 最大独立集合のサイズを求める

// 計算量: O(1.381^|V| * |V|)

// 解説: https://www.slideshare.net/wata_orz/ss-12131479
// 実装: https://qiita.com/drken/items/7f98315b56c95a6181a4

#include<vector>

using Weight = long long;
using Vertex = int;
struct Edge {Vertex from, to; Weight weight;};
using Graph = std::vector<std::vector<Edge>>;

int connected_case(const Graph&, std::vector<bool>& S);   // 頂点集合SによるGの誘導グラフに対して,
int general_case(const Graph&, const std::vector<bool>&); //       最大独立集合のサイズを出力する.

void dfs(std::vector<bool>& used, std::vector<bool>& next, const Graph& G, const std::vector<bool>& target, Vertex v) {
  used[v] = next[v] = true;
  for(const auto& e: G[v]) if(!used[e.to] && target[e.to]) dfs(used, next, G, target, e.to);
}

int connected_case(const Graph& G, std::vector<bool>& target) {
  int n = 0, mindeg = G.size(), maxdeg = 0, min = -1, max = -1;
  for(Vertex v = 0; v < G.size(); ++v) if(target[v]) {
    ++n;
    auto deg = 0;
    for(const auto& e: G[v]) if(target[e.to]) ++deg;
    if(maxdeg < deg) maxdeg = deg, max = v;
    if(deg < mindeg) mindeg = deg, min = v;
  }
  // The vertex-induced subgraph is an isolated vertex
  if(n == 1) return 1;
  // The vertex-induced subgraph is a path or a cycle
  if(maxdeg == 2) return mindeg==1 ? (n+1)/2 : n/2;
  // Remove a leaf
  if(mindeg == 1) {
    auto next = target;
    next[min] = false;
    for(const auto& e: G[min]) next[e.to] = false;
    return general_case(G, next) + 1;
  }
  // Remove a maxmum-degree vertex
  auto next = target;
  next[max] = false;
  auto res = general_case(G, next);
  for(const auto& e: G[max]) next[e.to] = false;
  return std::max(res, general_case(G, next) + 1);
}

int general_case(const Graph& G, const std::vector<bool>& target) {
  std::vector<bool> used(G.size());
  int res = 0;
  for(Vertex v = 0; v < G.size(); ++v) if(!used[v]) {
    std::vector<bool> next(G.size());
    dfs(used, next, G, target, v);
    res += connected_case(G, next);
  }
  return res;
}

int maximum_independent_set(const Graph& G) {
  return general_case(G, std::vector<bool>(G.size()));
}

#include<bits/stdc++.h>
using namespace std;

int main() {
}
