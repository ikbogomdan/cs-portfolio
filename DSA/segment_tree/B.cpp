#include <algorithm>
#include <iostream>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::vector;

struct Segt {
  vector<long long int> tre;
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
  int n;
  cin >> n;
  Segt tree;

  vector<long long int> a(n);
  for (int i = 0; i < n; ++i) {
    cin >> a[i];
    if (i % 2 == 1) {
      a[i] *= (-1);
    }
  }
  tree.Build(a);
  int q;
  cin >> q;
  for (int xui = 0; xui < q; ++xui) {
    int t;
    cin >> t;
    if (t == 0) {
      int pos;
      long long int x;
      cin >> pos >> x;
      if ((pos - 1) % 2 == 1) {
        x *= (-1);
      }
      tree.Setn(pos - 1, x);
    } else {
      int l;
      int r;
      cin >> l >> r;
      long long sum = tree.Sum(l - 1, r);
      if ((l - 1) % 2 == 1) {
        sum *= (-1);
      }
      cout << sum << '\n';
    }
  }
}
