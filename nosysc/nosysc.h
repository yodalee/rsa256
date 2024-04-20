#pragma once

#include <functional>
#include <glog/logging.h>
#include <queue>
#include <vector>

namespace nosysc {

struct Channel {
  unsigned id;
};

struct If : virtual public Channel {
  enum CombinationDependency : unsigned { eSrc, eDst, eNone } dep = eNone;
};

struct Clock {
  struct Node {
    ::std::vector<unsigned> neighbors;
    ::std::function<void(void)> comb;
  };
  ::std::vector<Node> nodes;

  void AddIfDependency(::std::function<void(void)> f,
                       const ::std::vector<If *> &ifs) {
    unsigned node = nodes.size();
    nodes.push_back({{}, f});
    for (auto i : ifs) {
      if (i->dep == If::eDst) {
        nodes[i->id].neighbors.push_back(node);
      } else if (i->dep == If::eSrc) {
        nodes[node].neighbors.push_back(i->id);
      }
    }
  }

  ::std::vector<::std::function<void(void)>> ff;
  ::std::vector<::std::function<void(void)>> comb;

  void Initialize() {
    // Modified from ChatGPT's results
    ::std::vector<unsigned> in_degree(nodes.size(), 0);

    // Calculate in-degrees for each node
    for (const auto &node : nodes) {
      for (const auto &neighbor : node.neighbors) {
        in_degree[neighbor]++;
      }
    }
    // Initialize queue with nodes having in-degree 0
    ::std::queue<unsigned int> q;
    for (unsigned int i = 0; i < in_degree.size(); ++i) {
      if (in_degree[i] == 0) {
        q.push(i);
      }
    }
    unsigned num_visited = 0;
    while (not q.empty()) {
      unsigned int node = q.front();
      q.pop();
      if (auto &f = nodes[node].comb; bool(f)) {
        comb.push_back(f);
      }
      ++num_visited;

      // Update in-degrees for neighbors
      for (const auto &neighbor : nodes[node].neighbors) {
        if (--in_degree[neighbor] == 0) {
          q.push(neighbor);
        }
      }
    }
    // Check for cycles
    if (num_visited != nodes.size()) {
      LOG(FATAL) << "The graph has a cycle. Topological sort not possible.";
    }
    nodes.clear();
  }

  void FF() {
    for (auto x : ff) {
      x();
    }
  }
  void Comb() {
    for (auto x : comb) {
      x();
    }
  }
};

// basic channels and its interfaces
template <typename Data> struct ValidReadyInIf : public If {
  virtual bool is_readable() = 0;
  virtual Data read() = 0;
  virtual ::std::pair<Data, bool> peek() = 0;
};

template <typename Data> struct ValidReadyOutIf : public If {
  virtual bool is_writeable() = 0;
  virtual void write(Data d) = 0;
};

template <typename Data, bool is_slow = true>
class ValidReady : public ValidReadyOutIf<Data>, public ValidReadyInIf<Data> {
  Data r, w;
  bool valid_r, valid_w, is_first;

public:
  ValidReady() {
    valid_r = false;
    valid_w = false;
  }

  void ClockedBy(Clock &clk) {
    Channel::id = clk.nodes.size();
    clk.nodes.resize(clk.nodes.size() + 1);
    if constexpr (not is_slow) {
      ValidReadyInIf<Data>::dep = If::eSrc;
      ValidReadyOutIf<Data>::dep = If::eDst;
    }
    clk.ff.push_back([this]() { always_ff(); });
  }

  void always_ff() {
    if (not valid_r and valid_w) {
      r = w;
      is_first = true;
      valid_r = true;
      valid_w = false;
    }
  }

  bool is_readable() { return valid_r; }

  Data read() {
    valid_w = 0;
    if (not is_slow) {
      valid_r = 0;
    }
    return r;
  }

  ::std::pair<Data, bool> peek() {
    bool tmp = is_first;
    is_first = false;
    return {r, tmp};
  }

  bool is_writeable() { return not valid_r; }

  void write(Data d) {
    w = d;
    valid_w = true;
  }
};

} // namespace nosysc