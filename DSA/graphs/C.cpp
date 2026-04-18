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

void Dfs(int v, vector<int>& c, vector<int>& time, vector<vector<int>>& g,
         int& timer) {
  c[v] = 1;
  for (int i = 0; i < (int)g[v].size(); ++i) {
    int u = g[v][i];
    if (c[u] == 1) {
      cout << "-1\n";
      exit(0);
    }
    if (c[u] != 0) {
      continue;
    }
    // time[u] = v;
    Dfs(u, c, time, g, timer);
  }
  time[timer++] = v;
  c[v] = 2;
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
    if (u == v) {
      cout << "-1\n";
      ;
      return 0;
    }
    if (ed.find({u, v}) == ed.end()) {
      g[u - 1].push_back(v - 1);
      ed.insert({u, v});
    }
  }
  int timer = 0;
  vector<int> c(n, 0);
  vector<int> time(n);
  for (int i = 0; i < n; ++i) {
    if (c[i] == 0) {
      Dfs(i, c, time, g, timer);
    }
  }
  for (int i = 0; i < n; ++i) {
    cout << time[n - i - 1] + 1 << ' ';
  }
}

