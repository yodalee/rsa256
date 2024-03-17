#include "VDut/VDut.h"
#include "verilated_fst_c.h"
#include <functional>
#include <glog/logging.h>
#include <iostream>
#include <memory>
#include <queue>
#include <random>
#include <type_traits>
#include <utility>
using namespace std;

struct Channel {
  unsigned id;
};

struct If : virtual public Channel {
  enum CombinationDependency : unsigned { eSrc, eDst, eNone } dep = eNone;
};

struct Clock {
  struct Node {
    vector<unsigned> neighbors;
    function<void(void)> comb;
  };
  vector<Node> nodes;

  void AddIfDependency(function<void(void)> f, const vector<If *> &ifs) {
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

  vector<function<void(void)>> ff;
  vector<function<void(void)>> comb;

  void Initialize() {
    // Modified from ChatGPT's results
    vector<unsigned> in_degree(nodes.size(), 0);

    // Calculate in-degrees for each node
    for (const auto &node : nodes) {
      for (const auto &neighbor : node.neighbors) {
        in_degree[neighbor]++;
      }
    }
    // Initialize queue with nodes having in-degree 0
    queue<unsigned int> q;
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
  virtual pair<Data, bool> peek() = 0;
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

  pair<Data, bool> peek() {
    bool tmp = is_first;
    is_first = false;
    return pair<Data, bool>(r, tmp);
  }

  bool is_writeable() { return not valid_r; }

  void write(Data d) {
    w = d;
    valid_w = true;
  }
};

// User code
bernoulli_distribution dist(0.5);
default_random_engine reng;

// Driver and Monitor is reused
struct Driver {
  ValidReadyOutIf<unsigned> *o;
  unsigned counter = 0;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {o});
  }

  void always_comb() {
    cout << "Driver" << endl;
    if (o->is_writeable() and dist(reng)) {
      o->write(counter);
      cout << "Driver write" << endl;
      counter++;
    }
  }
};

struct Monitor {
  ValidReadyInIf<unsigned> *i;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i});
  }

  void always_comb() {
    cout << "Monitor" << endl;
    if (i->is_readable() and dist(reng)) {
      cout << "Read " << i->read() << endl;
    }
  }
};

// V1: c-model
struct Dut {
  ValidReadyInIf<unsigned> *i;
  ValidReadyOutIf<unsigned> *o;

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i, o});
  }

  void always_comb() {
    if (o->is_writeable() and i->is_readable()) {
      o->write(i->read());
    }
  }
};

// V2: Verilated
class DutWrapper {
  VerilatedContext contextp;
  VerilatedFstC tfp;
  VDut vdut;
  unsigned counter = 0;

public:
  ValidReadyInIf<unsigned> *i;
  ValidReadyOutIf<unsigned> *o;

  DutWrapper() {
    Verilated::traceEverOn(true);
    vdut.trace(&tfp, 99);
    tfp.open("Dut.fst");

    vdut.clk = 0;
    vdut.rst_n = 1;
    vdut.eval();
    tfp.dump(counter++);

    vdut.rst_n = 0;
    vdut.eval();
    tfp.dump(counter++);

    vdut.rst_n = 1;
    vdut.eval();
    tfp.dump(counter++);
  }

  ~DutWrapper() { tfp.close(); }

  void ClockedBy(Clock &clk) {
    clk.AddIfDependency([this]() { always_comb(); }, {i, o});
  }

  void always_comb() {
    cout << "Dut" << endl;
    vdut.clk = 1;
    vdut.eval();

    vdut.i_valid = i->is_readable();
    if (i->is_readable()) {
      auto [i_data, i_first] = i->peek();
      if (i_first) {
        vdut.i_data = i_data;
      }
    }

    vdut.o_ready = o->is_writeable();

    vdut.eval();
    tfp.dump(counter++);

    if (vdut.i_valid and vdut.i_ready) {
      (void)i->read();
    }
    if (vdut.o_valid and vdut.o_ready) {
      o->write(vdut.o_data);
    }

    vdut.clk = 0;
    vdut.eval();
    tfp.dump(counter++);
  }
};

int main() {
  Clock clock;
  Driver driver;
  DutWrapper dut;
  Monitor monitor;
  ValidReady<unsigned, false> ch1;
  ValidReady<unsigned, false> ch2;
  driver.o = &ch1;
  dut.i = &ch1;
  dut.o = &ch2;
  monitor.i = &ch2;

  ch1.ClockedBy(clock);
  ch2.ClockedBy(clock);
  monitor.ClockedBy(clock);
  dut.ClockedBy(clock);
  driver.ClockedBy(clock);

  clock.Initialize();
  for (unsigned i = 0; i < 100; ++i) {
    cout << "cycle " << i << endl;
    clock.Comb();
    clock.FF();
  }
  return 0;
}
