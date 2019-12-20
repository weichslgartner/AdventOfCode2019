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

int last_digit(int numb) {
  if (numb < 0) {
    numb *= -1;
  }
  return numb % 10;
}

std::vector<int> generate_pattern(std::vector<int> &input) {
  unsigned int length{input.size()};
  std::vector<int> pattern{};
  std::vector<int> result_pattern{};
  result_pattern.reserve(length);
  pattern.reserve(length + 1);
  std::array<int, 4> base_pattern{0, 1, 0, -1};
  for (int line = 0; line < length; ++line) {
    pattern.clear();
    for (int i = 0; i < length + 1; i++) {
      for (int j = 0; j < line + 1; ++j) {
        if (pattern.size() < length + 1)
          pattern.push_back(base_pattern[i % base_pattern.size()]);
      }
    }
    pattern.erase(pattern.begin());
    for (auto val : pattern) {
      //  std::cout << val;
    }
    //  std::cout << "=";
    auto result = 0;
    for (int i = 0; i < length; ++i) {
      result += input[i] * pattern[i];
    }
    // std::cout << last_digit(result) << "\n";
    result_pattern.push_back(last_digit(result));
  }
  return result_pattern;
}

std::vector<int> parse_input(const std::string in_str) {
  std::vector<int> input_int{};
  for (auto const c : in_str) {
    input_int.push_back(c - '0');
  }
  return input_int;
}

void print_vec(std::vector<int> &vec, int digits) {
  int i{0};
  for (const auto &val : vec) {
    std::cout << val;
    i++;
    if (i >= digits) {
      break;
    }
  }
  std::cout << "\n";
}

void do_fft(std::vector<int, std::allocator<int>> input, int iterations) {
  // print_vec(input);
  for (int i = 0; i < iterations; ++i) {
    input = generate_pattern(input);
  }
  print_vec(input, 8);
}

int main() {
  // tests();
  auto input = parse_input(
      "597726982086712636082407645718608667401211646927131970431728764186144116"
      "712045690684383716941980332418542932775055475210822271277680003968758255"
      "885149318164696360736690865285798465681679842384688474243106928093565882"
      "831949383122470067707138723914495236166007094763373814081550579947176713"
      "104871166073217314721930541483833518314561938840468991137273013892974335"
      "539565528883085678973336571383537701910976769865164933047312390369595919"
      "220093710793930263326495585368889023035547973606911836816256044392500880"
      "624810525100161574728472894674105610256686375274084066153169400500604742"
      "608020004373562799103356244763303754853513732984915793647320295236641089"
      "87");
  int iterations = 100;
  do_fft(input, iterations);
  return 0;
}
