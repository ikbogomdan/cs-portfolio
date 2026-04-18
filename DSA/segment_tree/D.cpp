#include <algorithm>
#include <iostream>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::vector;

struct Segt {
  vector<long long int> tre;
  int Fin(int v, int lt, int rt, int j) {
    if (rt - lt == 1) {
      return lt;
    }
    int m = (lt + rt) / 2;
    if (tre[v * 2 + 1] < j) {
      return Fin(v * 2 + 2, m, rt, j - tre[v * 2 + 1]);
    }
    return Fin(v * 2 + 1, lt, m, j);
  }
  int Fin(int k_find) { return Fin(0, 0, (tre.size() + 1) / 2, k_find + 1); }
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
  vector<long long int> a;
  long long int x;
  while (cin >> x) {
    a.push_back(x);
  }
  int n = (int)a.size();
  if (n == 1) {
    cout << 0 << '\n';
    return 0;
  }
  vector<long long int> tr_build(n, 1);
  Segt tree;
  tree.Build(tr_build);
  vector<long long int> perm(n);

  for (int i = 0; i < n; ++i) {
    int pos = tree.Fin(a[i]);
    perm[pos] = i;
    tree.Setn(pos, 0);
  }
  vector<long long int> perm_inv(n);
  int cur = 0;
  for (int i = 0; i < n; ++i) {
    perm_inv[perm[i]] = cur++;
  }
  vector<long long int> ans(n + 4, 0);
  Segt tree2;
  tree2.Build(ans);
  for (int i = 0; i < n; ++i) {
    int x = perm_inv[i];
    tree2.Setn(x, 1);
    ans[x] = tree2.Sum(x + 1, n + 2);
  }
  for (int i = 0; i < n; ++i) {
    cout << ans[i] << ' ';
  }
}

