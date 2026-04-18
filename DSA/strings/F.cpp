#include <iostream>
#include <queue>
#include <string>
#include <vector>

const int cAlph = 26;
const int cMax = 1e6;

struct Node {
  std::vector<int> to = std::vector<int>(cAlph, -1);
  std::vector<int> go = std::vector<int>(cAlph, -1);
  int str_number = -1;
  bool terminal = false;
  Node* link = nullptr;
  Node* compressed;

  static std::vector<Node*> convert;
  Node(){};
  ~Node() {
    for (auto adr : to) {
      if (adr != -1) {
        delete convert[adr];
      }
    }
  }
};
std::vector<Node*> Node::convert = std::vector<Node*>(cMax, nullptr);

class Tree {
 public:
  Tree(int n) : links_between(n, -1) { Node::convert[vert_counter++] = root; }

  Node* root = new Node();
  std::vector<int> links_between;
  int str_cnt = 0;

  int vert_counter = 0;

  void Add(const std::string& t, int cnt) {
    auto* v = root;
    ++str_cnt;
    for (auto c : t) {
      if (v->to[c - 'a'] == -1) {
        Node::convert[vert_counter] = new Node();
        v->to[c - 'a'] = vert_counter++;
      }
      v = Node::convert[v->to[c - 'a']];
    }
    if (v->terminal) {
      links_between[cnt] = v->str_number;
    } else {
      v->str_number = cnt;
    }
    v->terminal = true;
  }

  void AhoKorasik() const {
    root->link = root;
    for (char c = 'a'; c <= 'z'; ++c) {
      if (root->to[c - 'a'] != -1) {
        root->go[c - 'a'] = root->to[c - 'a'];
      } else {
        root->go[c - 'a'] = 0;
      }
    }
    std::queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
      auto* v = q.front();
      q.pop();
      for (char c = 'a'; c <= 'z'; ++c) {
        if (v->to[c - 'a'] == -1) {
          continue;
        }
        auto* u = Node::convert[v->to[c - 'a']];
        if (u == nullptr) {
          continue;
        }
        if (v == root) {
          u->link = root;
        } else {
          u->link = Node::convert[v->link->go[c - 'a']];
        }
        for (char d = 'a'; d <= 'z'; ++d) {
          if (u->to[d - 'a'] != -1) {
            u->go[d - 'a'] = u->to[d - 'a'];
          } else {
            u->go[d - 'a'] = u->link->go[d - 'a'];
          }
        }
        q.push(u);
      }
    }
  }

  void CompressedCount() const {
    std::queue<Node*> q;
    q.push(root);
    root->compressed = root;
    while (!q.empty()) {
      auto* v = q.front();
      q.pop();
      for (char c = 'a'; c <= 'z'; ++c) {
        if (v->to[c - 'a'] == -1) {
          continue;
        }
        auto* u = Node::convert[v->to[c - 'a']];
        if (u == nullptr) {
          continue;
        }
        u->compressed = (u->link->terminal ? u->link : u->link->compressed);
        q.push(u);
      }
    }
  }

  std::vector<std::vector<int>> AllPos(const std::string& text) const {
    auto* v = root;
    std::vector<std::vector<int>> ret(str_cnt);
    for (int i = 0; i < (int)text.size(); ++i) {
      char c = text[i];
      v = Node::convert[v->go[c - 'a']];
      auto* u = v;
      while (u != root) {
        if (u->str_number != -1) {
          ret[u->str_number].push_back(i + 1);
        }
        u = u->compressed;
      }
    }
    return ret;
  }

  std::vector<int>& Links() { return links_between; }

  ~Tree() { delete root; }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::string text;
  std::cin >> text;
  int n;
  std::cin >> n;
  Tree t(n);
  std::vector<int> str_sizes(n);
  for (int i = 0; i < n; ++i) {
    std::string s;
    std::cin >> s;
    str_sizes[i] = s.size();
    t.Add(s, i);
  }

  t.AhoKorasik();
  t.CompressedCount();

  std::vector<std::vector<int>> ans = t.AllPos(text);
  const std::vector<int>& links = t.Links();

  for (int i = 0; i < (int)ans.size(); ++i) {
    int ind = i;
    if (links[i] != -1) {
      ind = links[i];
    }
    std::cout << ans[ind].size() << ' ';
    for (auto pos : ans[ind]) {
      std::cout << pos - str_sizes[i] + 1 << ' ';
    }
    std::cout << '\n';
  }
}
