#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <vector>
using std::cin;
using std::cout;
using std::max;
using std::min;
using std::pair;
using std::set;
using std::string;
using std::vector;

vector<int> mn;
vector<int> mx;
vector<int> prev;
vector<int> ans;
vector<int> sosed;
vector<int> own;
vector<bool> is_root;
std::map<pair<int, int>, int> num;
int timer = 0;
int cnt = 0;

void Dfs(int v, vector<int>& t, vector<int>& us, vector<vector<int>>& g) {
  us[v] = 1;
  t[v] = timer++;
  mn[v] = INT_MAX;
  for (int i = 0; i < (int)g[v].size(); ++i) {
    int u = g[v][i];

    if (us[u] != 0) {
      own[v] = min(own[v], t[u]);
      continue;
    }
    prev[u] = v;
    sosed[v]++;
    Dfs(u, t, us, g);
    mn[v] = min(mn[v], mn[u]);
    mx[v] = max(mx[v], mn[u]);
  }

  if (is_root[v]) {
    if (sosed[v] >= 2) {
      cnt++;
      ans.push_back(v + 1);
    }
  } else if (mx[v] >= t[v]) {
    ans.push_back(v + 1);
    ++cnt;
  }
  mn[v] = min(mn[v], own[v]);
}

int main() {
  int n;
  int m;
  cin >> n >> m;
  vector<vector<int>> g(n);
  set<pair<int, int>> ed;
  is_root.resize(n, false);
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
  prev.resize(n);
  vector<int> us(n, 0);
  sosed.resize(n, 0);
  own.resize(n, INT_MAX);
  vector<int> t(n, INT_MAX);
  mx.resize(n, INT_MIN);
  mn.resize(n, INT_MAX);
  for (int i = 0; i < n; ++i) {
    if (us[i] == 0) {
      prev[i] = -1;
      is_root[i] = true;
      Dfs(i, t, us, g);
    }
  }
  std::cout << cnt << '\n';
  std::sort(ans.begin(), ans.end());
  for (int i = 0; i < cnt; ++i) {
    std::cout << ans[i] << ' ';
  }
}

