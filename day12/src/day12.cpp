#include <algorithm>
#include <array>
#include <assert.h>
#include <deque>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
constexpr int DIM = 3;
constexpr int NUM_PLANETS = 4;

class Planet {
public:
  std::array<int, DIM> position;
  std::array<int, DIM> velocity;
};

std::ostream &operator<<(std::ostream &out, Planet const &planet) {

  out << fmt::format("pos<x={}, y={}, z={}> vel<x={} y={} z={}>",
                     planet.position[0], planet.position[1], planet.position[2],
                     planet.velocity[0], planet.velocity[1],
                     planet.velocity[2]);
  /* csv output
   out << fmt::format("{},{},{},{},{},{}", planet.position[0],
                      planet.position[1], planet.position[2],
   planet.velocity[0], planet.velocity[1], planet.velocity[2]); */
  return out;
}

std::vector<Planet> string2vector(std::string buffer) {
  std::regex reg{R"(-?\d+)"};
  std::smatch m;
  std::string str = buffer;
  std::vector<Planet> planets;
  Planet temp_planet{};
  int i = 0;
  while (regex_search(str, m, reg)) {
    temp_planet.position[i] = std::stoi(m[0]);
    i++;
    if (i == DIM) {
      i = 0;
      planets.push_back(temp_planet);
    }

    str = m.suffix().str(); // Proceed to the next match
  }
  return planets;
}

std::array<int, DIM> calc_gravity(Planet &planet_a, Planet &planet_b,
                                  std::array<int, DIM> &gravity) {

  for (int z = 0; z < DIM; ++z) {
    if (planet_a.position[z] < planet_b.position[z]) {
      gravity[z]++;
    } else if (planet_a.position[z] > planet_b.position[z]) {
      gravity[z]--;
    }
  }
  return gravity;
}

Planet update_planet(Planet &planet, std::array<int, DIM> &gravity) {
  for (int i = 0; i < DIM; ++i) {
    planet.velocity[i] += gravity[i];
    planet.position[i] += planet.velocity[i];
  }
  return planet;
}

std::vector<Planet> calculate(std::vector<Planet> &planets) {
  std::array<std::array<int, DIM>, NUM_PLANETS> gravity{};
  for (int i = 0; i < planets.size(); ++i) {
    for (int j = 0; j < planets.size(); ++j) {
      if (i == j)
        continue;
      gravity[i] = calc_gravity(planets[i], planets[j], gravity[i]);
    }
  }
  int i = 0;
  for (auto &planet : planets) {
    planet = update_planet(planet, gravity[i]);
    i++;
  }
  return planets;
}

void tests() {}

int acc_abbs(int acc, int inc) { return acc + std::abs(inc); }

int calc_energy(std::vector<Planet> planets) {
  int total_energy = 0;
  for (const auto &planet : planets) {
    int pot_energy = 0;
    int kin_energy = 0;
    for (int i = 0; i < DIM; ++i) {
      kin_energy += std::abs(planet.velocity[i]);
      pot_energy += std::abs(planet.position[i]);
    }
    total_energy += pot_energy * kin_energy;
  }
  return total_energy;
}

int simulate(std::vector<Planet> planets, int steps, bool verbose = false) {
  for (int i = 0; i < steps; ++i) {
    calculate(planets);
    if (verbose) {
      std::cout << i + 1 << ",";
      for (auto val : planets) {
        std::cout << val << ",";
      }
      std::cout << "\n";
    }
  }

  return calc_energy(planets);
}

bool is_period(std::vector<int> &arr, int begin, int period,
               int test_len = 200000) {
  for (int i = 0; i < test_len; ++i) {
    if (arr[begin + i] != arr[begin + i + period]) {
      return false;
    }
  }
  return true;
}

int find_period(std::vector<int> &arr) {
  bool period_found = false;
  auto i = 0;
  while (!period_found && i < arr.size() - 1) {
    i++;
    if (arr[i] == arr[0]) {
      period_found = is_period(arr, 0, i);
    }
  }
  if (period_found)
    return i;
  else
    return -1;
}
long long int lcm_usr(long long int a, long long int b) {
  return std::lcm(a, b);
}

long long int find_lcm(std::vector<Planet> planets, int steps = 100000) {
  // store all positions, velocities for each step
  constexpr int NUM_COL = NUM_PLANETS * DIM * 2;
  std::array<std::vector<int>, NUM_COL> pos_vel{};
  std::vector<long long int> periods{};
  // simulate for a time
  for (int step = 0; step < steps; ++step) {

    calculate(planets);
    int i = 0;
    for (auto planet : planets) {
      for (auto pos : planet.position) {
        pos_vel[i].push_back(pos);
        i++;
      }
      for (auto pos : planet.velocity) {
        pos_vel[i].push_back(pos);
        i++;
      }
    }
    // check if we have periods found already
    if (step == steps - 1) {
      periods.clear();
      for (int i = 0; i < NUM_COL; ++i) {
        auto period = find_period(pos_vel[i]);
        if (period == -1) {
          steps *= 2;
          break;
        }
        periods.push_back(static_cast<long long int>(period));
      }
    }
  }

  return std::accumulate(periods.begin(), periods.end(), 1LL, lcm_usr);
}

int main() {
  tests();
  std::ifstream infile("./input/input_12.txt");
  std::vector<Planet> planets;
  std::stringstream ss;
  ss << infile.rdbuf();
  std::string input = ss.str();
  planets = string2vector(input);
  std::cout << "Part1: " << simulate(planets, 10) << "\n";
  std::cout << "Part2: " << find_lcm(planets, 10000);
}
