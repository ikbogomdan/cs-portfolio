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
  int n;
  cin >> n;
  vector<pair<long long int, long long int>> a(n);
  vector<long long int> comp(2 * n);

  for (int i = 0; i < n; ++i) {
    cin >> a[i].first >> a[i].second;
    comp[2 * i] = a[i].first;
    comp[2 * i + 1] = a[i].second;
  }
  sort(comp.begin(), comp.end());
  vector<long long int> t;
  t.push_back(comp[0]);
  for (int i = 1; i < (int)comp.size(); ++i) {
    if (t.back() == comp[i]) {
      continue;
    }
    t.push_back(comp[i]);
  }
  for (int i = 0; i < (int)a.size(); ++i) {
    int left = 0;
    int right = t.size();
    long long int x1 = a[i].first;
    while (right - left > 1) {
      int m = (left + right) / 2;
      if (t[m] <= x1) {
        left = m;
      } else {
        right = m;
      }
    }
    a[i].first = left;
    left = 0;
    right = t.size();
    x1 = a[i].second;
    while (right - left > 1) {
      int m = (left + right) / 2;
      if (t[m] <= x1) {
        left = m;
      } else {
        right = m;
      }
    }
    a[i].second = left;
  }
  vector<long long int> tobu(t.size() + 1, 0);
  Segt tree;
  tree.Build(tobu);
  for (int i = 0; i < (int)a.size(); ++i) {
    a[i].first *= -1;
  }
  sort(a.begin(), a.end());
  long long int ans = 0;
  for (int i = 0; i < (int)a.size(); ++i) {
    long long int left = -a[i].first;
    long long int right = a[i].second;
    long long int k = tree.Sum(left, right + 1);
    ans += k;
    tree.Tosetn(right, 1);
  }
  for (int i = 0; i < (int)a.size(); ++i) {
    long long int k = 0;
    while ((i + 1 < (int)a.size()) && (a[i] == a[i + 1])) {
      ++k;
      ++i;
    }
    if (k != 0) {
      ans -= (k * (k + 1) / 2);
      --i;
    }
  }
  cout << ans << '\n';
}

