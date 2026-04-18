#include <iostream>
#include <set>
#include <vector>

std::vector<int> Z(const std::string& str) {
  std::vector<int> res(str.size(), 0);
  int cur_l = 0;
  int cur_r = 0;

  for (int i = 1; i < (int)str.size(); ++i) {
    int cur_ans = 0;
    if (i < cur_r) {
      int bro_pos = i - cur_l;
      cur_ans = std::min(cur_r - i + 1, res[bro_pos]);
    }
    while ((i + cur_ans < (int)str.size()) &&
           (str[i + cur_ans] == str[cur_ans])) {
      ++cur_ans;
    }
    if (cur_ans - 1 + i > cur_r) {
      cur_r = cur_ans - 1 + i;
      cur_l = i;
    }
    res[i] = cur_ans;
  }
  return res;
}

int main() {
  std::string in;
  std::cin >> in;
  std::vector<int> answer = Z(in);
  for (auto x : answer) {
    std::cout << x << ' ';
  }
}

