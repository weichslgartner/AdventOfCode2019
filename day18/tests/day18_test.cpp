//#define TESTING
#include "../src/day18.cpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

using testing::ElementsAre;
using testing::UnorderedElementsAre;

//GTEST_TEST(day18, input_18_1) {
//	using testing::Pair;
//	constexpr auto filename { "../day18/input/input_18_1.txt" };
//	std::unordered_set<char> keys_to_find { };
//	std::unordered_set<char> all_locks { };
//	std::unordered_map<char, Point> POI_map { };
//	std::unordered_map<Point, char> point_map { };
//	auto start = parse_input(filename, point_map, POI_map, keys_to_find, all_locks);
//	Point p { 15, 1 };
//	GTEST_ASSERT_EQ(start, p);
//	ASSERT_THAT(keys_to_find, UnorderedElementsAre('a', 'f', 'e', 'b', 'c', 'd'));
//	ASSERT_THAT(all_locks, UnorderedElementsAre('D', 'E', 'C', 'A', 'B'));
//	ASSERT_THAT(POI_map,
//			UnorderedElementsAre(Pair('f', Point { 1, 1 }), Pair('D', Point { 3, 1 }), Pair('E', Point { 5, 1 }), Pair('e', Point { 7, 1 }),
//					Pair('C', Point { 9, 1 }), Pair('b', Point { 11, 1 }), Pair('A', Point { 13, 1 }), Pair('@', Point { 15, 1 }), Pair('a', Point {
//							17, 1 }), Pair('B', Point { 19, 1 }), Pair('c', Point { 21, 1 }), Pair('d', Point { 1, 3 })));
//	auto const reach_map = create_reach_map(point_map, POI_map);
//	print_reach_map(reach_map,point_map);
//	auto part1 = find_min_steps(point_map, keys_to_find, reach_map, POI_map, start);
//	GTEST_ASSERT_EQ(part1, 86);
//}
//
//
//GTEST_TEST(day18, input_18_2) {
//	using testing::Pair;
//	constexpr auto filename { "../day18/input/input_18_2.txt" };
//	std::unordered_set<char> keys_to_find { };
//	std::unordered_set<char> all_locks { };
//	std::unordered_map<char, Point> POI_map { };
//	std::unordered_map<Point, char> point_map { };
//	auto start = parse_input(filename, point_map, POI_map, keys_to_find, all_locks);
//	Point p { 6, 3 };
//	GTEST_ASSERT_EQ(start, p);
//	//ASSERT_THAT(keys_to_find, UnorderedElementsAre('a', 'f', 'e', 'b', 'c', 'd'));
//	//ASSERT_THAT(all_locks, UnorderedElementsAre('D', 'E', 'C', 'A', 'B'));
//	/*
//	ASSERT_THAT(POI_map,
//			UnorderedElementsAre(Pair('f', Point { 1, 1 }), Pair('D', Point { 3, 1 }), Pair('E', Point { 5, 1 }), Pair('e', Point { 7, 1 }),
//					Pair('C', Point { 9, 1 }), Pair('b', Point { 11, 1 }), Pair('A', Point { 13, 1 }), Pair('@', Point { 15, 1 }), Pair('a', Point {
//							17, 1 }), Pair('B', Point { 19, 1 }), Pair('c', Point { 21, 1 }), Pair('d', Point { 1, 3 })));
//	*/
//	auto const reach_map = create_reach_map(point_map, POI_map);
//	print_reach_map(reach_map,point_map);
//	auto part1 = find_min_steps(point_map, keys_to_find, reach_map, POI_map, start);
//	GTEST_ASSERT_EQ(part1, 132);
//}


GTEST_TEST(day18, input_18_5) {
	using testing::Pair;
	constexpr auto filename { "../day18/input/input_18_4.txt" };
	std::unordered_set<char> keys_to_find { };
	std::unordered_set<char> all_locks { };
	std::unordered_map<char, Point> POI_map { };
	std::unordered_map<Point, char> point_map { };
	auto start = parse_input(filename, point_map, POI_map, keys_to_find, all_locks);
	Point p { 1, 1 };
	GTEST_ASSERT_EQ(start, p);
	//ASSERT_THAT(keys_to_find, UnorderedElementsAre('a', 'f', 'e', 'b', 'c', 'd'));
	//ASSERT_THAT(all_locks, UnorderedElementsAre('D', 'E', 'C', 'A', 'B'));
	/*
	ASSERT_THAT(POI_map,
			UnorderedElementsAre(Pair('f', Point { 1, 1 }), Pair('D', Point { 3, 1 }), Pair('E', Point { 5, 1 }), Pair('e', Point { 7, 1 }),
					Pair('C', Point { 9, 1 }), Pair('b', Point { 11, 1 }), Pair('A', Point { 13, 1 }), Pair('@', Point { 15, 1 }), Pair('a', Point {
							17, 1 }), Pair('B', Point { 19, 1 }), Pair('c', Point { 21, 1 }), Pair('d', Point { 1, 3 })));
	*/
	auto const reach_map = create_reach_map(point_map, POI_map);
	print_reach_map(reach_map,point_map);
	auto part1 = find_min_steps(point_map, keys_to_find, reach_map, POI_map, start);
	GTEST_ASSERT_EQ(part1, 132);
}


GTEST_TEST(day18, input_18_4) {
	using testing::Pair;
	constexpr auto filename { "../day18/input/input_18_4.txt" };
	std::unordered_set<char> keys_to_find { };
	std::unordered_set<char> all_locks { };
	std::unordered_map<char, Point> POI_map { };
	std::unordered_map<Point, char> point_map { };
	auto start = parse_input(filename, point_map, POI_map, keys_to_find, all_locks);
	Point p { 1, 1 };
	GTEST_ASSERT_EQ(start, p);
	//ASSERT_THAT(keys_to_find, UnorderedElementsAre('a', 'f', 'e', 'b', 'c', 'd'));
	//ASSERT_THAT(all_locks, UnorderedElementsAre('D', 'E', 'C', 'A', 'B'));
	/*
	ASSERT_THAT(POI_map,
			UnorderedElementsAre(Pair('f', Point { 1, 1 }), Pair('D', Point { 3, 1 }), Pair('E', Point { 5, 1 }), Pair('e', Point { 7, 1 }),
					Pair('C', Point { 9, 1 }), Pair('b', Point { 11, 1 }), Pair('A', Point { 13, 1 }), Pair('@', Point { 15, 1 }), Pair('a', Point {
							17, 1 }), Pair('B', Point { 19, 1 }), Pair('c', Point { 21, 1 }), Pair('d', Point { 1, 3 })));
	*/
	auto const reach_map = create_reach_map(point_map, POI_map);
	print_reach_map(reach_map,point_map);
	auto part1 = find_min_steps(point_map, keys_to_find, reach_map, POI_map, start);
	GTEST_ASSERT_EQ(part1, 132);
}

GTEST_TEST(day18, input_18_3) {
	using testing::Pair;
	constexpr auto filename { "../day18/input/input_18_3.txt" };
	std::unordered_set<char> keys_to_find { };
	std::unordered_set<char> all_locks { };
	std::unordered_map<char, Point> POI_map { };
	std::unordered_map<Point, char> point_map { };
	auto start = parse_input(filename, point_map, POI_map, keys_to_find, all_locks);
	Point p { 8, 4 };
	GTEST_ASSERT_EQ(start, p);
	//ASSERT_THAT(keys_to_find, UnorderedElementsAre('a', 'f', 'e', 'b', 'c', 'd'));
	//ASSERT_THAT(all_locks, UnorderedElementsAre('D', 'E', 'C', 'A', 'B'));
	/*
	ASSERT_THAT(POI_map,
			UnorderedElementsAre(Pair('f', Point { 1, 1 }), Pair('D', Point { 3, 1 }), Pair('E', Point { 5, 1 }), Pair('e', Point { 7, 1 }),
					Pair('C', Point { 9, 1 }), Pair('b', Point { 11, 1 }), Pair('A', Point { 13, 1 }), Pair('@', Point { 15, 1 }), Pair('a', Point {
							17, 1 }), Pair('B', Point { 19, 1 }), Pair('c', Point { 21, 1 }), Pair('d', Point { 1, 3 })));
	*/
	auto const reach_map = create_reach_map(point_map, POI_map);
	print_reach_map(reach_map,point_map);
	auto part1 = find_min_steps(point_map, keys_to_find, reach_map, POI_map, start);
	GTEST_ASSERT_EQ(part1, 132);
}
