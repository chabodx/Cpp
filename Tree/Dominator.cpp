// 支配節:
//   根付き木上の頂点 v に対して,
//   すべての root-v path に含まれるような頂点のうちで最も根から遠い頂点

// T. Lengauer and R. E. Tarjan, A Fast Algorithm for Finding Dominators in a Flowgraph (1979).
//   the simple version         O(m * log n)
//   the sophisticated version  O(m * a(m, n)), a: the functional inverse of Ackerman's function

#include<bits/stdc++.h>
using namespace std;

const int NIL = -1;

int root = 0;
vector<vector<int>> succ;
vector<vector<int>> pred;
vector<int> parent;
vector<int> semi;           // (i) 0 (ii) the number of w (iii) the number of sdom(w)
vector<int> vertex;
vector<vector<int>> bucket;
vector<int> dom;            // (i)   If sdom(w) = idom(w) then idom(w). Otherwise a vertex v whose number is smaller than w and idom(v) = idom(w)
                            // (ii)  idom(w)
vector<int> ancestor;
vector<int> label;
// For the sophisticated version.
vector<int> size;
vector<int> child;

// Number vertices in preorder.
int DFS(int v, int depth = 0) {
  semi[v] = depth;
  vertex[depth] = v;
  for(int w: succ[v]) {
    if(semi[w] == NIL) {
      parent[w] = v;
      depth = DFS(w, depth + 1);
    }
    pred[w].push_back(v);
  }
  return depth;
}

//*
// ** the sophisticated version **
// Path compression.
void COMPRESS(int v) {
  if(ancestor[v] != NIL) {
    COMPRESS(ancestor[v]);
    if(semi[label[ancestor[v]]] < semi[label[v]]) label[v] = label[ancestor[v]];
    if(ancestor[ancestor[v]] != NIL) ancestor[v] = ancestor[ancestor[v]];
  }
}
// If v is root of a tree in the forest, return v.
// Otherwise, let r be the root of the tree in the forest which contains v.
// Return any vertex u ≠ r of minimum semi(u) on the path r →. v.
int EVAL(int v) {
  if(ancestor[v] == NIL) return label[v];
  COMPRESS(v);
  return (semi[label[ancestor[v]]] >= semi[label[v]]) ? label[v] : label[ancestor[v]];
}
// Add edge(v, w) to the forest.
void LINK(int v, int w) {
  int s = w;
  while(child[s] != NIL && semi[label[w]] < semi[label[child[s]]]) {
    if(size[s] + size[child[child[s]]] >= 2 * (size[child[s]])) {
      ancestor[child[s]] = s;
      child[s] = child[child[s]];
    } else if(size[child[s]] == size[s]) {
      s = ancestor[s] = child[s];
    }
  }
  label[s] = label[w];
  size[v] = size[v] + size[w];
  if(size[v] < 2 * size[w]) swap(s, child[v]);
  while(s != NIL) {
    ancestor[s] = v;
    s = child[s];
  }
}
//*/

/*
// ** the simple version **
// Path compression.
void COMPRESS(int v) {
  if(ancestor[v] != NIL) {
    COMPRESS(ancestor[v]);
    if(semi[label[ancestor[v]]] < semi[label[v]]) label[v] = label[ancestor[v]];
    if(ancestor[ancestor[v]] != NIL) ancestor[v] = ancestor[ancestor[v]];
  }
}
// If v is root of a tree in the forest, return v.
// Otherwise, let r be the root of the tree in the forest which contains v.
// Return any vertex u ≠ r of minimum semi(u) on the path r →. v.
int EVAL(int v) {
  if(ancestor[v] == NIL) return label[v];
  COMPRESS(v);
  return label[v];
}
// Add edge(v, w) to the forest.
void LINK(int v, int w) {
  ancestor[w] = v;
  label[w] = w;
}
//*/

////////////////////////////////////////
int main() {
  // Input.
  int N, M;
  cin >> N >> M;
  succ.assign(N, vector<int>());
  for(int i = 0; i < M; ++i) {
    int s, t;
    cin >> s >> t;
    --s;
    --t;
    succ[s].push_back(t);
  }
  // Initialize.
  ancestor.assign(N, NIL);
  label.clear(); for(int i = 0; i < N; ++i) label.push_back(i);
  // Initialize for the sophisticated version.
  size.assign(N, 1);
  child.assign(N, NIL);
  // Step 1.
  pred.assign(N, vector<int>());
  semi.assign(N, NIL);
  vertex.assign(N, NIL);
  parent.assign(N, NIL);
  DFS(root);
  // Step 2 and 3 which uses LINK and EVAL.
  bucket.assign(N, vector<int>());
  dom.assign(N, NIL);
  for(int i = N - 1; 1 <= i; --i) {
    int w = vertex[i];
    // Step 2.
    for(int v: pred[w]) {
      int u = EVAL(v);
      if(semi[u] < semi[w]) semi[w] = semi[u];
    }
    bucket[vertex[semi[w]]].push_back(w);
    LINK(parent[w], w);
    // Step 3.
    while(!bucket[parent[w]].empty()) {
      int v = bucket[parent[w]].front();
      bucket[parent[w]].erase(begin(bucket[parent[w]]));
      int u = EVAL(v);
      dom[v] = (semi[u] < semi[v]) ? u : parent[w];
    }
  }
  // Step 4.
  for(int i = 1; i < N; ++i) {
    int w = vertex[i];
    if(dom[w] != vertex[semi[w]]) dom[w] = dom[dom[w]];
  }
  dom[root] = NIL;
  // Output.
  int Q;
  cin >> Q;
  for(int i = 0; i < Q; ++i) {
    int r;
    cin >> r;
    --r;
    cout << ((dom[r] == root) || (dom[r] == NIL) ? r + 1 : dom[r] + 1) << endl;
  }
}
