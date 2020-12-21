// Topological Sort

// 多重辺と自己ループを持たない DAG の頂点をトポロジカルソート

// AOJ: GRL_4_B: Topological Sort

#include<bits/stdc++.h>
using namespace std;

using Graph = vector<set<int>>;

void topological_sort(const Graph& reverse, int u) {
  static set<int> memo;
  if(memo.count(u)) return;
  memo.insert(u);
  for(int v: reverse[u]) topological_sort(reverse, v);
  cout << u << endl;
}

int main() {
  int V, E;
  cin >> V >> E;
  Graph reverse(V);
  for(int i = 0; i < E; ++i) {
    int u, v;
    cin >> u >> v;
    reverse[v].insert(u);
  }
  for(int i = 0; i < V; ++i) topological_sort(reverse, i);
}
