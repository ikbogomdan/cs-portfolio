#include <iostream>
#include <string>
#include <vector>

const int cMa = 27;

class SufMas {
 public:
  const std::string& c_str;
  int sz;
  std::vector<std::vector<int>> perm;
  std::vector<int> c;

  SufMas(const std::string& s)
      : c_str(s),
        sz(c_str.size()),
        perm(std::vector<std::vector<int>>(2,
                                           std::vector<int>(c_str.size(), 0))),
        c(c_str.size(), 0) {}

  int Iter() {
    int perm_pos = 0;
    int pos;
    std::vector<int> cnt(cMa, 0);
    for (int i = 0; i < sz; ++i) {
      if (c_str[i] == '#') {
        pos = 0;
      } else {
        pos = c_str[i] - 'a' + 1;
      }
      cnt[pos]++;
    }
    for (int i = 1; i < cMa; ++i) {
      cnt[i] += cnt[i - 1];
    }
    for (int i = sz - 1; i >= 0; --i) {
      if (c_str[i] == '#') {
        pos = 0;
      } else {
        pos = c_str[i] - 'a' + 1;
      }
      perm[perm_pos][--cnt[pos]] = i;
    }
    int l = 0;
    for (int i = 1; i < sz; ++i) {
      if (c_str[perm[perm_pos][i]] != c_str[perm[perm_pos][i - 1]]) {
        ++l;
      }
      c[perm[perm_pos][i]] = l;
    }
    return l;
  }

  void Build() {
    int k = 0;
    std::vector<int> cnt(sz, 0);
    int pos;
    int last_class_sz = Iter();
    while ((1 << k) < sz) {
      for (int i = 0; i < sz; ++i) {
        cnt[i] = 0;
      }
      for (int i = 0; i < sz; ++i) {
        ++cnt[c[i]];
      }
      for (int i = 1; i <= last_class_sz; ++i) {
        cnt[i] += cnt[i - 1];
      }
      int perm_last = 0;
      int perm_new = 1;
      for (int i = 0; i < sz; ++i) {
        perm[perm_new][i] = (perm[perm_last][i] - (1 << k) + sz) % sz;
      }

      for (int i = sz - 1; i >= 0; --i) {
        pos = perm[perm_new][i];
        perm[perm_last][--cnt[c[pos]]] = pos;
      }
      c[perm[0][0]] = 0;
      std::vector<int> cnew(sz, 0);
      int l = 0;
      for (int i = 1; i < sz; ++i) {
        if (std::make_pair(c[perm[perm_last][i - 1]],
                           c[(perm[perm_last][i - 1] + (1 << k)) % sz]) !=
            std::make_pair(c[perm[perm_last][i]],
                           c[(perm[perm_last][i] + (1 << k)) % sz])) {
          ++l;
        }
        cnew[perm[0][i]] = l;
      }

      c = cnew;
      last_class_sz = l;
      ++k;
    }
  }
};

int main() {
  std::string s;
  std::cin >> s;
  s += '#';
  SufMas t(s);
  t.Build();
  for (int i = 1; i < (int)s.size(); ++i) {
    std::cout << t.perm[0][i] + 1 << ' ';
  }
}

