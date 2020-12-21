// 木

// 用語:
//   離心数: 最も遠い葉までの距離
//   半径:   離心数の最小値
//   直径:   離心数の最大値 = 最遠点対の距離
//   中心:   離心数が最小の頂点
//   重心:   削除したときにmin{各連結成分の頂点数}が最大となるような頂点

#include<algorithm>
#include<functional>
#include<vector>

using Size = int;
using Vertex = int;
using Weight = long long;
struct Edge {Vertex from, to; Weight weight;};
using Graph = std::vector<std::vector<Edge>>;

// 頂点オイラーツアー O(|V| + |E|)
//   部分頂点列 v, ..., v が v を根とする部分木に対応する
std::vector<Vertex> dfstree(const Graph& G, Vertex root = 0) {
  std::vector<Vertex> tour;
  std::function<void(Vertex, Vertex)> dfs = [&](Vertex cur, Vertex pre) {
    tour.emplace_back(cur);
    for(const auto& e: G[cur]) if(e.to != pre) dfs(e.to, e.from);
    tour.emplace_back(cur);
  };
  dfs(root, -1);
  return tour;
}

// 最遠点 O(|V| + |E|)
Vertex farthest(const Graph& G, Vertex v) {
  std::vector<Weight> d(G.size());
  std::function<Vertex(Vertex, Vertex)> dfs = [&](Vertex cur, Vertex pre) {
    auto res = cur;
    for(const auto& e: G[cur]) if(e.to != pre) {
      auto nex = dfs(e.to, e.from);
      auto w = e.weight + d[e.to];
      if(w <= d[cur]) continue;
      d[cur] = w;
      res = nex;
    }
    return res;
  };
  return dfs(v, -1);
}

// 最遠点対 O(|V| + |E|)
std::pair<Vertex, Vertex> farthest(const Graph& G) {
  auto v = farthest(G, 0);
  return std::make_pair(v, farthest(G, v));
}

// 高さ (最も遠い葉までの距離) O(|V| + |E|)
Weight height(const Graph& G, Vertex v) {
  std::vector<Weight> d(G.size());
  std::function<Weight(Vertex, Vertex)> dfs = [&](Vertex cur, Vertex pre) {
    for(const auto& e: G[cur]) if(e.to != pre) d[cur] = std::max(d[cur], e.weight + dfs(e.to, e.from));
    return d[cur];
  };
  return dfs(v, -1);
}

// 直径 O(|V| + |E|)
Weight diameter(const Graph& G) {
  return height(G, farthest(G, 0));
}

// 高さ (静的) O(|V| |E|)
std::vector<Weight> GRL_5_B(const Graph& G) {
  std::vector<std::vector<Weight>> memo;
  std::function<Weight(Vertex, Size)> dfs = [&](Vertex u, Size i) {
    if(~memo[u][i]) return memo[u][i];
    memo[u][i] = G[u][i].weight;
    auto v = G[u][i].to;
    for(std::size_t j = 0; j < G[v].size(); ++j) if(G[v][j].to != u) memo[u][i] = std::max(memo[u][i], G[u][i].weight + dfs(v, j));
    return memo[u][i];
  };
  for(auto i: G) memo.emplace_back(i.size(), -1);
  std::vector<Weight> res(G.size());
  for(std::size_t u = 0; u < G.size(); ++u) for(std::size_t i = 0; i < G[u].size(); ++i) res[u] = std::max(res[u], dfs(u, i));
  return res;
}

// 最大独立集合 O(|V| + |E|)
//   (一応) source と同じ連結成分に対して, 最大独立集合の頂点数を返す
Size maximum_independent_set(const Graph& G, Vertex source = 0) {
  std::vector<std::vector<Size>> dp(2, std::vector<Size>(G.size(), -1));
  std::function<Size(bool, Vertex, Vertex)> dfs = [&](bool opt, Vertex cur, Vertex pre) {
    if(~dp[opt][cur]) return dp[opt][cur];
    dp[opt][cur] = opt ? 1 : 0;
    for(const auto& e: G[cur]) if(e.to != pre) {
      if(opt) dp[opt][cur] += dfs(false, e.to, e.from);
      else    dp[opt][cur] += std::max(dfs(false, e.to, e.from), dfs(true, e.to, e.from));
    }
    return dp[opt][cur];
  };
  return std::max(dfs(false, source, -1), dfs(true, source, -1));
}

// 重心 O(|V| + |E|)
std::vector<Vertex> centroid(const Graph& G) {
  std::vector<Vertex> V;
  std::vector<Size> size(G.size(), 1);
  std::function<Size(Vertex, Vertex)> dfs = [&](Vertex cur, Vertex pre) {
    auto ok = true;
    for(auto e: G[cur]) if(e.to != pre) {
      size[cur] += dfs(e.to, e.from);
      if(G.size() / 2.0 < size[e.to]) ok = false;
    }
    if(G.size() / 2.0 < G.size() - size[cur]) ok = false;
    if(ok) V.emplace_back(cur);
    return size[cur];
  };
  dfs(0, 0);
  return V;
}

// 全方位木DP O(|V| + |E|) (ABC160F)
//   dp[v][i] := 頂点 v の i 番目の辺を削除したときの終点側部分木のスコア
void rerooting(const Graph& G, Vertex root = 0) {
  struct DP {
    int size = 0;
    const DP operator+(const DP& o) const {return {size + o.size};}
    const DP addRoot() const {return {size + 1};}
  };
  std::vector<std::vector<DP>> dp(G.size());
  std::function<DP(Vertex, Vertex)> dfs = [&](Vertex v, Vertex p) {
    const int N = G[v].size();
    dp[v].resize(N);
    DP res;
    for(auto i=0; i<N; ++i) if(G[v][i].to != p) res = res + (dp[v][i] = dfs(G[v][i].to, v));
    return res.addRoot();
  };
  std::function<void(Vertex, Vertex)> rev = [&](Vertex v, Vertex p) {
    const int N = G[v].size();
    std::vector<DP> sl(N+1), sr(N+1);
    for(auto i=0; i<N; ++i) sl[i+1] = sl[i] + dp[v][i];
    for(auto i=N; 0<i; --i) sr[i-1] = sr[i] + dp[v][i-1];
    for(auto i=0; i<N; ++i) {
      auto nex = G[v][i].to;
      if(nex == p) continue;
      for(std::size_t j=0; j<G[nex].size(); ++j) if(G[nex][j].to == v) dp[nex][j] = (sl[i] + sr[i+1]).addRoot();
      rev(nex, v);
    }
  };
  dfs(root, -1); rev(root, -1);
//   for(auto v=0; v<G.size(); ++v) cout << std::accumulate(std::begin(dp[v]), std::end(dp[v]), DP(), [](auto s, auto p) {return s + p.second;}).addRoot().val << std::endl;
}

// Centroid Decomposition (っぽいもの)
//   参考: https://blog.anudeep2011.com/heavy-light-decomposition/
//   構築: O(|V| + |E|)
//   LCA:  O(log |V|)
// dfs するときに in/out の順番を覚えておくと部分木クエリにも対応できる (https://codeforces.com/blog/entry/53170)
class CentroidDecomposition {
 public:
  CentroidDecomposition(const Graph& G, Vertex root) : head(G.size()), parent(G.size(), NIL), depth(G.size(), 0), subsize(G.size(), 1) {
    std::function<Size(Vertex)> dfs = [&](Vertex cur) {
      for(const auto& e: G[cur]) if(e.to != parent[cur]) {
        depth[e.to] = depth[cur] + 1;
        parent[e.to] = cur;
        subsize[cur] += dfs(e.to);
      }
      return subsize[cur];
    };
    std::function<void(Vertex)> build = [&](Vertex cur) {
      Vertex nex = NIL;
      Size size = 0;
      for(const auto& e: G[cur]) if(e.to != parent[cur]) if(chmax(size, subsize[e.to]) == subsize[e.to]) nex = e.to;
      for(const auto& e: G[cur]) if(e.to != parent[cur]) {
        if(e.to == nex) head[e.to] = head[cur];
        else            head[e.to] = e.to;
        build(e.to);
      }
    };
    dfs(root);
    build(head[root] = root);
  }
  Vertex lca(Vertex u, Vertex v) const {
    while(head[u] != head[v]) {
      if(depth[head[u]] > depth[head[v]]) std::swap(u, v);
      v = parent[head[v]];
    }
    return depth[u] < depth[v] ? u : v;
  }
 private:
  inline Size position(Vertex v) {return depth[v] - depth[head[v]];}
  template<typename T> inline T chmax(T& l, const T& r) {return l = std::max(l, r);};
  const Size NIL = -1;
  std::vector<Vertex> head, parent;
  std::vector<Size> depth, subsize;
};
/*
class CentroidPathDecomposition {
 public:
  CentroidPathDecomposition(const Graph& G, Vertex root) : chain(G.size()), position(G.size()), parent(G.size(), NIL), depth(G.size(), NIL), subsize(G.size()) {
    // construct a tree
    std::function<Size(Vertex, Size)> dfs = [&](Vertex cur, Size d) {
      depth[cur] = d;
      subsize[cur] = 1;
      for(const auto& e: G[cur]) if(depth[e.to] == NIL) {
        parent[e.to] = e.from;
        subsize[cur] += dfs(e.to, d+1);
      }
      return subsize[cur];
    };
    // decomposition
    std::function<Size(Vertex, Size)> CPD = [&](Vertex cur, Size id) {
      // add to a chain
      if(head.size() <= id) {
        head.emplace_back(cur);
        size.emplace_back(0);
      }
      chain[cur] = id;
      position[cur] = size[id];
      ++size[id];
      // find the special child
      auto max = 0, nex = 0;
      for(const auto& e: G[cur]) if(e.from == parent[e.to]) if(subsize[e.to] > max) {
        max = std::max(max, subsize[e.to]);
        nex = e.to;
      }
      // if cur is not a leaf
      if(max > 0) id = CPD(nex, id);
      // normal childs
      for(const auto& e: G[cur]) if(e.from == parent[e.to]) if(e.to != nex) id = CPD(e.to, id+1);
      return id;
    };
    // initialize
    dfs(root, 0);
    CPD(root, 0);
  }
  Vertex lca(Vertex u, Vertex v) const {
    while(head[chain[u]] != head[chain[v]]) {
      if(depth[head[chain[u]]] > depth[head[chain[v]]]) std::swap(u, v);
      v = parent[head[chain[v]]];
    }
    return depth[u] < depth[v] ? u : v;
  }
 private:
  const Size NIL = -1;
  std::vector<Vertex> head;
  std::vector<Size> chain, position, size;
  std::vector<Vertex> parent;
  std::vector<Size> depth, subsize;
};
*/

#include<bits/stdc++.h>
using namespace std;

int main() {
}
