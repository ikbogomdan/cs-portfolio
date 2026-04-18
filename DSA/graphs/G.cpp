#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

const int cMax = 3000000;

class Graph {
  friend class Shell;
  std::vector<std::vector<std::pair<int, int>>> g_;
  std::vector<std::pair<int, std::pair<int, int>>> ed_;
  int n_;

 public:
  Graph(int size, int m) : g_(size), ed_(m), n_(size) {
    for (int i = 0; i < m; i++) {
      int u;
      int v;
      int c;
      std::cin >> u >> v >> c;
      ed_[i] = {std::min(u, v), {std::max(u, v), c}};
    }
    std::sort(ed_.begin(), ed_.end());
    for (int i = 0; i < static_cast<int>(ed_.size()); ++i) {
      int u = ed_[i].first;
      int v = ed_[i].second.first;
      int c = ed_[i].second.second;

      g_[u].push_back({v, c});
      g_[v].push_back({u, c});

      while ((i != static_cast<int>(ed_.size())) &&
             ((ed_[i].first == u) && (ed_[i].second.first == v))) {
        ++i;
      }
      --i;
    }
  }

  Graph() = default;
};

class Shell {
  const Graph& graph_;

  std::vector<int> dist_;
  int vert_num_;
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      queue_;

 public:
  Shell(const Graph& gr) : graph_(gr), dist_(gr.n_, cMax), vert_num_(gr.n_) {}

  void Dijkstra(int start) {
    queue_.push({0, start});
    dist_[start] = 0;

    while (!queue_.empty()) {
      auto t = queue_.top();
      queue_.pop();
      int u = t.second;
      int d = t.first;
      if (d > dist_[u]) {
        continue;
      }
      for (int i = 0; i < static_cast<int>(graph_.g_[u].size()); ++i) {
        int v = graph_.g_[u][i].first;
        int u_v_cost = graph_.g_[u][i].second;
        if (dist_[v] > dist_[u] + u_v_cost) {
          dist_[v] = dist_[u] + u_v_cost;
          queue_.push({dist_[v], v});
        }
      }
    }

    for (int i = 0; i < vert_num_; ++i) {
      if (dist_[i] == cMax) {
        std::cout << "2009000999" << ' ';
      } else {
        std::cout << dist_[i] << ' ';
      }
    }
  }
};

int main() {
  int repeat;
  std::cin >> repeat;
  for (int ii = 0; ii < repeat; ++ii) {
    int n;
    int m;
    std::cin >> n >> m;

    Graph graph(n, m);

    int start;
    std::cin >> start;

    Shell graph_shell(graph);
    graph_shell.Dijkstra(start);
  }
}

