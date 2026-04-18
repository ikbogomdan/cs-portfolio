#include <algorithm>
#include <climits>
#include <iostream>
#include <set>
#include <vector>

const int cMax = 300000;
const int cEmpty = 100000;

class Graph {
  friend class Shell;
  std::vector<std::vector<int>> g_;
  int vert_num_;

 public:
  Graph(int size) : g_(size, std::vector<int>(size, cMax)), vert_num_(size) {
    for (int i = 0; i < vert_num_; i++) {
      for (int j = 0; j < vert_num_; j++) {
        int cost;
        std::cin >> cost;
        if (cost != cEmpty) {
          g_[i][j] = cost;
        } else {
          g_[i][j] = cMax;
        }
      }
    }
  }

  Graph() = default;
};

class Shell {
  const Graph& graph_;
  std::vector<int> dist_;
  std::vector<int> parent_;
  std::vector<int> neg_;
  int vert_num_;
  std::vector<std::vector<int>> dp_;

 public:
  Shell(const Graph& gr)
      : graph_(gr),
        parent_(gr.vert_num_, -1),
        vert_num_(gr.vert_num_),
        dp_(gr.vert_num_, std::vector<int>(gr.vert_num_ + 1, cMax)) {}

  void BellmanFord() {
    for (int i = 0; i < vert_num_; ++i) {
      dp_[i][0] = 0;
    }
    for (int i = 1; i < vert_num_ + 1; ++i) {
      for (int j = 0; j < vert_num_; ++j) {
        dp_[j][i] = dp_[j][i - 1];
        for (int k = 0; k < vert_num_; ++k) {
          if ((graph_.g_[k][j] != cMax) && (dp_[k][i - 1] != cMax)) {
            if (dp_[j][i] > dp_[k][i - 1] + graph_.g_[k][j]) {
              dp_[j][i] = dp_[k][i - 1] + graph_.g_[k][j];
              parent_[j] = k;
            }
          }
        }
      }
    }
  }

  int CheckNegative() {
    int pos_start = -1;

    for (int i = 0; i < vert_num_; ++i) {
      if (dp_[i][vert_num_ - 1] > dp_[i][vert_num_]) {
        pos_start = i;
        break;
      }
    }

    return pos_start;
  }

  void FindNegative() {
    int last = CheckNegative();

    if (last == -1) {
      std::cout << "NO" << '\n';
      return;
    }

    for (int i = 0; i < vert_num_; ++i) {
      last = parent_[last];
    }

    int y = last;
    do {
      neg_.push_back(y + 1);
      y = parent_[y];
    } while (y != last);

    neg_.push_back(last + 1);

    reverse(neg_.begin(), neg_.end());

    std::cout << "YES" << '\n' << static_cast<int>(neg_.size()) << '\n';
    for (int i = 0; i < static_cast<int>(neg_.size()); ++i) {
      std::cout << neg_[i] << ' ';
    }
  }
};

int main() {
  int n;
  std::cin >> n;
  Graph graph(n);
  Shell graph_shell(graph);
  graph_shell.BellmanFord();
  graph_shell.FindNegative();
}

