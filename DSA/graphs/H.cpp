#include <algorithm>
#include <climits>
#include <iostream>
#include <set>
#include <vector>
using std::cin;
using std::cout;
using std::max;
using std::min;
using std::string;
using std::vector;

const int cMax = 30000;

int main() {
  int n;
  int m;
  cin >> n >> m;
  vector<vector<int>> g(n, vector<int>(n, cMax));
  for (int i = 0; i < m; i++) {
    int u;
    int v;
    int c;
    cin >> u >> v >> c;
    g[u - 1][v - 1] = min(g[u - 1][v - 1], c);
  }

  vector<vector<int>> dp(n, vector<int>(n, cMax));
  dp[0][0] = 0;
  for (int i = 1; i < n - 1; ++i) {
    for (int j = 0; j < n; ++j) {
      dp[j][i] = dp[j][i - 1];
      for (int k = 0; k < n; ++k) {
        if ((g[k][j] != cMax) && (dp[k][i - 1] != cMax)) {
          dp[j][i] = min(dp[j][i], dp[k][i - 1] + g[k][j]);
        }
      }
    }
  }
  if (n == 1) {
    std::cout << 0;
    return 0;
  }
  for (int i = 0; i < n; ++i) {
    std::cout << dp[i][n - 2] << ' ';
  }
}

