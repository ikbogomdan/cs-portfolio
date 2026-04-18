#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

class Graph {
  friend class Shell;

  std::vector<std::vector<int>> g_;
  std::set<std::pair<int, int>> ed_;
  int n_;

 public:
  Graph(int size, int m) : g_(size), n_(size) {
    for (int i = 0; i < m; ++i) {
      int u;
      int v;
      std::cin >> u >> v;
      if (u == v) {
        continue;
      }
      if (ed_.find({u, v}) == ed_.end()) {
        g_[u - 1].push_back(v - 1);
        ed_.insert({u, v});
      }
    }
  }

  Graph() = default;
};

class Shell {
  const Graph& graph_;
  std::set<std::pair<int, int>> ed_;
  std::vector<std::pair<int, int>> time_;
  std::vector<int> color_cnt_;
  std::vector<int> color_;
  Graph g_invert_;
  int comp_count_ = 1;
  int timer_ = 0;
  int n_;

 public:
  Shell(const Graph& gr)
      : graph_(gr), color_cnt_(gr.n_, 0), color_(gr.n_, 0), n_(gr.n_) {
    g_invert_.g_.resize(gr.n_);

    for (int i = 0; i < n_; ++i) {
      for (int j = 0; j < static_cast<int>(gr.g_[i].size()); ++j) {
        g_invert_.g_[gr.g_[i][j]].push_back(i);
      }
    }
  }

  void DfsOnInvert(int v) {
    color_[v] = 1;
    color_cnt_[v] = comp_count_;
    for (int i = 0; i < static_cast<int>(g_invert_.g_[v].size()); ++i) {
      int u = g_invert_.g_[v][i];
      if (color_[u] != 0) {
        continue;
      }
      color_cnt_[u] = comp_count_;
      DfsOnInvert(u);
    }
    color_[v] = 2;
  }

  void Dfs(int v) {
    color_[v] = 1;
    for (int i = 0; i < static_cast<int>(graph_.g_[v].size()); ++i) {
      int u = graph_.g_[v][i];
      if (color_[u] != 0) {
        continue;
      }
      Dfs(u);
    }
    color_[v] = 2;
    ++timer_;
    time_.push_back({-timer_, v});
  }

  void StrongComp() {
    for (int i = 0; i < n_; ++i) {
      if (color_[i] == 0) {
        Dfs(i);
      }
    }

    std::sort(time_.begin(), time_.end());

    for (int i = 0; i < n_; ++i) {
      color_[i] = 0;
    }

    for (int i = 0; i < n_; ++i) {
      if (color_[time_[i].second] == 0) {
        DfsOnInvert(time_[i].second);
        ++comp_count_;
      }
    }

    std::cout << comp_count_ - 1 << '\n';
    for (int i = 0; i < n_; ++i) {
      std::cout << color_cnt_[i] << ' ';
    }
  }
};

int main() {
  int n;
  int m;
  std::cin >> n >> m;
  Graph g(n, m);

  Shell graph_solve(g);

  graph_solve.StrongComp();
}

