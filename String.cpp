// 文字列

// iomanip
//   小数点以下8桁: cout << setprecision(8) << fixed << real_number << endl;
//   8桁0埋め:      cout << setw(8) << setfill('0') << real_number << endl;

#include<algorithm>
#include<string>
#include<vector>
#include<queue>
#include<memory>
#include<unordered_map>

// 最長回文: R[i] := S[i]を中心とする最長回文の長さ
// Manacher: O(|S|)
//   アルゴリズム概要: http://snuke.hatenablog.com/entry/2014/12/02/235837
//     iを中心とする回文の長さjを求める
//     S[i-j..i]に回文があれば, S[i..i+j]の対称位置にも同長回文が存在する
//   最長の奇数長回文の長さを求められる
//   a$b$a$a$bなど, 予め各文字間にダミー文字を追加すると偶数長回文にも対応可能
std::vector<int> Manacher(const std::string& S) {
  std::vector<int> R(S.size());
  for(auto i = 0, j = 0, k = 0; i < std::ssize(S); i += k, j -= k) {
    while(i - j >= 0 && i + j < std::ssize(S) && S[i - j] == S[i + j]) ++j;
    for(R[i] = j, k = 1; i - k >= 0 && i + k < std::ssize(S) && k + R[i - k] < j; ++k) R[i + k] = R[i - k];
  }
  return R;
}
// R[i] := S[i]を中心とする最長奇数長回文の長さ*2 (i: 偶数)
//         S[i]を中心とする最長偶数長回文の長さ*2 (i: 奇数)
std::vector<int> palindrome(std::string S, const std::string separator = "$") {
  for(auto i = 1; i < std::ssize(S); i += 2) S.insert(i, separator);
  return Manacher(S);
}

// 最長共通接頭辞: Z[i] := S[i]から始まる部分文字列のうち, Sのprefixになっているものの最長の長さ
// Z Algorithm: O(|S|)
//   アルゴリズム概要:                      http://codeforces.com/blog/entry/3107
//   図の緑がS[L, R], 緑の先頭からの幅がk:  http://snuke.hatenablog.com/entry/2014/12/03/214243
// 例: 文字列Sから文字列Tを探す
//       "T S"に対してZ Algorithmを適用
//       Z[i] >= |T|であるようなi (> |T|)を探す
std::vector<int> z_algorithm(const std::string& S) {
  std::vector<int> Z(S.size(), S.size());
  for(auto i = 1, L = 0, R = 0; i < std::ssize(S); ++i) {
    if(i > R) {
      L = R = i;
      while(R < std::ssize(S) && S[R - L] == S[R]) ++R;
      Z[i] = R-- - L;
    } else {
      if(Z[i - L] < R - i + 1) Z[i] = Z[i - L];
      else {
        L = i;
        while(R < std::ssize(S) && S[R - L] == S[R]) ++R;
        Z[i] = R-- - L;
      }
    }
  }
  return Z;
}

// 編集距離: 追加/編集/置換
int LevenshteinDistance(const std::string& s, const std::string& t) {
  auto n = std::ssize(s);
  auto m = std::ssize(t);
  if(!n || !m) return n | m;
  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1));
  for(auto i = 1; i <= n; ++i) dp[i][0] = i;
  for(auto j = 1; j <= m; ++j) dp[0][j] = j;
  for(auto i = 1; i <= n; ++i) for(auto j = 1; j <= m; ++j) {
    if(s[i-1] == t[j-1]) dp[i][j] = dp[i - 1][j - 1];
    else                 dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + 1});
  }
  return dp[n][m];
}

// ローリングハッシュ
using ULL = unsigned long long;
const ULL B = 100000007;
ULL power(int n) {
  static std::vector<ULL> memo = {1};
  while(std::ssize(memo) <= n) memo.emplace_back(B * memo.back());
  return memo[n];
}
class RollingHash {
 public:
  RollingHash(const std::string& s) : s_(s) {
    h_ = {0};
    for(auto c: s) h_.emplace_back(B * h_.back() + c);
  }
  char& operator[](int i) {return s_[i];}
  int size() const {return s_.size();}
  ULL hash() const {return h_.back();}
  ULL hash(int i) const {return h_.back() - h_[i] * power(s_.size() - i);}
  ULL hash(int l, int r) const {return h_[r + 1] - h_[l] * power(r - l + 1);}
 private:
  std::string s_;
  std::vector<ULL> h_;
};

// パターンマッチ: Sに含まれるPの位置(最後に一致した文字のインデックス)を列挙
// Knuth-Morris-Prat: O(|S| + |P|)
std::vector<int> KMP(const std::string& s, const std::string& p) {
  std::vector<int> fail(p.size() + 1);
  auto j = fail[0] = -1;
  for(auto i = 1; i <= std::ssize(p); ++i) {
    while(0<=j && p[j]!=p[i-1]) j = fail[j];
    fail[i] = ++j;
  }
  std::vector<int> index;
  for(auto i = 0, k = 0; i < std::ssize(s); ++i) {
    while(0<=k && p[k]!=s[i]) k = fail[k];
    if(std::ssize(p) <= ++k) {
      index.emplace_back(i);
      k = fail[k];
    }
  }
  return index;
}

// 複数パターン検索
// Aho-Corasick: O(|S| + Σ|Pi|)
//   アルゴリズム概要: http://algoogle.hadrori.jp/algorithm/aho-corasick.html
//   マッチした個数を返すようにしているけど, マッチ箇所やパターンの種類を返したほうが良いかもしれない
template<typename T> class AhoCorasick {
 public:
  AhoCorasick(const std::vector<T>& p) : root_(std::make_shared<Trie>()) {
    // Build the Trie.
    for(const auto& i: p) {
      auto node = root_;
      for(auto c: i) {
        if(!node->child[c]) node->child[c] = std::make_shared<Trie>();
        node->child[c]->depth = node->depth + 1;
        node = node->child[c];
        if(node->depth == std::ssize(i)) ++node->match;  // パターンの種類を記憶するなら node->match.emplace(std::hash(i));
        // node->string=i.substr(0,node->depth);
      }
    }
    // Set fails.
    std::queue<std::shared_ptr<Trie>> q;
    for(auto i: root_->child) {
      i.second->fail = root_;
      q.emplace(i.second);
    }
    while(!q.empty()) {
      auto node = q.front();
      q.pop();
      for(const auto& i: node->child) {
        auto c = i.first;
        auto n = i.second;
        q.emplace(n);
        n->fail = root_;
        auto f = node->fail;
        while(f!=root_ && !f->child[c]) f = f->fail;
        if(f->child[c]) n->fail = f->child[c];
        n->match += n->fail->match;  // パターンの種類を記憶するなら n->match = union(n->match, n->fail->match);
        // std::cout<<"  node: "<<node->string<<"  child: "<<n->string<<" fail: "<<n->fail->string<<"  match: "<<n->match<<std::endl;
      }
    }
  }
  int match(const T& s) {
    int res = 0;
    auto node = root_;
    for(auto c: s) {
      while(node!=root_ && !node->child[c]) node = node->fail;
      if(node->child[c]) node = node->child[c];
      res += node->match;
    }
    return res;
  }
 private:
  struct Trie {
    std::unordered_map<char, std::shared_ptr<Trie>> child;
    std::shared_ptr<Trie> fail;
    int depth = 0;
    int match = 0;
    // std::string string;
  };
  std::shared_ptr<Trie> root_;
};

#include<bits/stdc++.h>
using namespace std;
int main() {
  for(auto i: Manacher("banana")) cout << " " << i;
  cout << endl;
  for(auto i: z_algorithm("abracadabra")) cout << " " << i;
  cout << endl;
  string S("bacab");
  vector<string> pattern{"a", "ab", "aca", "ba", "c", "cab"};
  cout << AhoCorasick(pattern).match(S) << endl;
}
