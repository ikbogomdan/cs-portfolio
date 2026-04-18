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
using std::max;
using std::min;
using std::pair;
using std::set;
using std::string;
using std::vector;
vector<vector<int>> ans;
void Dfs(int v, vector<int>& c, vector<int>& us, vector<vector<int>>& g,
         int k) {
  c[v] = k;
  ans[k].push_back(v + 1);
  us[v] = 1;
  for (int i = 0; i < (int)g[v].size(); ++i) {
    int u = g[v][i];
    if (us[u] != 0) {
      continue;
    }
    Dfs(u, c, us, g, k);
  }
}

int main() {
  int n;
  int m;
  cin >> n >> m;
  vector<vector<int>> g(n);
  set<pair<int, int>> ed;
  for (int i = 0; i < m; i++) {
    int u;
    int v;
    cin >> u >> v;
    int tmp = min(u, v);
    int tmp2 = max(u, v);
    u = tmp;
    v = tmp2;
    if (u == v) {
      continue;
    }
    if (ed.find({u, v}) == ed.end()) {
      g[u - 1].push_back(v - 1);
      g[v - 1].push_back(u - 1);
      ed.insert({u, v});
    }
  }

  vector<int> c(n, 0);
  vector<int> us(n, 0);
  int k = 0;
  ans.resize(n);
  for (int i = 0; i < n; ++i) {
    if (us[i] == 0) {
      Dfs(i, c, us, g, k);
      ++k;
    }
  }
  cout << k << '\n';
  for (int i = 0; i < k; ++i) {
    cout << ans[i].size() << '\n';
    for (int j = 0; j < (int)ans[i].size(); ++j) {
      cout << ans[i][j] << ' ';
    }
    cout << '\n';
  }
}

