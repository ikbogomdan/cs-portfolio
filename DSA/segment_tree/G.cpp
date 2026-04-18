#include <algorithm>
#include <iostream>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::vector;

const int cMax = 1e9;

class Segt {
  vector<long long int> tre_;
  vector<vector<long long int>> tresrt_;
  int x_zap_;
  int y_zap_;
  int cnt_ans_;

 public:
  void Permutations(int ver, int lt, int rt, int left, int right) {
    if ((left >= rt) || (right <= lt)) {
      return;
    }
    if (rt - lt == 1) {
      if ((tre_[ver] >= x_zap_) && (tre_[ver] <= y_zap_)) {
        cnt_ans_++;
      }
      return;
    }
    if ((lt >= left) && (rt <= right)) {
      int left_bin = 0;
      int right_bin = tresrt_[ver].size();
      if ((tresrt_[ver][left_bin] > y_zap_) ||
          (tresrt_[ver][right_bin - 1] < x_zap_)) {
        return;
      }
      int lft_pos;
      int rght_pos;
      while (right_bin - left_bin > 1) {
        int mid_bin = (left_bin + right_bin) / 2;
        if (tresrt_[ver][mid_bin] <= x_zap_) {
          left_bin = mid_bin;
        } else {
          right_bin = mid_bin;
        }
      }
      lft_pos = left_bin;
      if (tresrt_[ver][lft_pos] < x_zap_) {
        ++lft_pos;
      }
      left_bin = 0;
      right_bin = tresrt_[ver].size();
      while (right_bin - left_bin > 1) {
        int mid_bin = (left_bin + right_bin) / 2;
        if (tresrt_[ver][mid_bin] <= y_zap_) {
          left_bin = mid_bin;
        } else {
          right_bin = mid_bin;
        }
      }
      rght_pos = left_bin;
      if (tresrt_[ver][lft_pos] > y_zap_) {
        return;
      }
      cnt_ans_ += (rght_pos - lft_pos + 1);
      return;
    }
    Permutations(2 * ver + 1, lt, (rt + lt) / 2, left, right);
    Permutations(2 * ver + 2, (rt + lt) / 2, rt, left, right);
  }
  int Permutations(int left, int right, int x, int y) {
    x_zap_ = x;
    y_zap_ = y;
    cnt_ans_ = 0;
    Permutations(0, 0, (tre_.size() + 1) / 2, left, right);
    return cnt_ans_;
  }
  void Build(vector<long long int>& a, int ver, int left, int right) {
    if (right - left == 1) {
      if (left < static_cast<int>(a.size())) {
        tre_[ver] = a[left];
        tresrt_[ver].push_back(a[left]);
      } else {
        tresrt_[ver].push_back(cMax);
      }
      return;
    }
    Build(a, 2 * ver + 1, left, (right + left) / 2);
    Build(a, 2 * ver + 2, (right + left) / 2, right);
    tre_[ver] = tre_[2 * ver + 1] + tre_[2 * ver + 2];
    int lj = 0;
    int rj = 0;
    int lid = 2 * ver + 1;
    int rid = 2 * ver + 2;
    while ((lj < static_cast<int>(tresrt_[lid].size())) &&
           (rj < static_cast<int>(tresrt_[rid].size()))) {
      if (tresrt_[lid][lj] <= tresrt_[rid][rj]) {
        tresrt_[ver].push_back(tresrt_[lid][lj]);
        lj++;
      } else {
        tresrt_[ver].push_back(tresrt_[rid][rj]);
        rj++;
      }
    }
    if (lj == (int)tresrt_[lid].size()) {
      while (rj != static_cast<int>(tresrt_[rid].size())) {
        tresrt_[ver].push_back(tresrt_[rid][rj]);
        rj++;
      }
    } else {
      while (lj != static_cast<int>(tresrt_[lid].size())) {
        tresrt_[ver].push_back(tresrt_[lid][lj]);
        lj++;
      }
    }
  }
  void Build(vector<long long int>& a) {
    int sz = 1;
    while (sz < static_cast<int>(a.size())) {
      sz *= 2;
    }
    tre_.assign(sz * 2 - 1, 0);
    tresrt_.resize(sz * 2 - 1);
    Build(a, 0, 0, sz);
  }
};

int main() {
  int n;
  int q;
  cin >> n >> q;
  vector<long long int> a(n);
  for (int i = 0; i < n; ++i) {
    cin >> a[i];
  }
  Segt tree;
  tree.Build(a);
  for (int i = 0; i < q; ++i) {
    int left;
    int right;
    int x_zap;
    int y_zap;
    cin >> left >> right >> x_zap >> y_zap;
    cout << tree.Permutations(left - 1, right, x_zap, y_zap) << '\n';
  }
}
