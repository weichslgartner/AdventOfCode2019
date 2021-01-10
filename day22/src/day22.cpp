#include <algorithm>
#include <array>

#include <cassert>
#include <cctype>
#include <cstdint>
#include <fmt/core.h>
#include <fmt/format.h>
#include <cstddef>
#include <tuple>
#include <fstream>
#include <functional>
#include <deque>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <gtest/gtest.h>
using int128_t = __int128;

template<typename T>
auto mod(T a, T b)
{
  return (a >= static_cast<T>(0)) ? (a % b) : (b + a % b);
}

template<typename T>
T mod_inv(T a, T b) {
	T const b0 { b };
	T temp { };
	T quotient { };
	T x0 { 0LL };
	T x1 { 1LL };
	if (b == 1) {
		return 1;
	}
	while (a > 1) {
		quotient = a / b;
		temp = b;
		b = a % b; // remainder
		a = temp;
		temp = x0;
		x0 = x1 - quotient * x0;
		x1 = temp;
	}
	if (x1 < 0)
		x1 += b0;
	return x1;
}

std::vector<std::string> read_lines(std::string const &filename) {
	std::ifstream infile(filename);
	if (!infile.is_open()) {
		fmt::print(stderr, "can't open file {}", filename);
	}
	std::stringstream ss { };
	ss << infile.rdbuf();
	std::vector<std::string> lines { };
	std::string line;
	while (std::getline(ss, line)) {
		lines.push_back(line);
	}
	return lines;
}

std::vector<std::string> read_tokenz(std::string_view const line) {
	std::stringstream ss { };
	ss << line;
	std::vector<std::string> tokenz { };
	std::string token;
	while (std::getline(ss, token, ' ')) {
		tokenz.push_back(token);
	}
	return tokenz;
}

template<class T>
T new_stack(T &stack) {
	std::reverse(stack.begin(), stack.end());
	return stack;
}

template<class T>
T cut_stack(T &stack, int n) {
	T new_stack { };
	if (n > 0) {
		new_stack.insert(new_stack.end(), stack.begin() + n, stack.end());
		new_stack.insert(new_stack.end(), stack.begin(), stack.begin() + n);
	} else {
		new_stack.insert(new_stack.end(), stack.end() + n, stack.end());
		new_stack.insert(new_stack.end(), stack.begin(), stack.end() + n);
	}
	return new_stack;
}

template<class T>
T deal_increment(T &stack, int n) {
	T new_stack { };
	auto N = stack.size();
	new_stack.resize(N);
	auto i { 0 };
	while (not stack.empty()) {
		new_stack[i] = stack.front();
		stack.erase(stack.begin());
		i += n;
		i = i % (N);

	}
	return new_stack;
}

template<class T>
void print_stack(T &stack) {
	fmt::print("{}\n", fmt::join(stack, ","));
}

template<class T>
std::deque<T> play_game(T n_cards, std::vector<std::string> &lines, int iterations = 1) {
	std::deque<T> stack;
	for (T i { 0 }; i < n_cards; ++i) {
		stack.push_back(i);
	}
	print_stack(stack);
	for (T i { 0 }; i < iterations; ++i) {
		for (auto &line : lines) {
			auto tokenz = read_tokenz(line);
			//print_stack(tokenz);
			if (tokenz.back() == "stack") {
				//fmt::print("deal new stack {}\n", 0);
				stack = new_stack(stack);
			} else if (tokenz[1] == "with") {
				//fmt::print("deal with  increment {}\n", tokenz.back());
				stack = deal_increment(stack, std::stoi(tokenz.back()));
			} else if (tokenz.front() == "cut") {
				stack = cut_stack(stack, std::stoi(tokenz.back()));
				//fmt::print("cut stack {}\n", std::stoi(tokenz.back()));
			} else {
				fmt::print("parsing error {}\n", line);
			}
			//print_stack(stack);
		}

		print_stack(stack);
	}
	return stack;
}

template<typename T>
auto mod_power(T b, T e, T m) {
	if (m == static_cast<T>(1)) {
		return static_cast<T>(0);
	}
	T r = static_cast<T>(1);
	b = mod(b, m);
	while (e > 0) {
		if (e % 2 == 1) {
			r = mod((r * b) , m);

		}
		e = e >> static_cast<T>(1);
		b = mod((b * b) , m);
	}
	return r;
}

int128_t modular_power(int128_t base, int128_t exponent, int128_t n)
{
  assert(exponent >= 0);
  if (exponent == 0) {
    return (base == 0) ? 0 : 1;
  }

  int128_t bit = 1;
  int128_t power = mod(base, n);
  int128_t out = 1;
  while (bit <= exponent) {
    if (exponent & bit) {
      out = mod(out * power, n);
    }
    power = mod(power * power, n);
    bit <<= 1;
  }

  return out;
}
template<typename T>
T mod_inv2(T a, T b) {
	return modular_power(a, b-2, b);
}
template<typename T>
auto part2(T n_cards, T card, std::vector<std::string> &lines, T iterations = 1) {
	T a = 1;
	T b = 0;
	T la { }, lb { };
	for (auto &line : lines) {
		auto tokenz = read_tokenz(line);
		//print_stack(tokenz);
		if (tokenz.back() == "stack") {
			la = static_cast<T>(-1);
			lb = static_cast<T>(-1);
		} else if (tokenz[1] == "with") {
			la = static_cast<T>(std::stoi(tokenz.back()));
			lb = 0;
			//fmt::print("deal with  increment {}\n", tokenz.back());
		} else if (tokenz.front() == "cut") {
			//stack = cut_stack(stack,));
			la =static_cast<T>( 1);
			//a = a;
			lb = -1*static_cast<T>(std::stoi(tokenz.back()));
			//fmt::print("cut stack {}\n", std::stoi(tokenz.back()));
		} else {
			fmt::print("parsing error {}\n", line);
		}

		//print_stack(stack);
		a = mod((la * a) , n_cards);
		b = mod((la * b + lb) , n_cards);
		fmt::print("{} {} {} {}\n",la,lb, a,b);

	}

	auto Ma = modular_power(a, iterations, n_cards);
	fmt::print("{}\n",Ma);
	auto temp=  mod(b * (Ma - static_cast<T>(1)), n_cards);
	auto Mb = mod(temp  * mod_inv2(a - static_cast<T>(1), n_cards) , n_cards);
	fmt::print("{}\n",Mb);
	assert(mod(b * (Ma - static_cast<T>(1)),n_cards)== 68054971591981);
	return mod(((card - Mb) * mod_inv2(Ma, n_cards)) , n_cards);
}



int main(int argc, char **argv) {
	assert(modular_power(91554340096269,101741582076661,119315717514047)==77239560156966);
	constexpr auto filename { "build/input/input_22.txt" };
	constexpr auto  TESTING {false};
	auto lines = read_lines(filename);
	//auto res = play_game(10,lines);
	//auto res = play_game(119315717514047LL, lines);
	//auto pos = std::find(res.begin(), res.end(), 2019);
	//fmt::print("Part 1: {}\n", pos - res.begin()); //to high 5702
	int128_t n_cards = 119315717514047;
	int128_t iterations = 101741582076661;
	auto p2 = part2(n_cards, static_cast<int128_t>(2020), lines, iterations);
	fmt::print("Part 2: {}\n", p2);
	/*
	if (TESTING) {
		::testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
*/
	return EXIT_SUCCESS;
}

