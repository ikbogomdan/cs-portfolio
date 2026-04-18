#include <iostream>
#include <set>
#include <vector>

std::vector<int> Pref(const std::string& str) {
  std::vector<int> res(str.size(), 0);
  if (str.size() > 1 && (str[1] == str[0])) {
    res[1] = 1;
  }
  for (int i = 2; i < (int)str.size(); ++i) {
    int cur_ans = res[i - 1];
    while (cur_ans != 0 && (str[i] != str[cur_ans])) {
      cur_ans = res[cur_ans - 1];
    }
    if (str[i] == str[cur_ans]) {
      res[i] = cur_ans + 1;
    }
  }
  return res;
}

int main() {
  std::string in;
  std::cin >> in;
  std::vector<int> answer = Pref(in);
  for (auto x : answer) {
    std::cout << x << ' ';
  }
}

