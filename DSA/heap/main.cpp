#include <algorithm>
#include <climits>
#include <iostream>
#include <string>
#include <vector>
using std::cin;
using std::cout;
using std::pair;
using std::vector;
class Heap {
  int n_ = 0;
  static const int cSiz = 1000000;
  vector<pair<long long int, long long int>> elem_;
  vector<pair<long long int, long long int>> pos_in_;
  int Size() const { return n_; }
  void Siftup(int i) {
    if (i == 0) {
      return;
    }
    if (elem_[(i - 1) / 2].first > elem_[i].first) {
      int tmp = (i - 1) / 2;
      pos_in_[elem_[tmp].second].first = i;
      pos_in_[elem_[i].second].first = tmp;
      std::swap(elem_[i], elem_[(i - 1) / 2]);
      Siftup((i - 1) / 2);
    }
  }
  void Siftdown(int i) {
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    if ((left < n_) && (right < n_) &&
        (elem_[i].first > std::min(elem_[left].first, elem_[right].first))) {
      if (elem_[left].first <= elem_[right].first) {
        pos_in_[elem_[left].second].first = i;
        pos_in_[elem_[i].second].first = left;
        std::swap(elem_[i], elem_[left]);
        Siftdown(left);
      } else {
        pos_in_[elem_[right].second].first = i;
        pos_in_[elem_[i].second].first = right;
        std::swap(elem_[i], elem_[right]);
        Siftdown(right);
      }
      return;
    }
    if ((left < n_) && (elem_[i] > elem_[left])) {
      pos_in_[elem_[left].second].first = i;
      pos_in_[elem_[i].second].first = left;
      std::swap(elem_[i], elem_[left]);
      Siftdown(left);
    }
  }

 public:
  Heap() { pos_in_.resize(cSiz); }
  void Del() {
    pos_in_[elem_[n_ - 1].second].first = 0;
    std::swap(elem_[0], elem_[n_ - 1]);
    --n_;
    elem_.pop_back();
    Siftdown(0);
  }
  void Del(int k) {
    elem_[k].first = INT_MIN;
    Siftup(k);
    pos_in_[elem_[n_ - 1].second].first = 0;
    std::swap(elem_[0], elem_[n_ - 1]);
    --n_;
    elem_.pop_back();
    Siftdown(0);
  }
  void Decrease(int i, long long int delta) {
    elem_[pos_in_[i].first].first -= delta;
    Siftup(pos_in_[i].first);
  }
  long long int Getmin() { return elem_[0].first; }
  void Add(long long int k, int i) {
    elem_.push_back({k, i});
    ++n_;
    pos_in_[i].first = n_ - 1;
    Siftup(n_ - 1);
  }
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  std::cout.tie(0);
  int q;
  cin >> q;
  Heap a;
  for (int i = 0; i < q; ++i) {
    std::string s;
    cin >> s;
    if (s == "insert") {
      long long int k;
      cin >> k;
      a.Add(k, i);
    }
    if (s == "getMin") {
      cout << a.Getmin() << '\n';
    }
    if (s == "extractMin") {
      a.Del();
    }
    if (s == "decreaseKey") {
      int pos;
      long long int k;
      cin >> pos >> k;
      a.Decrease(pos - 1, k);
    }
  }
}