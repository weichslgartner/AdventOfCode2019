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

std::vector<int> replicate_vector(std::vector<int> input, int replications) {
  std::vector<int> output{};
  output.reserve(input.size() * replications);
  auto pos = output.begin();
  for (int i = 0; i < replications; ++i) {
    output.insert(output.end(), input.begin(), input.end());
  }
  return output;
}

int get_number(std::vector<int> &input, int offset, int length) {
  assert(input.size() >= length);
  int result{0};
  for (int i = 0; i < length; ++i) {
    result += input[offset + i] * std::pow(10, length - 1 - i);
  }
  return result;
}

int get_offset(std::vector<int> &input) { return get_number(input, 0, 7); }

constexpr int last_digit(int numb) {
  if (numb < 0) {
    numb *= -1;
  }
  return numb % 10;
}

std::vector<std::vector<int>> generate_pattern(std::size_t size) {
  std::size_t length{size};
  std::vector<int> result_pattern{};
  result_pattern.reserve(length);
  std::vector<std::vector<int>> pattern;
  pattern.reserve(size);
  std::array<int, 4> base_pattern{0, 1, 0, -1};
  for (int line = 0; line < length; ++line) {
    std::vector<int> pattern_line{};
    pattern_line.reserve(length + 1);
    for (int i = 0; i < length + 1; i++) {
      for (int j = 0; j < line + 1; ++j) {
        if (pattern_line.size() < length + 1)
          pattern_line.push_back(base_pattern[i % base_pattern.size()]);
        else
          break;
      }
    }
    pattern_line.erase(pattern_line.begin());
    pattern.push_back(pattern_line);
  }
  return pattern;
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

int do_fft(std::vector<int> &input, int iterations) {
  // print_vec(input);
  const auto pattern = generate_pattern(input.size());
  for (int i = 0; i < iterations; ++i) {
    std::vector<int> line_result;
    line_result.reserve(input.size());
    for (int line = 0; line < input.size(); ++line) {
      const auto line_pattern = pattern[line];
      auto result = 0;
      for (int i = 0; i < input.size(); ++i) {
        result += input[i] * line_pattern[i];
      }
      line_result.push_back(last_digit(result));
    }
    input = line_result;
  }
  return get_number(input, 0, 8);
}

int part_2(std::vector<int> &input, int iterations, int offset) {
  for (int i = 0; i < iterations; ++i) {
    int partial_sum{0};
    for (int pos = input.size() - 1; pos >= offset; --pos) {
      input[pos] = last_digit(partial_sum + input[pos]);
      partial_sum = input[pos];
    }
  }
  return get_number(input, offset, 8);
}

void tests() {
  int iterations{100};
  auto test0 = parse_input("12345678");
  assert(do_fft(test0, 4) == 1029498);
  auto test1 = parse_input("80871224585914546619083218645595");
  assert(do_fft(test1, iterations) == 24176176);
  auto test2 = parse_input("19617804207202209144916044189917");
  assert(do_fft(test2, iterations) == 73745418);
  auto test3 = parse_input("69317163492948606335995924319873");
  assert(do_fft(test3, iterations) == 52432133);
  auto test4 = parse_input("03036732577212944063491565474664");
  test4 = replicate_vector(test4, 10000);
  assert(part_2(test4, iterations, get_offset(test4)) == 84462026);
  auto test5 = parse_input("02935109699940807407585447034323");
  test5 = replicate_vector(test5, 10000);
  assert(part_2(test5, iterations, get_offset(test5)) == 78725270);
  auto test6 = parse_input("03081770884921959731165446850517");
  test6 = replicate_vector(test6, 10000);
  assert(part_2(test6, iterations, get_offset(test6)) == 53553731);
}

int main() {
  tests();
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
  int iterations{100};
  auto input_repl = replicate_vector(input, 10000);
  std::cout << "Part 1: " << do_fft(input, iterations) << "\n";
  std::cout << "Part 2: "
            << part_2(input_repl, iterations, get_offset(input_repl));
  return 0;
}
