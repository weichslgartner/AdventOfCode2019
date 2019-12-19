#include <algorithm>
#include <assert.h>
#include <cmath>
#include <deque>
#include <fmt/format.h>
#include <fmt/printf.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

class Node {
public:
  std::string m_name;
  int m_amount;
  std::vector<Node> m_preds{};
  Node() {
    m_name = "";
    m_amount = 0;
  }
  Node(std::string name, int amount) {
    m_name = name;
    m_amount = amount;
  }
};

using Pred_map = std::unordered_map<std::string, Node>;
using Suc_map = std::unordered_map<std::string, int>;

int calc_fuel(std::unordered_map<std::string, Node> tree) {
  std::deque<std::pair<std::string, int>> deq{};
  Node cur_node = tree["FUEL"];
  deq.push_back(std::make_pair(cur_node.m_name, cur_node.m_amount));
  int fuel{0};
  while (!deq.empty()) {
    auto [cur_name, needed] = deq.front();
    deq.pop_front();
    cur_node = tree[cur_name];
    if (cur_name.compare("ORE") == 0) {
      fuel += needed;
      continue;
    }

    auto temp = std::ceil(static_cast<float>(needed) /
                          static_cast<float>(cur_node.m_amount));
    auto factor = static_cast<int>(temp);
    bool already_in_deq{false};
    for (auto pred : cur_node.m_preds) {

      for (auto &element : deq) {
        if (element.first.compare(pred.m_name) == 0) {
          element.second += factor * pred.m_amount;
          already_in_deq = true;
          break;
        }
      }
      if (!already_in_deq) {
        deq.push_back(std::make_pair(pred.m_name, factor * pred.m_amount));
      }
    }
  }
  return fuel;
}

std::ostream &operator<<(std::ostream &out, Node const &node) {
  out << fmt::format("{}:{} n_pred{}", node.m_name, node.m_amount,
                     node.m_preds.size());
  return out;
}

std::vector<Node> parse_Nodes(std::string token) {
  std::regex reg_token{R"((\d+)\s([A-Z]+))"};
  std::smatch m{};
  std::string str = token;
  std::vector<Node> nodez{};
  while (regex_search(str, m, reg_token)) {
    Node node{m[2], std::stoi(m[1])};
    nodez.push_back(node);
    str = m.suffix().str();
  }
  return nodez;
}

std::pair<Pred_map, Suc_map> string2tree(std::string input_file) {
  std::ifstream infile{input_file};
  std::stringstream ss{};
  ss << infile.rdbuf();
  Pred_map tree;
  Suc_map suc_m;
  //  // matches one token
  // matches the line and splits it in two tokens
  std::regex reg_line{R"(([\d\s\w,]+)\s=>\s([\d\s\w]+))"};
  std::smatch m{};
  std::string buffer;
  while (std::getline(ss, buffer)) {
    std::string str = buffer;

    while (regex_search(str, m, reg_line)) {
      // std::cout << "-" << m[1] << "-" << m[2] << "\n";
      auto rhs = parse_Nodes(m[2]);
      auto lhs = parse_Nodes(m[1]);
      rhs[0].m_preds = lhs;
      tree.insert({rhs[0].m_name, rhs[0]});
      for (auto l : lhs) {
        auto iter = suc_m.find(l.m_name);
        if (iter == suc_m.end()) {
          suc_m[l.m_name] = 1;
        } else {
          iter->second++;
        }
      }
      str = m.suffix().str(); // Proceed to the next match
    }
  }
  return std::make_pair(tree, suc_m);
}

int calc_ore(Pred_map &pmap, Suc_map &smap) {
  auto root = pmap["FUEL"];
  std::unordered_map<std::string, int> equation{};
  for (auto node : root.m_preds) {
    equation.insert({node.m_name, node.m_amount});
  }
  std::vector<std::string> erase_list{};
  for (auto [key, value] : equation) {
    fmt::print("{} {} ", key, value);
  }
  fmt::print("\n====\n");
  while (true) {
    std::vector<std::pair<std::string, int>> keys{};
    for (auto [key, value] : equation) {
      keys.push_back(std::make_pair(key, value));
    }

    for (auto [key, value] : keys) {
      if (smap[key] <= 1) {
        std::string name = key;
        if (name.compare("ORE") == 0) {
          continue;
        }
        auto cur_amount = equation[key];
        // equation.erase(key);
        erase_list.push_back(key);
        if (pmap.find(name) == pmap.end()) {
          continue;
        }
        assert(pmap[name].m_preds.size() > 0);
        auto factor{1};
        for (auto new_el : pmap[name].m_preds) {
          auto temp = std::ceil(static_cast<float>(cur_amount) /
                                static_cast<float>(pmap[name].m_amount));
          factor = static_cast<int>(temp);
          bool already_in{false};
          auto found = equation.find(new_el.m_name);

          if (found != equation.end()) {
            fmt::print("before {} {} ", new_el.m_name, found->second);
            found->second = found->second + (factor * new_el.m_amount);
            fmt::print("after {} \n", found->second);
          } else {
            fmt::print("enter {} {}\n", new_el.m_name,
                       factor * new_el.m_amount);
            equation.insert({new_el.m_name, factor * new_el.m_amount});
          }
          fmt::print("{}({}) {} ", factor * new_el.m_amount, factor,
                     new_el.m_name);
          smap[new_el.m_name]--;
        }
        fmt::print("produce {} {} surplus {}\n", factor * pmap[name].m_amount,
                   name, factor * pmap[name].m_amount - cur_amount);
      }
    }

    for (auto e : erase_list) {
      equation.erase(e);
    }
    for (auto [key, value] : equation) {
      fmt::print("{} {} ", key, value);
    }
    fmt::print("\n====\n");
    erase_list.clear();
    if (equation.size() == 1 && equation.find("ORE") != equation.end()) {
      for (auto [key, value] : equation) {
        std::cout << key << " " << value << "\n";
      }
      break;
    }

    std::cout << "======\n";
  }
  return equation["ORE"];
}

int main() {
  // tests();
  std::string input_file{"./input/input_14.txt"};

  auto [tree, suc_m] = string2tree(input_file);
  for (auto [key, value] : tree) {
    std::cout << key << " " << value << "\n";
  }
  std::cout << calc_ore(tree, suc_m);
  return 0;
}
