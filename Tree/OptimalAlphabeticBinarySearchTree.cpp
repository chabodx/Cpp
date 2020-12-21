// C - 最適二分探索木 (Optimal Alphabetic Binary Search Tree)

// n個の葉を持つ順序付き二分木を作る
// 入力として各葉の重みが与えられる
// 二分木のコストをsum_{i: 1 to n}(w[i] * depth[i])で定義する
// コストの最小値を求める

// n <= 1e5
// 1 <= w <= 1000

// 解法: 動的計画法 O(n^3)
//   dp[i][j] := sum(w[i..j]) * min(dp[i...k] + dp[k..j])
// 解法: Monge性の活用による動的計画法の高速化 O(n^2)
//   最適を出すkの値が単調: dp[i-1][j]のk <= dp[i][j]のk <= dp[i][j+1]のk
// 解法: Hu-Tuckerアルゴリズム + 併合可能順位キュー + 経路圧縮

#include<memory>
#include<utility>

class SkewHeap {
 public:
  using Key = long long;
  struct Node {
    Key key;
    std::unique_ptr<Node> lchild, rchild;
    Node(Key key) : key(key) {}
  };

  SkewHeap () {}

  std::unique_ptr<Node> make() const {return nullptr;}
  std::unique_ptr<Node> make(Key key) const {return std::move(std::make_unique<Node>(key));}

  std::unique_ptr<Node> meld(std::unique_ptr<Node>& lhs, std::unique_ptr<Node>& rhs) const {
    if(!lhs) return std::move(rhs);
    if(!rhs) return std::move(lhs);
    if(lhs->key > rhs->key) std::swap(lhs, rhs);
    lhs->rchild = meld(lhs->rchild, rhs);
    std::swap(lhs->lchild, lhs->rchild);
    return std::move(lhs);
  }

  void push(std::unique_ptr<Node>& root, Key key) const {
    auto n = std::make_unique<Node>(key);
    root = meld(root, n);
  }

  Key top(std::unique_ptr<Node>& root) const {return root->key;}

  Key pop(std::unique_ptr<Node>& root) const {
    auto key = root->key;
    root = meld(root->lchild, root->rchild);
    return key;
  }
};

// OABST
#include<functional>
#include<queue>
#include<vector>

using Heap = SkewHeap;
using Pointer = std::unique_ptr<Heap::Node>;
using Weight = long long;
using Size = int;

Weight HuTucker(std::vector<Weight> leaf) {
  const Size N = leaf.size();
  const Heap heap;

  Weight result = 0;

  std::vector<Pointer> hpq;
  std::vector<Size> lmemo(N), rmemo(N);
  for(Size i = 0; i < N; ++i) {
    hpq.emplace_back(heap.make(leaf[i]));
    lmemo[i] = i - 1;
    rmemo[i] = i + 1;
  }
  std::function<Size(Size)> left = [&](Size i) {
    auto j = lmemo[i];
    if(j < 0)  return j;
    if(hpq[j]) return j;
    return lmemo[i] = left(j);
  };
  std::function<Size(Size)> right = [&](Size i) {
    auto j = rmemo[i];
    if(N <= j) return j;
    if(hpq[j]) return j;
    return rmemo[i] = right(j);
  };

  using Tuple = std::tuple<Weight, Size, Size>;
  std::priority_queue<Tuple, std::vector<Tuple>, std::greater<Tuple>> mpq;
  for(Size i = 0; i < N - 1; ++i) mpq.emplace(heap.top(hpq[i]) + heap.top(hpq[right(i)]), i, right(i));

  while(!mpq.empty()) {
    Weight cost; Size i, j;
    std::tie(cost, i, j) = mpq.top(); mpq.pop();

    if(!hpq[i]) continue;
    auto x = heap.pop(hpq[i]);
    if(!hpq[j]) {heap.push(hpq[i], x); continue;}
    auto y = heap.pop(hpq[j]);
    if(cost != x + y) {heap.push(hpq[i], x); heap.push(hpq[j], y); continue;}
    heap.push(hpq[i], cost);
    result += cost;

    for(auto* k: {&i, &j}) {
      if(right(*k) < N && leaf[right(*k)] != heap.top(hpq[right(*k)])) hpq[*k] = heap.meld(hpq[*k], hpq[right(*k)]);
      if(0 <= left(*k) && leaf[left(*k)] != heap.top(hpq[left(*k)])) {
        auto nex = left(*k);
        if(nex != *k) hpq[nex] = heap.meld(hpq[nex], hpq[*k]);
        *k = nex;
      }
    }
    if(i != j) hpq[i] = heap.meld(hpq[i], hpq[j]);

    x = heap.pop(hpq[i]);
    if(0 <= left(i)) mpq.emplace(heap.top(hpq[left(i)]) + x, left(i), i);
    if(right(i) < N) mpq.emplace(x + heap.top(hpq[right(i)]), i, right(i));
    if(hpq[i])       mpq.emplace(x + heap.top(hpq[i]), i, i);
    if(0 <= left(i) && right(i) < N) mpq.emplace(heap.top(hpq[left(i)]) + heap.top(hpq[right(i)]), left(i), right(i));
    heap.push(hpq[i], x);
  }

  return result;
}

#include<bits/stdc++.h>
using namespace std;

int main() {
  int n;
  cin >> n;
  vector<Weight> w(n);
  for(auto& i: w) cin >> i;
  cout << HuTucker(w) << endl;
}
