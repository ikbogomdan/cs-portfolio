#include <climits>
#include <iostream>
#include <queue>
#include <vector>

class Graph {
  friend class Shell;
  std::vector<std::vector<std::pair<int, int>>> g_;
  int vert_num_;

 public:
  Graph(int size, int ed_num) : g_(size), vert_num_(size) {
    for (int i = 0; i < ed_num; ++i) {
      int u;
      int v;
      int cost;
      std::cin >> u >> v >> cost;
      --u;
      --v;
      if (u != v) {
        g_[u].push_back({v, cost});
        g_[v].push_back({u, cost});
      }
    }
  }
};

void Check(int n) {
  if (n == 1) {
    std::cout << "0\n";
    exit(0);
  }
}

class Shell {
  const Graph& graph_;
  std::vector<bool> used_;
  int vert_num_;
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      queue_;

 public:
  Shell(const Graph& gr)
      : graph_(gr), used_(gr.vert_num_, false), vert_num_(gr.vert_num_) {}

  void Mst() {
    used_[0] = true;
    for (const std::pair<int, int>& edge : graph_.g_[0]) {
      queue_.push({edge.second, edge.first});
    }

    long long ans = 0;
    int edges_used = 0;

    while (!queue_.empty() && edges_used < vert_num_ - 1) {
      int cost = queue_.top().first;
      int u = queue_.top().second;
      queue_.pop();

      if (used_[u]) {
        continue;
      }

      used_[u] = true;
      ans += cost;
      edges_used++;

      for (const std::pair<int, int>& edge : graph_.g_[u]) {
        int v = edge.first;
        int c = edge.second;
        if (!used_[v]) {
          queue_.push({c, v});
        }
      }
    }

    std::cout << ans << "\n";
  }
};

int main() {
  int n;
  int m;
  std::cin >> n >> m;

  Check(n);

  Graph graph(n, m);
  Shell graph_shell(graph);
  graph_shell.Mst();
}

