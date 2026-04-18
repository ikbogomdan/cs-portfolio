#include <algorithm>
#include <climits>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::string;
using std::vector;

int main() {
  int n;
  int m;
  int g;
  cin >> n >> g >> m;
  vector<pair<pair<int, int>, int>> v(n, {{0, 0}, 0});  // f, w, c

  for (int i = 0; i < n; i++) {
    int w1;
    int c1;
    int f1;
    cin >> w1 >> c1 >> f1;
    v[i] = {{f1, w1}, c1};
  }
  std::sort(v.begin(), v.end());

  vector<vector<int>> dp(g + 1, vector<int>(m + 1, -1));
  dp[0][0] = 0;
  int st = 0;
  for (int i = 1; i <= g; ++i) {
    for (int k = st; (k < n) && (v[k].first.first == v[st].first.first); ++k) {
      for (int j = 0; j <= m; ++j) {
        if (dp[i - 1][j] > dp[i][j]) {
          dp[i][j] = dp[i - 1][j];
        }
        int t = j - v[k].first.second;
        if ((t >= 0) && (dp[i - 1][t] != -1) &&
            (dp[i][j] < dp[i - 1][t] + v[k].second)) {
          dp[i][j] = dp[i - 1][t] + v[k].second;
        }
      }
      if (((k + 1) < n) && (v[k + 1].first.first != v[st].first.first)) {
        st = k + 1;
        break;
      }
    }
  }
  int ans = 0;
  for (int i = 0; i <= m; ++i) {
    ans = std::max(ans, dp[g][i]);
  }
  cout << ans << '\n';
}
