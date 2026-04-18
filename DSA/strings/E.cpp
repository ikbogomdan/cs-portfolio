#include <iostream>
#include <queue>
#include <string>
#include <vector>

const int cAlph = 26;
const int cMa = 1e6 + 1;

struct Segt {
  std::vector<long long int> tre;
  void Setn(int v, int lt, int rt, int j, long long int x) {
    if (rt - lt == 1) {
      tre[v] += x;
      return;
    }
    int m = (lt + rt) / 2;
    if (j < m) {
      Setn(2 * v + 1, lt, m, j, x);
    } else {
      Setn(2 * v + 2, m, rt, j, x);
    }
    tre[v] = tre[2 * v + 1] + tre[2 * v + 2];
  }

  void Setn(int j, long long int x) { Setn(0, 0, (tre.size() + 1) / 2, j, x); }

  long long int Sum(int v, int lt, int rt, int l, int r) {
    if (rt - lt == 1) {
      if ((lt >= l) && (rt <= r)) {
        return tre[v];
      }
      return 0;
    }
    if ((l >= rt) || (r <= lt)) {
      return 0;
    }
    if ((lt >= l) && (rt <= r)) {
      return tre[v];
    }
    long long int s1;
    long long int s2;
    s1 = Sum(2 * v + 1, lt, (rt + lt) / 2, l, r);
    s2 = Sum(2 * v + 2, (rt + lt) / 2, rt, l, r);
    return (s1 + s2);
  }
  long long int Sum(int l, int r) {
    return Sum(0, 0, (tre.size() + 1) / 2, l, r);
  }
  void Build(const std::vector<long long int>& a, int v, int l, int r) {
    if (r - l == 1) {
      if (l < (int)a.size()) {
        tre[v] = a[l];
      } else {
        tre[v] = 0;
      }
      return;
    }
    Build(a, 2 * v + 1, l, (r + l) / 2);
    Build(a, 2 * v + 2, (r + l) / 2, r);
    tre[v] = tre[2 * v + 1] + tre[2 * v + 2];
  }
  void Build(const std::vector<long long int>& a) {
    int sz = 1;
    while (sz < (int)a.size()) {
      sz *= 2;
    }
    tre.assign(sz * 2 - 1, 0);
    Build(a, 0, 0, sz);
  }
};

struct Node {
  static std::vector<Node*> per;

  std::vector<int> to = std::vector<int>(cAlph, -1);
  int pr = 0;
  int our_vert;
  bool terminal = false;
  Node* link = nullptr;

  Node(){};
  ~Node() {
    for (auto adr : to) {
      if (adr != -1) {
        delete per[adr];
      }
    }
  }
};
std::vector<Node*> Node::per = std::vector<Node*>(cMa, nullptr);

class Tree {
 public:
  Tree() {
    Node::per[vert_counter++] = root;
    root->our_vert = 0;
  }

  Node* root = new Node();
  std::vector<int> nums;
  std::vector<int> prices;
  int str_cnt = 0;

  int vert_counter = 0;

  void Add(const std::string& t, int prrr) {
    auto* v = root;
    ++str_cnt;
    for (auto c : t) {
      if (v->to[c - 'a'] == -1) {
        Node::per[vert_counter] = new Node();
        Node::per[vert_counter]->our_vert = vert_counter;
        v->to[c - 'a'] = vert_counter++;
      }
      v = Node::per[v->to[c - 'a']];
    }
    v->terminal = true;
    nums.push_back(v->our_vert);
    v->pr = prrr;
  }

  std::vector<std::vector<int>> MakeGraph() {
    std::vector<std::vector<int>> g(vert_counter);
    prices.resize(vert_counter);
    std::queue<int> q;
    q.push(0);
    while (!q.empty()) {
      int cur_v = q.front();
      prices[cur_v] = Node::per[cur_v]->pr;
      q.pop();
      auto* v = Node::per[cur_v];
      for (char c = 'a'; c <= 'z'; ++c) {
        if (v->to[c - 'a'] == -1) {
          continue;
        }
        g[cur_v].push_back(v->to[c - 'a']);
        q.push(v->to[c - 'a']);
      }
    }
    return g;
  }

  const std::vector<int>& GetNums() const { return nums; }
  const std::vector<int>& GetPrices() const { return prices; }

  int Vrt() const { return vert_counter; }

  ~Tree() { delete root; }
};

std::vector<std::vector<int>> tres;

class HeavyLight {
 public:
  const std::vector<std::vector<int>>& g;
  const std::vector<int>& c;
  std::vector<int> heavy_cnt;
  std::vector<int> which_tree;
  std::vector<int> parent;
  std::vector<int> pos_in_tree;
  std::vector<int> v_len;
  const std::vector<int>& terminals;
  int last_tree = 0;

  std::vector<std::vector<long long int>> future_segs;
  std::vector<Segt> segs;

  void DfsCnt(int v, int cur_len) {
    v_len[v] = cur_len;
    if ((int)g[v].size() == 0) {
      heavy_cnt[v] = 1;
      return;
    }
    for (const auto& u : g[v]) {
      parent[u] = v;
      DfsCnt(u, cur_len + 1);
    }
    for (const auto& u : g[v]) {
      heavy_cnt[v] += heavy_cnt[u];
    }
    --cur_len;
  }

  void DfsSeg(int v) {
    if (which_tree[v] == -1) {
      future_segs.push_back(std::vector<long long int>());
      which_tree[v] = last_tree++;
      pos_in_tree[v] = 0;
      future_segs[which_tree[v]].push_back(v);
    }
    if (g[v].empty()) {
      return;
    }
    int max_vert = g[v][0];
    for (const auto& u : g[v]) {
      if (heavy_cnt[u] > heavy_cnt[max_vert]) {
        max_vert = u;
      }
    }
    for (const auto& u : g[v]) {
      if (u == max_vert) {
        which_tree[u] = which_tree[v];
        pos_in_tree[u] = static_cast<int>(future_segs[which_tree[u]].size());
        future_segs[which_tree[u]].push_back(u);
      }
      DfsSeg(u);
    }
  }

  void Cnt() {
    DfsCnt(0, 0);
    DfsSeg(0);
    for (int i = 0; i < (int)future_segs.size(); ++i) {
      int first = future_segs[i][0];
      for (int j = 1; j < (int)future_segs[i].size(); ++j) {
        parent[future_segs[i][j]] = first;
      }
    }
    auto tmp = future_segs;
    for (int i = 0; i < (int)future_segs.size(); ++i) {
      for (int j = 0; j < (int)future_segs[i].size(); ++j) {
        future_segs[i][j] = c[future_segs[i][j]];
      }
    }
    segs.resize(future_segs.size());
    for (int i = 0; i < (int)future_segs.size(); ++i) {
      segs[i].Build(future_segs[i]);
    }
    future_segs = std::move(tmp);
  }

  long long int Sum(int pizza, int len) {
    int v = terminals[pizza];
    long long int ret = 0;
    int meme = 0;
    while ((v != 0) && (v_len[v] >= len)) {
      ++meme;
      if (which_tree[v] == -1) {
        ret += c[v];
        v = parent[v];
        continue;
      }
      int tree_num = which_tree[v];
      int right = pos_in_tree[v] + 1;
      int left = static_cast<int>(future_segs[which_tree[v]][0]);
      if (v_len[left] < len) {
        left = len - v_len[left];
      } else {
        left = 0;
      }
      ret += segs[tree_num].Sum(left, right);

      int gf = which_tree[v];
      v = parent[v];
      if (which_tree[v] == gf) {
        v = parent[v];
      }
      if (meme > cMa) {
        break;
      }
    }
    return ret;
  }

  void Change(int pizza, int new_price) {
    segs[which_tree[terminals[pizza]]].Setn(pos_in_tree[terminals[pizza]],
                                            new_price);
  }

  HeavyLight(const std::vector<std::vector<int>>& q, const std::vector<int>& pa,
             const std::vector<int>& termal, int vrt)
      : g(q),
        c(pa),
        heavy_cnt(vrt),
        which_tree(vrt, -1),
        parent(vrt, 0),
        pos_in_tree(vrt, 0),
        v_len(vrt, 0),
        terminals(termal) {}
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n;
  int q;
  std::cin >> n >> q;
  Tree t;
  std::string sese;
  int nextprice;
  for (int i = 0; i < n; ++i) {
    std::cin >> sese;
    std::cin >> nextprice;
    t.Add(sese, nextprice);
  }
  auto g = t.MakeGraph();
  HeavyLight sol(g, t.GetPrices(), t.GetNums(), t.Vrt());
  sol.Cnt();
  for (int i = 0; i < q; ++i) {
    char c;
    int l;
    int r;
    std::cin >> c >> l >> r;
    if (c == '!') {
      sol.Change(l - 1, r);
    } else {
      std::cout << sol.Sum(l - 1, r) << '\n';
    }
  }
}
