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

int n;
vector<vector<int>> g;
vector<int> neighbor_left;
vector<int> neighbor_right;
vector<bool> clique_left;
vector<int> clique_cnt;
vector<int> corr;

int main() {
  cin >> n;
  int k = n / 2;
  g.resize(n);
  neighbor_left.resize(k);
  neighbor_right.resize(n - k);
  clique_left.resize((1 << k), false);
  clique_cnt.resize((1 << (n - k)), 0);
  corr.resize((1 << k), 0);
  for (int i = 0; i < n; ++i) {
    string s;
    cin >> s;
    for (int j = 0; j < n; ++j) {
      int t = (s[j] - '0');
      g[i].push_back(t);
    }
  }
  for (int i = 0; i < k; ++i) {
    int t = 0;
    for (int j = 0; j < k; ++j) {
      if (g[i][j] == 1) {
        t += (1 << j);
      }
    }
    neighbor_left[i] = t;
  }

  for (int i = k; i < n; ++i) {
    int t = 0;
    for (int j = k; j < n; ++j) {
      if (g[i][j] == 1) {
        t += (1 << (j - k));
      }
    }
    neighbor_right[i - k] = t;
  }

  int last_deg = 0;
  clique_left[0] = true;
  for (int mask = 1; mask < (1 << k); ++mask) {
    if (mask == (1 << last_deg)) {
      ++last_deg;
    }
    int mask_raz = mask ^ (1 << (last_deg - 1));
    clique_left[mask] =
        (clique_left[mask_raz] &&
         ((mask_raz & neighbor_left[last_deg - 1]) == mask_raz));
  }

  last_deg = 0;
  clique_cnt[0] = 1;

  for (int mask = 1; mask < (1 << (n - k)); ++mask) {
    if (mask == (1 << last_deg)) {
      ++last_deg;
    }
    int mask_raz = mask ^ (1 << (last_deg - 1));
    int mask2 = mask & neighbor_right[last_deg - 1];
    clique_cnt[mask] = clique_cnt[mask_raz] + clique_cnt[mask2];
  }

  for (int i = 0; i < k; ++i) {
    int t = 0;
    for (int j = k; j < n; ++j) {
      if (g[i][j] == 1) {
        t += (1 << (j - k));
      }
    }
    neighbor_left[i] = t;
  }
  last_deg = 0;
  corr[0] = (1 << (n - k)) - 1;
  for (int mask = 1; mask < (1 << k); ++mask) {
    if (mask == (1 << last_deg)) {
      ++last_deg;
    }
    corr[mask] =
        corr[mask ^ (1 << (last_deg - 1))] & neighbor_left[last_deg - 1];
  }

  long long int ans = 0;
  for (int i = 1; i < (1 << k); ++i) {
    if (!clique_left[i]) {
      continue;
    }
    int mask = corr[i];
    ans += clique_cnt[mask];
  }
  ans += clique_cnt[(1 << (n - k)) - 1];
  cout << ans << '\n';
}

