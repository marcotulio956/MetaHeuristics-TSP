/* 
    
 * tsp_heuristics.cpp
 *
 *  Created on: 04/08/2026

 About: This program implements heuristics for the Traveling Salesman Problem (TSP), including nearest neighbor and simulated annealing algorithms. It reads TSP instances from files based on the TSPLIB 95 format, computes routes, and outputs results to a CSV file.
 Author: Marco Túlio Siqueira da Mata, PPGCC UFMG.

 AI Disclaimer: Part of this code was created together with the assistance of AI tools for autocompletion; 
 exclusively for parsing and writing files, measuring execution time for batches, generating random numbers, casting correct data types, documenting functions, and validating the parser in the runTests() function. 
 The AI was not used for the core logic of the TSP heuristics.

*/

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

/* 

Structs for representing cities, TSP instances, route results, and simulated annealing configuration.

*/

struct City {
    int id{};
    double x{};
    double y{};
};

struct Instance {
    std::string name;
    std::string edge_weight_type;
    std::vector<City> cities;
    std::vector<std::vector<int>> distance;
};

struct RouteResult {
    std::vector<int> route;
    long long cost{};
    double milliseconds{};
};

struct AnnealingConfig {
    int iterations_per_city = 2500;
    double initial_temperature = 0.0;
    double cooling = 0.995;
    int minimum_temperature_iterations = 1000;
};

/*

Utility functions for string manipulation and parsing.

*/

static std::string trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(" \t\r\n");
    return value.substr(first, last - first + 1);
}

static std::string headerValue(const std::string& line) {
    const auto separator = line.find(':');
    if (separator == std::string::npos) {
        return {};
    }
    return trim(line.substr(separator + 1));
}

/*

See README.md for details on the distance calculation methods.

*/

static int euclideanDistance(const City& first, const City& second) {
    const double dx = first.x - second.x;
    const double dy = first.y - second.y;
    return static_cast<int>(std::lround(std::sqrt(dx * dx + dy * dy)));
}

static int attDistance(const City& first, const City& second) {
    const double dx = first.x - second.x;
    const double dy = first.y - second.y;
    const double rij = std::sqrt((dx * dx + dy * dy) / 10.0);
    const int tij = static_cast<int>(std::lround(rij));
    return tij < rij ? tij + 1 : tij;
}

/*

Function to read a TSP instance from a file. (AI help)

*/

static Instance readInstance(const fs::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Nao foi possivel abrir " + path.string());
    }

    Instance instance;
    int dimension = 0;
    bool readingCoordinates = false;
    std::string line;
    while (std::getline(input, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        if (line == "EOF") {
            break;
        }
        if (!readingCoordinates) {
            if (line == "NODE_COORD_SECTION") {
                readingCoordinates = true;
            } else if (line.rfind("NAME", 0) == 0) {
                instance.name = headerValue(line);
            } else if (line.rfind("DIMENSION", 0) == 0) {
                dimension = std::stoi(headerValue(line));
            } else if (line.rfind("EDGE_WEIGHT_TYPE", 0) == 0) {
                instance.edge_weight_type = headerValue(line);
            }
            continue;
        }

        std::istringstream row(line);
        City city;
        if (!(row >> city.id >> city.x >> city.y)) {
            throw std::runtime_error("Coordenada invalida em " + path.string());
        }
        instance.cities.push_back(city);
        if (dimension > 0 && static_cast<int>(instance.cities.size()) == dimension) {
            break;
        }
    }

    if (instance.name.empty()) {
        instance.name = path.stem().string();
    }
    if (dimension <= 0 || static_cast<int>(instance.cities.size()) != dimension) {
        throw std::runtime_error("Numero de cidades invalido em " + path.string());
    }
    if (instance.edge_weight_type != "EUC_2D" && instance.edge_weight_type != "ATT") {
        throw std::runtime_error("Tipo de distancia nao suportado em " + path.string());
    }

    instance.distance.assign(dimension, std::vector<int>(dimension, 0));
    for (int i = 0; i < dimension; ++i) {
        for (int j = i + 1; j < dimension; ++j) {
            const int distance = instance.edge_weight_type == "ATT"
                ? attDistance(instance.cities[i], instance.cities[j])
                : euclideanDistance(instance.cities[i], instance.cities[j]);
            instance.distance[i][j] = distance;
            instance.distance[j][i] = distance;
        }
    }
    return instance;
}

/*

Function to calculate the cost of a route in a TSP instance.

*/

static long long routeCost(const Instance& instance, const std::vector<int>& route) {
    if (route.size() != instance.cities.size()) {
        throw std::runtime_error("Rota com tamanho invalido");
    }
    std::vector<bool> visited(route.size(), false);
    long long cost = 0;
    for (std::size_t index = 0; index < route.size(); ++index) {
        const int current = route[index];
        const int next = route[(index + 1) % route.size()];
        if (current < 0 || current >= static_cast<int>(route.size()) || visited[current]) {
            throw std::runtime_error("Rota nao e uma permutacao valida");
        }
        visited[current] = true;
        cost += instance.distance[current][next];
    }
    return cost;
}

/*

Function to check if a route is valid for a TSP instance.

*/

static bool validRoute(const std::vector<int>& route, std::size_t dimension) {
    if (route.size() != dimension) {
        return false;
    }
    std::vector<bool> visited(dimension, false);
    for (const int city : route) {
        if (city < 0 || city >= static_cast<int>(dimension) || visited[city]) {
            return false;
        }
        visited[city] = true;
    }
    return true;
}

/**
 * Function to find a near-optimal route using the nearest neighbor heuristic.
 * @param instance The TSP instance.
 * @param start The starting city (default is 0).
 * @return A vector representing the route.
 */

static std::vector<int> nearestNeighbor(const Instance& instance, int start = 0) {
    const int dimension = static_cast<int>(instance.cities.size());
    if (start < 0 || start >= dimension) {
        throw std::runtime_error("Cidade inicial invalida");
    }
    std::vector<int> route;
    std::vector<bool> visited(dimension, false);
    route.reserve(dimension);
    route.push_back(start);
    visited[start] = true;

    while (static_cast<int>(route.size()) < dimension) {
        const int current = route.back();
        int candidate = -1;
        for (int city = 0; city < dimension; ++city) {
            if (!visited[city] && (candidate == -1 || instance.distance[current][city] < instance.distance[current][candidate] ||
                (instance.distance[current][city] == instance.distance[current][candidate] && city < candidate))) {
                candidate = city;
            }
        }
        route.push_back(candidate);
        visited[candidate] = true;
    }
    return route;
}

/**
 * Function to improve a route using the 2-opt heuristic.
 * @param instance The TSP instance.
 * @param route The initial route.
 * @return The improved route.
 */

static long long twoOptDelta(const Instance& instance, const std::vector<int>& route, int first, int second) {
    const int before = route[(first - 1 + static_cast<int>(route.size())) % static_cast<int>(route.size())];
    const int after = route[(second + 1) % static_cast<int>(route.size())];
    const int removed = instance.distance[before][route[first]] + instance.distance[route[second]][after];
    const int added = instance.distance[before][route[second]] + instance.distance[route[first]][after];
    return static_cast<long long>(added) - removed;
}

/**
 * Function to apply the 2-opt operation to a route.
 * @param route The route to modify.
 * @param first The index of the first city to swap.
 * @param second The index of the second city to swap.
 */

static void applyTwoOpt(std::vector<int>& route, int first, int second) {
    std::reverse(route.begin() + first, route.begin() + second + 1);
}

/**
 * Function to improve a route using the 2-opt heuristic.
 * @param instance The TSP instance.
 * @param route The initial route.
 * @return The improved route.
 */

static std::vector<int> improveTwoOpt(const Instance& instance, std::vector<int> route) {
    bool improved = true;
    const int dimension = static_cast<int>(route.size());
    while (improved) {
        improved = false;
        for (int first = 1; first < dimension - 1 && !improved; ++first) {
            for (int second = first + 1; second < dimension; ++second) {
                if (twoOptDelta(instance, route, first, second) < 0) {
                    applyTwoOpt(route, first, second);
                    improved = true;
                    break;
                }
            }
        }
    }
    return route;
}

/**
 * Function to run the nearest neighbor heuristic on a TSP instance.
 * @param instance The TSP instance.
 * @return A RouteResult containing the route, its cost, and the time taken.
 */

static RouteResult runNearestNeighbor(const Instance& instance) {
    const auto started = std::chrono::steady_clock::now();
    auto route = nearestNeighbor(instance);
    route = improveTwoOpt(instance, std::move(route));
    const auto finished = std::chrono::steady_clock::now();
    return {route, routeCost(instance, route), std::chrono::duration<double, std::milli>(finished - started).count()};
}

/**
 * Function to run the simulated annealing algorithm on a TSP instance.
 * @param instance The TSP instance.
 * @param seed The random seed.
 * @param config The annealing configuration.
 * @return A RouteResult containing the route, its cost, and the time taken.
 */

static RouteResult simulatedAnnealing(const Instance& instance, std::uint32_t seed, const AnnealingConfig& config) {
    const auto started = std::chrono::steady_clock::now();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> cityDistribution(1, static_cast<int>(instance.cities.size()) - 1);
    std::uniform_real_distribution<double> probability(0.0, 1.0);

    std::vector<int> current = nearestNeighbor(instance);
    current = improveTwoOpt(instance, std::move(current));
    std::vector<int> best = current;
    long long currentCost = routeCost(instance, current);
    long long bestCost = currentCost;
    const double temperature = config.initial_temperature > 0.0
        ? config.initial_temperature
        : std::max(100.0, static_cast<double>(bestCost) * 0.10);
    double currentTemperature = temperature;
    const int iterations = std::max(1, config.iterations_per_city * static_cast<int>(instance.cities.size()));

    for (int iteration = 0; iteration < iterations; ++iteration) {
        int first = cityDistribution(generator);
        int second = cityDistribution(generator);
        if (first > second) {
            std::swap(first, second);
        }
        if (first == second) {
            continue;
        }
        const long long delta = twoOptDelta(instance, current, first, second);
        if (delta <= 0 || probability(generator) < std::exp(-static_cast<double>(delta) / currentTemperature)) {
            applyTwoOpt(current, first, second);
            currentCost += delta;
            if (currentCost < bestCost) {
                best = current;
                bestCost = currentCost;
            }
        }
        currentTemperature *= config.cooling;
        if (currentTemperature < 1e-9) {
            currentTemperature = temperature * 0.001;
        }
    }

    const auto finished = std::chrono::steady_clock::now();
    return {best, bestCost, std::chrono::duration<double, std::milli>(finished - started).count()};
}

/** Testbenching functions for TSP heuristics. */

static std::vector<fs::path> instancePaths(const fs::path& root) {
    std::vector<fs::path> paths;
    paths.push_back(root / "att48.tsp");
    const fs::path euclidean = root / "EUC_2D";
    for (const auto& entry : fs::directory_iterator(euclidean)) {
        if (entry.is_regular_file() && entry.path().extension() == ".tsp") {
            paths.push_back(entry.path());
        }
    }
    std::sort(paths.begin() + 1, paths.end());
    return paths;
}

static void runTests(const fs::path& root) {
    Instance euclidean;
    euclidean.name = "synthetic-euc";
    euclidean.edge_weight_type = "EUC_2D";
    euclidean.cities = {{1, 0.0, 0.0}, {2, 3.0, 4.0}, {3, 3.0, 0.0}, {4, 0.0, 4.0}};
    euclidean.distance.assign(4, std::vector<int>(4, 0));
    for (int i = 0; i < 4; ++i) {
        for (int j = i + 1; j < 4; ++j) {
            euclidean.distance[i][j] = euclidean.distance[j][i] = euclideanDistance(euclidean.cities[i], euclidean.cities[j]);
        }
    }
    if (euclidean.distance[0][1] != 5 || euclidean.distance[0][2] != 3) {
        throw std::runtime_error("Falha no teste EUC_2D");
    }

    City attFirst{1, 0.0, 0.0};
    City attSecond{2, 3.0, 4.0};
    if (attDistance(attFirst, attSecond) != 2) {
        throw std::runtime_error("Falha no teste ATT");
    }

    const auto route = nearestNeighbor(euclidean);
    if (!validRoute(route, 4) || routeCost(euclidean, route) <= 0) {
        throw std::runtime_error("Falha na rota construtiva");
    }
    const auto improved = improveTwoOpt(euclidean, route);
    if (!validRoute(improved, 4) || routeCost(euclidean, improved) > routeCost(euclidean, route)) {
        throw std::runtime_error("Falha no 2-opt");
    }

    const auto att = readInstance(root / "att48.tsp");
    if (att.cities.size() != 48 || att.edge_weight_type != "ATT") {
        throw std::runtime_error("Falha ao ler att48");
    }
    const auto berlin = readInstance(root / "EUC_2D" / "berlin52.tsp");
    if (berlin.cities.size() != 52 || berlin.edge_weight_type != "EUC_2D") {
        throw std::runtime_error("Falha ao ler berlin52");
    }
    std::cout << "Todos os testes passaram.\n";
}

static void runBatch(const fs::path& root, const fs::path& output, int repetitions) {
    std::ofstream csv(output);
    if (!csv) {
        throw std::runtime_error("Nao foi possivel criar " + output.string());
    }
    csv << "instance,type,n,seed,constructive_cost,annealing_cost,constructive_ms,annealing_ms\n";
    for (const auto& path : instancePaths(root)) {
        const auto instance = readInstance(path);
        std::cout << "Processando " << instance.name << " (" << instance.cities.size() << " cidades)\n";
        for (int repetition = 0; repetition < repetitions; ++repetition) {
            const std::uint32_t seed = 20260917u + static_cast<std::uint32_t>(repetition) * 1009u + static_cast<std::uint32_t>(instance.cities.size());
            const auto constructive = runNearestNeighbor(instance);
            const auto annealing = simulatedAnnealing(instance, seed, AnnealingConfig{});
            if (!validRoute(constructive.route, instance.cities.size()) || !validRoute(annealing.route, instance.cities.size())) {
                throw std::runtime_error("Algoritmo produziu rota invalida em " + instance.name);
            }
            csv << instance.name << ',' << instance.edge_weight_type << ',' << instance.cities.size() << ',' << seed << ','
                << constructive.cost << ',' << annealing.cost << ',' << std::fixed << std::setprecision(3)
                << constructive.milliseconds << ',' << annealing.milliseconds << '\n';
            std::cout << "  semente " << seed << ": construtiva=" << constructive.cost << ", SA=" << annealing.cost << '\n';
        }
    }
}

int main(int argc, char** argv) {
    try {
        const fs::path root = fs::path(argv[0]).parent_path().empty()
            ? fs::current_path()
            : fs::path(argv[0]).parent_path();
        if (argc >= 2 && std::string(argv[1]) == "--test") {
            runTests(root);
            return 0;
        }
        const fs::path output = argc >= 2 ? fs::path(argv[1]) : root / "results.csv";
        const int repetitions = argc >= 3 ? std::stoi(argv[2]) : 5;
        if (repetitions <= 0) {
            throw std::runtime_error("O numero de repeticoes deve ser positivo");
        }
        runBatch(root, output, repetitions);
        std::cout << "Resultados gravados em " << output << '\n';
    } catch (const std::exception& error) {
        std::cerr << "Erro: " << error.what() << '\n';
        return 1;
    }
}
