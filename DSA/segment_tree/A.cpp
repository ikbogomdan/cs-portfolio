#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::vector;

const int cPort = 1e5 + 5;
const int cMeters = 42200;
const int cPrec = 15;

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
  int q;
  cin >> q;

  vector<int> a(cPort, 0);
  vector<long long int> b(cMeters + 3, 0);
  Segt tree;
  tree.Build(b);

  for (int tt = 0; tt < q; ++tt) {
    std::string s;
    cin >> s;
    if (s == "RUN") {
      int x;
      int y;
      cin >> x >> y;
      --x;
      long long int tmp = a[x];
      long long int tmpsum;
      if (tmp != 0) {
        tmpsum = tree.Sum(tmp, tmp + 1);
        tree.Setn(tmp, tmpsum - 1);
      }
      a[x] = y;
      tmpsum = tree.Sum(y, y + 1);
      tree.Setn(y, tmpsum + 1);
    }
    if (s == "CHEER") {
      int pos;
      cin >> pos;
      --pos;
      if (a[pos] == 0) {
        cout << 0 << '\n';
        continue;
      }
      if ((tree.Sum(0, a[pos]) == 0) &&
          (tree.Sum(a[pos] + 1, cMeters + 1) == 0) &&
          (tree.Sum(a[pos], a[pos] + 1) == 1)) {
        cout << 1 << '\n';
        continue;
      }
      double viv =
          (double)tree.Sum(0, a[pos]) / (double)(tree.Sum(0, cMeters + 1) - 1);
      std::cout << std::setprecision(cPrec) << viv << '\n';
    }
  }
}

