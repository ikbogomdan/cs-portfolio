#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::vector;

struct Segt {
  vector<long long int> tre;
  void Find(int v, int lt, int rt, int pos, long long int value, int& cur) {
    if (rt - lt == 1) {
      cur = lt;
      return;
    }
    int m = (lt + rt) / 2;
    if ((cur == -1) && (m > pos) && (tre[2 * v + 1] >= value)) {
      Find(2 * v + 1, lt, m, pos, value, cur);
    }
    if ((cur == -1) && (rt > pos) && (tre[2 * v + 2] >= value)) {
      Find(2 * v + 2, m, rt, pos, value, cur);
    }
  }
  long long int Find(int pos, long long int value) {
    int cur = -1;
    Find(0, 0, (tre.size() + 1) / 2, pos, value, cur);
    return cur;
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
    tre[v] = std::max(tre[2 * v + 1], tre[2 * v + 2]);
  }
  void Setn(int j, long long int x) { Setn(0, 0, (tre.size() + 1) / 2, j, x); }
  void Build(vector<long long int>& a, int v, int l, int r) {
    if (r - l == 1) {
      if (l < (int)a.size()) {
        tre[v] = a[l];
      }
      return;
    }
    Build(a, 2 * v + 1, l, (r + l) / 2);
    Build(a, 2 * v + 2, (r + l) / 2, r);
    tre[v] = std::max(tre[2 * v + 1], tre[2 * v + 2]);
  }
  void Build(vector<long long int>& a) {
    int sz = 1;
    while (sz < (int)a.size()) {
      sz *= 2;
    }
    tre.assign(sz * 2 - 1, LLONG_MIN);
    Build(a, 0, 0, sz);
  }
};

int main() {
  int n;
  int q;
  cin >> n >> q;
  Segt tree;
  vector<long long int> a(n, 0);
  for (int i = 0; i < n; ++i) {
    cin >> a[i];
  }
  tree.Build(a);
  for (int tt = 0; tt < q; ++tt) {
    int type;
    int pos;
    int x;
    cin >> type >> pos >> x;
    if (type == 0) {
      tree.Setn(pos - 1, x);
    } else {
      int f = tree.Find(pos - 1, x) + 1;
      if (f == 0) {
        --f;
      }
      cout << f << '\n';
    }
  }
}
