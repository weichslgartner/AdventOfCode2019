#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
#include <unordered_map>
#include <unordered_set>
#include <iterator>
#include <deque>

using SucMap = std::unordered_map<std::string, std::vector<std::string>>;
using PredMap = std::unordered_map<std::string, std::string>;
using CostMap = std::unordered_map<std::string, int>;

struct Node
{
    std::string target;
    std::string from_node;
};

struct Graph
{
public:
    SucMap successors;
    PredMap predecessors;
};

void cnt_orbits_recursive(SucMap map, std::string element, int &cnt)
{
    auto entry = map.find(element);
    if (entry != map.end())
    {
        for (auto it : map[element])
        {
            cnt++;
            cnt_orbits_recursive(map, it, cnt);
        }
    }
}

void cnt_orbits(SucMap suc_map, std::string element, int &cnt)
{
    std::deque<std::string> deq;
    deq.push_back(element);
    while (!deq.empty())
    {
        std::string e = deq.front();
        deq.pop_front();
        auto entry = suc_map.find(e);
        if (entry != suc_map.end())
        {
            for (auto it : suc_map[e])
            {
                cnt++;
                //cnt_orbits(map, it, cnt);
                deq.push_back(it);
            }
        }
    }
}

std::string gen_key(std::string node1, std::string node2)
{
    return node1 + ")" + node2;
}

int find_shortest_path(Graph &graph, std::string src, std::string dest)
{
    CostMap costs;
    std::unordered_set<std::string> visited;
    std::deque<Node> dec;
    Node next{graph.predecessors[src], src};
    dec.push_back(next);
    visited.insert(src);
    costs[src] = 0;
    while (!dec.empty())
    {
        auto cur_element = dec.front();
        dec.pop_front();
        costs[cur_element.target] = costs[cur_element.from_node] + 1;
        if (cur_element.target.compare(dest) == 0)
        {
            //std::cout << "found target " << costs[cur_element.target] - 2 << "\n";
            return costs[cur_element.target] - 2;
        }
        visited.insert(cur_element.target);
        next.from_node = cur_element.target;
        // add all unvisited successors
        for (auto suc : graph.successors[cur_element.target])
        {
            if (visited.find(suc) == visited.end())
            {
                next.target = suc;
                dec.push_back(next);
            }
        }
        // add predecessors
        auto pred = graph.predecessors.find(cur_element.target);
        if (pred != graph.predecessors.end())
        {
            next.target = graph.predecessors[cur_element.target];
            dec.push_back(next);
        }
    }
    // no path found
    return 0;
}

int eval(SucMap map)
{
    int count = 0;
    for (auto element : map)
    {
        cnt_orbits(map, element.first, count);
    }
    return count;
}

Graph parse_orbits(std::stringstream &ss)
{
    Graph graph;
    std::string temp;
    std::vector<std::string> lines;

    while (getline(ss, temp, '\n'))
    {
        lines.push_back(temp);
    }
    for (auto line : lines)
    {
        std::stringstream ss2(line);
        std::vector<std::string> tokenz;
        while (getline(ss2, temp, ')'))
        {
            tokenz.push_back(temp);
        }
        if (graph.predecessors.find(tokenz[1]) != graph.predecessors.end())
        {
            std::cout << "already in"
                      << "\n";
        }
        graph.predecessors[tokenz[1]] = tokenz[0]; //each token has esactly one predecessor
        auto entry = graph.successors.find(tokenz[0]);
        if (entry == graph.successors.end())
        {
            graph.successors[tokenz[0]] = std::vector<std::string>();
        }
        graph.successors[tokenz[0]].push_back(tokenz[1]);
        // std::cout << tokenz[0] << " " << tokenz[1] << "\n";
    }

    return graph;
}

int main()
{
    //tests();
    std::ifstream infile("../input_06.txt");
    std::stringstream ss;
    ss << infile.rdbuf();
    auto orbits = parse_orbits(ss);
    auto cnt = eval(orbits.successors);
    std::cout << "Part 1: " << cnt << "\n";
    auto length = find_shortest_path(orbits, "YOU", "SAN");
    std::cout << "Part 2: " << length << "\n";
    return 0;
}