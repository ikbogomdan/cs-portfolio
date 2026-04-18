#include <algorithm>
#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <vector>

class Graph {
  friend class Shell;

  std::vector<std::vector<int>> g_;
  int n_;
  std::set<std::pair<int, int>> ed_;
  std::set<std::pair<int, int>> net_;
  std::map<std::pair<int, int>, int> num_;

 public:
  Graph(int size, int m) : g_(size), n_(size) {
    for (int i = 0; i < m; i++) {
      int u;
      int v;
      std::cin >> u >> v;
      int tmp = std::min(u, v);
      int tmp2 = std::max(u, v);
      u = tmp;
      v = tmp2;
      if (u == v) {
        continue;
      }
      if (ed_.find({u, v}) == ed_.end()) {
        g_[u - 1].push_back(v - 1);
        g_[v - 1].push_back(u - 1);
        num_[{u - 1, v - 1}] = i + 1;
        ed_.insert({u, v});
      } else {
        net_.insert({u - 1, v - 1});
      }
    }
  }

  Graph() = default;
};

class Shell {
  const Graph& graph_;
  Graph g_invert_;
  std::vector<int> color_cnt_;
  std::vector<int> color_;
  int n_;
  std::vector<std::pair<int, int>> time_;

  std::vector<int> prev_;
  std::vector<int> neigh_vert_;
  std::vector<int> bridges_;
  std::vector<int> us_edge_;
  std::vector<int> brige_to_add_;
  std::map<std::pair<int, int>, int> num_;

  int cnt_ = 0;
  int timer_ = 0;

 public:
  Shell(const Graph& gr)
      : graph_(gr),
        color_cnt_(gr.n_, 0),
        color_(gr.n_, 0),
        n_(gr.n_),
        prev_(gr.n_),
        neigh_vert_(gr.n_, INT_MAX),
        us_edge_(gr.n_, 0),
        brige_to_add_(gr.n_),
        num_(gr.num_) {
    g_invert_.g_.resize(gr.n_);
  }

  void Dfs(int v) {
    us_edge_[v] = 1;
    brige_to_add_[v] = timer_++;
    neigh_vert_[v] = brige_to_add_[v];
    for (int i = 0; i < static_cast<int>(graph_.g_[v].size()); ++i) {
      int u = graph_.g_[v][i];
      if (us_edge_[u] != 0) {
        if (prev_[v] != u) {
          neigh_vert_[v] = std::min(brige_to_add_[u], neigh_vert_[v]);
        }
        continue;
      }
      prev_[u] = v;
      Dfs(u);
      neigh_vert_[v] = std::min(neigh_vert_[v], neigh_vert_[u]);
    }
    if ((prev_[v] != -1) && (neigh_vert_[v] == brige_to_add_[v]) &&
        (graph_.net_.find({std::min(v, prev_[v]), std::max(v, prev_[v])}) ==
         graph_.net_.end())) {
      bridges_.push_back(num_[{std::min(v, prev_[v]), std::max(v, prev_[v])}]);
      ++cnt_;
    }
  }

  void Mostiki() {
    for (int i = 0; i < n_; ++i) {
      if (us_edge_[i] == 0) {
        prev_[i] = -1;
        Dfs(i);
      }
    }
    std::cout << cnt_ << '\n';
    std::sort(bridges_.begin(), bridges_.end());
    for (int i = 0; i < cnt_; ++i) {
      std::cout << bridges_[i] << ' ';
    }
  }
};

int main() {
  int n;
  int m;
  std::cin >> n >> m;

  Graph g(n, m);

  Shell graph_solve(g);
  graph_solve.Mostiki();
}

