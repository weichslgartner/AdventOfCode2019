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
using  int128_t = __int128;
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
	for(T i {0}; i <iterations; ++i) {
		for(auto &line : lines) {
			auto tokenz = read_tokenz(line);
			//print_stack(tokenz);
			if(tokenz.back() == "stack") {
				//fmt::print("deal new stack {}\n", 0);
				stack = new_stack(stack);
			} else if(tokenz[1] == "with") {
				//fmt::print("deal with  increment {}\n", tokenz.back());
				stack = deal_increment(stack, std::stoi(tokenz.back()));
			} else if(tokenz.front() == "cut") {
				stack = cut_stack(stack, std::stoi(tokenz.back()));
				//fmt::print("cut stack {}\n", std::stoi(tokenz.back()));
			} else {
				fmt::print("parsing error {}\n",line);
			}
			//print_stack(stack);
		}

		print_stack(stack);
	}
	return stack;
}

int main() {
	constexpr auto filename { "build/input/input_22.txt" };
	auto lines = read_lines(filename);
	//auto res = play_game(10,lines);
	auto res = play_game(119315717514047LL, lines);
	auto pos = std::find(res.begin(), res.end(), 2019);
	fmt::print("Part 1: {}\n", pos - res.begin()); //to high 5702
	fmt::print("Part 2: {}\n", 0);
	return EXIT_SUCCESS;
}

