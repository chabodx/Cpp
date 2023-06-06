// 統計学

// 標準ライブラリの乱数
#include<random>
// 特徴: 非決定的, 決定的な乱数より100倍くらい遅い
unsigned int rnd32() {
  static std::random_device rnd;
  return rnd();
}
// メルセンヌ・ツイスタ
//   特徴: 決定的
//   周期: 2^19937 - 1
unsigned long mt32() {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  return mt();
}
// メルセンヌ・ツイスタ
//   特徴: 決定的
//   周期: 2^19937 - 1
unsigned long mt64() {
  static std::random_device rd;
  static std::mt19937_64 mt(rd());
  return mt();
}

// xorshift
//   周期: 2^128 - 1
unsigned long xor128() {
  static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
  auto t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

// Box-Muller法: 2つの一様分布から正規分布に変換
//   開区間 (0, 1) 上の一様分布 U と, U に従う独立な確率変数 U1, U2 に対して, X1 と X2 は正規分布に従う
//     X1 = sqrt(-2 * log(U1)) * cos(2 * Pi * U2)
//     X2 = sqrt(-2 * log(U1)) * sin(2 * Pi * U2)
#include<cmath>
double BoxMuller() {
  static std::random_device rd;
  static std::mt19937 U1(rd()), U2(rd());
  static const double MAX = rd.max();
  static const double PI = std::acos(-1);
  double x = 0;
  double y = U2() / MAX;
  while(x == 0 || x == 1) x = U1() / MAX;
  return std::sqrt(-2 * std::log(x)) * std::cos(2 * PI * y);
//   return std::sqrt(-2 * std::log(x)) * std::sin(2 * PI * y);
}

#include<bits/stdc++.h>
using namespace std;
int main() {
  for(auto i=0; i<9; ++i) cout << BoxMuller() << endl;
}
