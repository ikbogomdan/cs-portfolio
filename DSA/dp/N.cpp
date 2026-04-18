#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  int n;
  int m;
  cin >> n >> m;
  vector<int> a(n);
  vector<int> b(m);
  for (int i = 0; i < n; ++i) {
    cin >> a[i];
  }
  for (int i = 0; i < m; ++i) {
    cin >> b[i];
  }

  vector<vector<int>> d(n + 1, vector<int>(m + 1, 0));
  vector<int> pr(m + 1, 0);

  for (int i = 1; i <= n; ++i) {
    int daun = 0;
    int bb = 0;

    for (int j = 1; j <= m; ++j) {
      d[i][j] = d[i - 1][j];

      if ((a[i - 1] == b[j - 1]) && (d[i - 1][j] < bb + 1)) {
        d[i][j] = bb + 1;
        pr[j] = daun;
      }

      if ((a[i - 1] > b[j - 1]) && (d[i - 1][j] > bb)) {
        bb = d[i - 1][j];
        daun = j;
      }
    }
  }
  int mbls = 0;
  for (int j = 1; j <= m; ++j) {
    mbls = max(mbls, d[n][j]);
  }
  cout << mbls;

  return 0;
}
