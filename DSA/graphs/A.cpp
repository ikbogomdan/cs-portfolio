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

void Viv(int finish, int cur, vector<int>& p) {
  cout << "YES\n";
  vector<int> ans;
  ans.push_back(finish);
  while (cur != finish) {
    ans.push_back(cur);
    cur = p[cur];
  }
  for (int i = (int)ans.size() - 1; i >= 0; i--) {
    cout << ans[i] + 1 << ' ';
  }
  exit(0);
}
void Dfs(int v, vector<int>& c, vector<int>& p, vector<vector<int>>& g) {
  c[v] = 1;
  for (int i = 0; i < (int)g[v].size(); ++i) {
    int u = g[v][i];
    if (c[u] == 1) {
      Viv(u, v, p);
    }
    if (c[u] != 0) {
      continue;
    }
    p[u] = v;
    Dfs(u, c, p, g);
  }
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
      cout << "YES\n" << u << ' ' << u << '\n';
      return 0;
    }
    if (ed.find({u, v}) == ed.end()) {
      g[u - 1].push_back(v - 1);
      ed.insert({u, v});
    }
  }

  vector<int> c(n, 0);
  vector<int> p(n);
  for (int i = 0; i < n; ++i) {
    if (c[i] == 0) {
      Dfs(i, c, p, g);
    }
  }
  cout << "NO\n";
}

