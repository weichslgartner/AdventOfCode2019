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
  long long int m_amount;
  std::vector<Node> m_preds{};
  Node() {
    m_name = "";
    m_amount = 0;
  }
  Node(std::string name, long long int amount) {
    m_name = name;
    m_amount = amount;
  }
};

using Pred_map = std::unordered_map<std::string, Node>;
using Suc_map = std::unordered_map<std::string, long long int>;

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
    Node node{m[2], static_cast<long long int>(std::stoi(m[1]))};
    nodez.push_back(node);
    str = m.suffix().str();
  }
  return nodez;
}

Pred_map string2tree(std::string input_file) {
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
      auto rhs = parse_Nodes(m[2]);
      auto lhs = parse_Nodes(m[1]);
      rhs[0].m_preds = lhs;
      tree.insert({rhs[0].m_name, rhs[0]});

      str = m.suffix().str(); // Proceed to the next match
    }
  }
  return tree;
}

long long int calc_ore(Pred_map &pmap) {
  auto root = pmap["FUEL"];
  std::unordered_map<std::string, long long int> equation{};
  for (auto node : root.m_preds) {
    equation.insert({node.m_name, node.m_amount * root.m_amount});
  }
  while (true) {
    std::vector<std::pair<std::string, long long int>> keys{};
    for (auto [key, value] : equation) {
      keys.push_back(std::make_pair(key, value));
    }
    int elements{0};
    for (auto [key, value] : keys) {
      if (key.compare("ORE") != 0 && value > 0) {
        auto cur_amount = equation[key];
        if (pmap.find(key) == pmap.end()) {
          continue;
        }
        assert(pmap[key].m_preds.size() > 0);
        auto temp = std::ceil(static_cast<double>(cur_amount) /
                              static_cast<double>(pmap[key].m_amount));
        auto factor = static_cast<long long int>(temp);
        equation[key] -= factor * pmap[key].m_amount;
        for (auto new_el : pmap[key].m_preds) {
          bool already_in{false};
          auto found = equation.find(new_el.m_name);
          if (found != equation.end()) {
            found->second = found->second + (factor * new_el.m_amount);
          } else {
            equation.insert({new_el.m_name, factor * new_el.m_amount});
          }
        }
        elements++;
      }
    }
    if (elements == 0)
      break;
  }
  return equation["ORE"];
}

long long int part2(Pred_map &pmap, long long goal) {
  auto &fuel = pmap["FUEL"].m_amount;
  long long int left{0LL};
  long long int right{100000000};
  long long int result{};
  while (abs(left - right) > 1) {
    auto mid = (left + right) / 2;
    fuel = mid;
    result = calc_ore(pmap);
    if (result < goal) {
      left = mid;
    } else {
      right = mid;
    }
  }
  return left;
}

int main() {
  // tests();
  std::string input_file{"./input/input_14.txt"};
  auto tree = string2tree(input_file);
  std::cout << "Part 1: " << calc_ore(tree) << "\n";
  std::cout << "Part 2: " << part2(tree, 1000000000000);
  return 0;
}
