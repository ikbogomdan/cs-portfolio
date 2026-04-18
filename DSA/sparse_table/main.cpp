#include <algorithm>
#include <climits>
#include <iostream>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::vector;
class Sparse {
  vector<int> a_or_;
  vector<vector<int>> a_min_;
  vector<vector<int>> a_minpos_;
  vector<int> min_pow_;
  int Minonseg(int left, int right) {
    if (right - left == 0) {
      return (INT_MAX);
    }
    int pow = min_pow_[right - left];
    return (std::min(a_min_[pow][left], a_min_[pow][right - (1 << pow)]));
  }

 public:
  Sparse(const vector<int>& a) {
    a_or_ = a;
    int n = static_cast<int>(a.size());
    int pow_two = 0;
    while ((1 << (pow_two + 1)) <= n) {
      ++pow_two;
    }
    min_pow_.resize(n + 1);
    {
      int pow_2 = 0;
      for (int i = 1; i < n + 1; ++i) {
        if (i > (1 << (pow_2 + 1))) {
          pow_2++;
        }
        min_pow_[i] = pow_2;
      }
    }
    a_min_.resize(pow_two + 1, vector<int>(n, 0));
    a_minpos_.resize(pow_two + 1, vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
      a_min_[0][i] = a[i];
      a_minpos_[0][i] = i;
    }
    for (int i = 1; i <= pow_two; ++i) {
      for (int j = 0; j + (1 << i) <= n; ++j) {
        a_min_[i][j] =
            std::min(a_min_[i - 1][j], a_min_[i - 1][j + (1 << (i - 1))]);
        if (a_min_[i - 1][j] < a_min_[i - 1][j + (1 << (i - 1))]) {
          a_minpos_[i][j] = a_minpos_[i - 1][j];
        } else {
          a_minpos_[i][j] = a_minpos_[i - 1][j + (1 << (i - 1))];
        }
      }
    }
  }

  int Ask_second_stat(int left, int right) {
    int pow = min_pow_[right - left];
    int minon =
        a_or_[a_minpos_[pow][left]] <= a_or_[a_minpos_[pow][right - (1 << pow)]]
            ? a_minpos_[pow][left]
            : a_minpos_[pow][right - (1 << pow)];
    int first_min = Minonseg(left, minon);
    int second_min = Minonseg(minon + 1, right);
    return std::min(first_min, second_min);
  }
  int Ask(int left, int right) {
    int pow = min_pow_[right - left];
    int minon =
        a_or_[a_minpos_[pow][left]] <= a_or_[a_minpos_[pow][right - (1 << pow)]]
            ? a_minpos_[pow][left]
            : a_minpos_[pow][right - (1 << pow)]; 
    return a_or_[minon];
  }
};

int main() {
  int n;
  int q;
  cin >> n >> q;
  vector<int> a(n);
  for (int i = 0; i < n; ++i) {
    cin >> a[i];
  }
  Sparse sparse(a);
  for (int i = 0; i < q; ++i) {
    int left;
    int right;
    cin >> left >> right;
    --left;
    cout << sparse.Ask_second_stat(left, right) << '\n';
  }
}