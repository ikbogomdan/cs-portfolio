#include <algorithm>
#include <array>
#include <climits>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using std::array;
using std::cin;
using std::cout;
using std::string;
using std::vector;
const long long int cMod = 1e9 + 9;
const array<long long int, 4> cE = {1, 0, 0, 1};
array<long long int, 4> m_ret;
array<long long int, 4> res;

array<long long int, 4> operator*(const array<long long int, 4>& lmat,
                                  const array<long long int, 4>& rmat) {
  res[0] =
      ((lmat[0] * rmat[0] % cMod + lmat[1] * rmat[2] % cMod) % cMod + cMod) %
      cMod;
  res[1] =
      ((lmat[0] * rmat[1] % cMod + lmat[1] * rmat[3] % cMod) % cMod + cMod) %
      cMod;
  res[2] =
      ((lmat[2] * rmat[0] % cMod + lmat[3] * rmat[2] % cMod) % cMod + cMod) %
      cMod;
  res[3] =
      ((lmat[2] * rmat[1] % cMod + lmat[3] * rmat[3] % cMod) % cMod + cMod) %
      cMod;
  return res;
}
array<long long int, 4> BinFib(array<long long int, 4> mt, long long int deg) {
  if (deg == 0) {
    return cE;
  }
  if (deg % 2 == 1) {
    return (BinFib(mt, deg - 1) * mt);
  }
  m_ret = BinFib(mt, deg / 2);
  return (m_ret * m_ret);
}
array<long long int, 4> Fibcount(long long int deg) {
  array<long long int, 4> fib = {1, 1, 1, 0};
  return BinFib(fib, deg);
}
int main() {
  long long int n;
  long long int q;
  cin >> n >> q;
  vector<std::pair<long long int, long long int>> s(n);
  vector<long long int> k(n);
  for (int i = 0; i < n; ++i) {
    cin >> s[i].first >> s[i].second >> k[i];
    k[i] = k[i] % cMod;
  }
  vector<std::pair<long long int, long long int>> c(q);
  for (int i = 0; i < q; ++i) {
    cin >> c[i].first;
    c[i].second = i;
  }
  vector<long long int> c_ans(q, 0);
  std::sort(c.begin(), c.end());
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < q; ++j) {
      if (c[j].first + s[i].first > s[i].second) {
        break;
      }
      if (c[j].first + s[i].first == s[i].second) {
        c_ans[c[j].second] = ((c_ans[c[j].second] + k[i]) % cMod + cMod) % cMod;
        continue;
      }
      long long int sum = 0;
      long long int deg = llabs(s[i].first - (s[i].second - c[j].first));
      array<long long int, 4> fib = Fibcount(deg);
      sum = fib[0] % cMod;
      c_ans[c[j].second] =
          ((c_ans[c[j].second] + (k[i] * sum) % cMod) % cMod + cMod) % cMod;
    }
  }
  for (int i = 0; i < q; ++i) {
    cout << c_ans[i] << '\n';
  }
}
