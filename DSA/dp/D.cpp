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

int main() {
  string s1;
  string t1;
  cin >> s1 >> t1;
  string s = "a";
  string t = "a";
  s += s1;
  t += t1;

  int l1 = (int)s.length();
  int l2 = (int)t.length();
  vector<vector<int>> dp(l1 + 1, vector<int>(l2 + 1, 0));
  vector<vector<int>> brat(l1 + 1, vector<int>(l2 + 1, 0));

  for (int i = 1; i < l1; ++i) {
    for (int j = 1; j < l2; ++j) {
      dp[i][j] = std::max(((int)(s[i] == t[j]) * (dp[i - 1][j - 1] + 1)),
                          std::max(dp[i - 1][j], dp[i][j - 1]));
      if ((int)(s[i] == t[j]) * (dp[i - 1][j - 1] + 1) >
          std::max(dp[i - 1][j], dp[i][j - 1])) {
        brat[i][j] = 3;
      } else if (dp[i - 1][j] > dp[i][j - 1]) {
        brat[i][j] = 2;
      } else {
        brat[i][j] = 1;
      }
    }
  }
  cout << dp[l1 - 1][l2 - 1] << '\n';
  if (dp[l1 - 1][l2 - 1] == 0) {
    return 0;
  }
  string ans = "a";
  ans.pop_back();
  int i1 = l1 - 1;
  int j1 = l2 - 1;
  while (j1 != 0 && i1 != 0) {
    if (brat[i1][j1] == 3) {
      ans += s[i1];
      --i1;
      --j1;
    } else if (brat[i1][j1] == 2) {
      --i1;
    } else {
      --j1;
    }
  }
  reverse(ans.begin(), ans.end());
  int pos = 0;
  for (int i = 1; i < l1; ++i) {
    if (pos == (int)ans.size()) {
      break;
    }
    if (ans[pos] == s[i]) {
      cout << i << ' ';
      ++pos;
    }
  }
  cout << '\n';
  pos = 0;
  for (int i = 1; i < l2; ++i) {
    if (pos == (int)ans.size()) {
      break;
    }
    if (ans[pos] == t[i]) {
      cout << i << ' ';
      ++pos;
    }
  }
}
