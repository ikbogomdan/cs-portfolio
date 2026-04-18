#include <algorithm>
#include <iostream>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::vector;

struct Segt {
  vector<long long int> tre;
  void Tosetn(int v, int lt, int rt, int j, long long int x) {
    if (rt - lt == 1) {
      tre[v] += x;
      return;
    }
    int m = (lt + rt) / 2;
    if (j < m) {
      Tosetn(2 * v + 1, lt, m, j, x);
    } else {
      Tosetn(2 * v + 2, m, rt, j, x);
    }
    tre[v] = tre[2 * v + 1] + tre[2 * v + 2];
  }
  void Tosetn(int j, long long int x) {
    Tosetn(0, 0, (tre.size() + 1) / 2, j, x);
  }
  void Setn(int v, int lt, int rt, int j, long long int x) {
    if (rt - lt == 1) {
      tre[v] = x;
      return;
    }
    int m = (lt + rt) / 2;
    if (j < m) {
      Setn(2 * v + 1, lt, m, j, x);
    } else {
      Setn(2 * v + 2, m, rt, j, x);
    }
    tre[v] = tre[2 * v + 1] + tre[2 * v + 2];
  }
  void Setn(int j, long long int x) { Setn(0, 0, (tre.size() + 1) / 2, j, x); }
  long long int Sum(int v, int lt, int rt, int l, int r) {
    if (rt - lt == 1) {
      if ((lt >= l) && (rt <= r)) {
        return tre[v];
      }
      return 0;
    }
    if ((l >= rt) || (r <= lt)) {
      return 0;
    }
    if ((lt >= l) && (rt <= r)) {
      return tre[v];
    }
    long long int s1;
    long long int s2;
    s1 = Sum(2 * v + 1, lt, (rt + lt) / 2, l, r);
    s2 = Sum(2 * v + 2, (rt + lt) / 2, rt, l, r);
    return (s1 + s2);
  }
  long long int Sum(int l, int r) {
    return Sum(0, 0, (tre.size() + 1) / 2, l, r);
  }
  void Build(vector<long long int>& a, int v, int l, int r) {
    if (r - l == 1) {
      if (l < (int)a.size()) {
        tre[v] = a[l];
      } else {
        tre[v] = 0;
      }
      return;
    }
    Build(a, 2 * v + 1, l, (r + l) / 2);
    Build(a, 2 * v + 2, (r + l) / 2, r);
    tre[v] = tre[2 * v + 1] + tre[2 * v + 2];
  }
  void Build(vector<long long int>& a) {
    int sz = 1;
    while (sz < (int)a.size()) {
      sz *= 2;
    }
    tre.assign(sz * 2 - 1, 0);
    Build(a, 0, 0, sz);
  }
};

int main() {
  int q;
  cin >> q;
  vector<long long int> g;
  vector<pair<int, int>> t(q);
  for (int i = 0; i < q; i++) {
    char c;
    cin >> c;
    if (c == '?') {
      int x;
      cin >> x;
      t[i].first = 0;
      t[i].second = x;
    } else {
      int x;
      cin >> x;
      t[i].first = 1;
      t[i].second = x;
      g.push_back(x);
    }
  }
  sort(g.begin(), g.end());
  if (g.empty()) {
    for (int i = 0; i < (int)t.size(); ++i) {
      cout << 0 << '\0';
    }
    return 0;
  }
  vector<long long int> a;
  a.push_back(g[0]);
  for (int i = 1; i < (int)g.size(); ++i) {
    if (a.back() == g[i]) {
      continue;
    }
    a.push_back(g[i]);
  }
  Segt tree;
  vector<long long int> tmgp(a.size() + 1, 0);
  tree.Build(tmgp);
  tmgp.clear();

  for (int i = 0; i < q; ++i) {
    if (t[i].first == 1) {
      int left = 0;
      int right = a.size();
      int tmp = t[i].second;
      while (right - left > 1) {
        int m = (left + right) / 2;
        if (a[m] <= tmp) {
          left = m;
        } else {
          right = m;
        }
      }
      tree.Tosetn(left, a[left]);
    } else {
      int left = 0;
      int right = a.size();
      int tmp = t[i].second;
      if ((a.empty()) || (a[0] > tmp)) {
        cout << 0 << '\n';
        continue;
      }
      if (a.back() <= tmp) {
        cout << tree.Sum(0, a.size() + 1) << '\n';
        continue;
      }
      while (right - left > 1) {
        int m = (left + right) / 2;
        if (a[m] <= tmp) {
          left = m;
        } else {
          right = m;
        }
      }
      cout << tree.Sum(0, left + 1) << '\n';
    }
  }
}
