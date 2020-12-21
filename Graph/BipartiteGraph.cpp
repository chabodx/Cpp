// 単純有向2部グラフ

// 最大マッチング

#include<functional>
#include<vector>

class BipartiteGraph {
 public:
  BipartiteGraph(int X, int Y) : X(X), Y(Y), G(X + Y) {}
  void insert(int x, int y) {G[x].emplace_back(X + y);}
  int matching() const {
    // Ford-Fullkerson O(最大流量 * 辺数)
    std::vector<int> match_to(G.size(), -1);
    std::vector<bool> used(G.size(), false);
    std::function<bool(int)> dfs = [&](int from) {
      if(from == -1) return true;
      if(used[from]) return false;
      used[from] = true;
      for(const auto& to: G[from]) if(dfs(match_to[to])) {
        match_to[from] = to;
        match_to[to] = from;
        return true;
      }
      return false;
    };
    int count = 0;
    for(int i = 0; i < X; ++i) {
      used.assign(G.size(), false);
      if(dfs(i)) ++count;
    }
    return count;
  }
 private:
  int X, Y;
  std::vector<std::vector<int>> G;
};

////////////////////////////////////////////////////////////////////////////////

#include<iostream>
using namespace std;

int main() {
  BipartiteGraph g(3, 4);
  g.insert(0, 0);
  g.insert(0, 2);
  g.insert(0, 3);
  g.insert(1, 1);
  g.insert(2, 2);
  g.insert(2, 3);
  cout << g.matching() << endl;
}
