#include <algorithm>
#include <climits>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

const long long int cInf = LLONG_MIN;
const long long int cMa = LLONG_MAX;

void ReadInput(int& n, std::vector<long long int>& a) {
  std::cin >> n;
  a.resize(n + 1);
  for (int i = 0; i < n; ++i) {
    std::cin >> a[i];
  }
  a[n] = cInf;
  std::reverse(a.begin(), a.end());
}

void InitializeDP(std::vector<std::pair<long long int, long long int>>& dp,
                  std::vector<std::vector<int>>& dp_rev,
                  const std::vector<long long int>& a) {
  dp[1] = {a[1], 1};
  dp_rev[1].push_back(1);
  dp[0] = {cInf, 0};
}

void ProcessDP(const std::vector<long long int>& a,
               std::vector<std::pair<long long int, long long int>>& dp,
               std::vector<std::vector<int>>& dp_rev, int n) {
  for (int i = 2; i <= n; ++i) {
    auto it = std::upper_bound(dp.begin(), dp.end(), std::make_pair(a[i], cMa));
    *it = std::make_pair(a[i], i);
    int d = std::distance(dp.begin(), it);
    dp_rev[d].push_back(i);
  }
}

int FindElems(const std::vector<std::pair<long long int, long long int>>& dp,
              int n) {
  int elems = 0;
  for (int i = 1; i <= n; ++i) {
    if (dp[i + 1].first == cMa) {
      elems = i;
      break;
    }
  }
  return elems;
}

void BuildRightOrder(
    const std::vector<std::pair<long long int, long long int>>& dp,
    const std::vector<std::vector<int>>& dp_rev, int elems, int n,
    std::vector<long long int>& right_order) {
  right_order.push_back(n - dp[elems].second + 1);
  int ind = dp[elems].second;
  for (int i = elems - 1; i >= 1; --i) {
    for (int j = static_cast<int>(dp_rev[i].size() - 1); j >= 0; --j) {
      if (dp_rev[i][j] < ind) {
        right_order.push_back(n - dp_rev[i][j] + 1);
        ind = dp_rev[i][j];
        break;
      }
    }
  }
  std::sort(right_order.begin(), right_order.end());
}

void PrintResult(int elems, const std::vector<long long int>& right_order) {
  std::cout << elems << '\n';
  for (int i = 0; i < static_cast<int>(right_order.size()); ++i) {
    std::cout << right_order[i] << ' ';
  }
}

int main() {
  int n;
  std::vector<long long int> a;
  ReadInput(n, a);

  std::vector<std::pair<long long int, long long int>> dp(n + 2, {cMa, cMa});
  std::vector<std::vector<int>> dp_rev(n + 1);
  InitializeDP(dp, dp_rev, a);

  ProcessDP(a, dp, dp_rev, n);

  int elems = FindElems(dp, n);

  std::vector<long long int> right_order;
  BuildRightOrder(dp, dp_rev, elems, n, right_order);

  PrintResult(elems, right_order);
}
