#include <algorithm>
#include <climits>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using std::cin;
using std::cout;
using std::string;
using std::vector;

int main() {
  int n;
  int m;
  cin >> n >> m;
  vector<int> w(m + 1, 0);
  vector<int> c(n + 1, 0);
  for (int i = 1; i <= n; i++) {
    cin >> w[i];
  }
  for (int i = 1; i <= n; i++) {
    cin >> c[i];
  }
  vector<vector<int>> dp(n + 1, vector<int>(m + 1, -1));
  vector<vector<int>> dp_last(n + 1, vector<int>(m + 1, -1));
  dp[0][0] = 0;
  for (int i = 1; i <= n; ++i) {
    for (int j = 0; j <= m; j++) {
      if (dp[i - 1][j] != -1) {
        dp[i][j] = dp[i - 1][j];
      }
      int t = j - w[i];
      if ((t >= 0) && (dp[i - 1][t] != -1)) {
        dp[i][j] = std::max(dp[i][j], dp[i - 1][t] + c[i]);
        dp_last[i][j] = 1;
      }
    }
  }
  int prev = 0;
  for (int i = 0; i <= m; i++) {
    if (dp[n][prev] < dp[n][i]) {
      prev = i;
    }
  }
  if (dp[n][prev] == 0) {
    return 0;
  }
  vector<int> elem;
  int k = n;
  while (k != 0) {
    if (dp_last[k][prev] == 1) {
      elem.push_back(k);
      prev -= w[k];
    }
    --k;
  }
  for (int i = static_cast<int>(elem.size()) - 1; i >= 0; i--) {
    cout << elem[i] << ' ';
  }
}
