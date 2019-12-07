#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
#include <unordered_set>
#include <iterator>


struct Instruction
{
public:
    char direction;
    int length;
};

struct Point
{
public:
    int x;
    int y;
    int steps;
    const bool
    operator==(const Point &obj) const
    {
        return obj.x == x && obj.y == y;
    }
};

namespace std
{
template <>
struct hash<Point>
{

    std::size_t operator()(const Point &p) const
    {
        std::size_t val{0};
        val = 100000 * p.y + p.x;
        return val;
    }
};
} // namespace std

void print_vector(std::vector<Instruction> vec)
{
    for (auto v : vec)
    {
        std::cout << v.direction << " " << v.length << "-";
    }
    std::cout << "\n";
}

int combined_steps(Point p1, Point p2){
    return p1.steps + p2.steps;
}

int manhattan_distance(Point p1, Point p2)
{
    return abs(p1.x - p2.x) + abs(p1.y + p2.y);
}

std::vector<std::vector<Instruction>> string2vector(std::stringstream &ss)
{
    std::vector<std::vector<Instruction>> two_vec;
    std::vector<Instruction> vec;
    Instruction inst;
    std::string temp_number = "";
    int i = 0;
    for (char a : ss.str())
    {
        if (a == ',')
        {
            inst.length = atoi(temp_number.c_str());
            vec.push_back(inst);
            inst = {};
            temp_number = "";
        }
        else if (a == '\n')
        {   
            inst.length = atoi(temp_number.c_str());
            vec.push_back(inst);
            inst = {};
            temp_number = "";
            two_vec.push_back(vec);
            vec = {};
        }
        else if (a == 'R' || a == 'L' || a == 'U' || a == 'D')
        {
            inst.direction = a;
        }
        else
        {
            temp_number += a;
        }
    }
    two_vec.push_back(vec);
    return two_vec;
}

void tests()
{
    std::stringstream ss{"1,0,0,0,99"};
}

void find_intersect(std::vector<std::unordered_set<Point, std::hash<Point>>> two_sets){
    Point zero{0,0};
    int min_distance = INT32_MAX;
    int min_steps = INT32_MAX;
    for (auto element : two_sets[0]){
        auto found = two_sets[1].find(element);
        if(found != two_sets[1].end()){
            auto dist = manhattan_distance(zero, element);
            if (dist < min_distance){
                min_distance = dist;
            }
            auto steps = combined_steps(element,(*found));
            if(steps < min_steps){
                min_steps = steps;
            }
        }
    }
    std::cout << "min manhattan: " << min_distance << " \n";
    std::cout << "min combined steps: " << min_steps << " \n";
}


void eval(std::vector<std::vector<Instruction>> two_vec)
{
    std::vector<std::unordered_set<Point, std::hash<Point>>> two_sets;
    for (auto vec : two_vec)
    {
        std::unordered_set<Point, std::hash<Point>> point_set;
        Point curpoint{0,0};
        int steps = 0;
        for (auto element : vec)
        {
            for(int i = 0; i < element.length; ++i){
                switch (element.direction)
                {
                case 'R':
                    curpoint = Point{curpoint.x+1, curpoint.y};
                    break;
                case 'L':
                    curpoint = Point{curpoint.x-1, curpoint.y};
                    break;
                case 'U':
                    curpoint = Point{curpoint.x, curpoint.y+1};
                    break;
                case 'D':
                    curpoint = Point{curpoint.x, curpoint.y-1};
                    break;
                default:
                    break;
                }
                steps++;
                curpoint.steps = steps;
                point_set.emplace(curpoint);
            }
        }
        two_sets.push_back(point_set);
    }
    find_intersect(two_sets);
}

int main()
{
    tests();
    std::ifstream infile("../input_03.txt");
    std::vector<std::vector<Instruction>> two_vec;
    std::stringstream ss;
    ss << infile.rdbuf();
    two_vec = string2vector(ss);
    eval(two_vec);
    return 0;
}