#include <iostream>
#include <set>
#include <vector>

std::vector<int> Man(const std::string& str_1) {
  std::string str;
  for (int i = 0; i < (int)str_1.size(); ++i) {
    str += str_1[i];
    if (i != (int)str_1.size() - 1) {
      str += "#";
    }
  }
  std::vector<int> res(str.size(), 0);
  res[0] = 1;
  int cur_l = 0;
  int cur_r = 0;

  for (int i = 1; i < (int)str.size(); ++i) {
    int cur_ans = 1;
    if (i < cur_r) {
      int m = (cur_l + cur_r) / 2;
      int sem_pos = 2 * m - i;
      cur_ans = std::min(res[sem_pos], cur_r - i);
    }
    while ((i - cur_ans >= 0) && (str[i - cur_ans] == str[i + cur_ans])) {
      ++cur_ans;
    }
    res[i] = cur_ans;
    if (i % 2 == 1) {
      if (res[i] % 2 == 1) {
        --res[i];
      }
    } else if (res[i] % 2 == 0) {
      --res[i];
    }
    if (res[i] + i - 1 > cur_r) {
      cur_l = i - res[i] + 1;
      cur_r = i + res[i] - 1;
    }
  }
  return res;
}

int main() {
  std::string in;
  std::cin >> in;
  std::vector<int> answer = Man(in);
  for (auto x : answer) {
    std::cout << x << ' ';
  }
}

