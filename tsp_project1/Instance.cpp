#include "Instance.h"

#include <iostream>
#include <algorithm>

Instance::Instance()
{
    using clock = std::chrono::high_resolution_clock;

    std::vector<std::vector<int> > costMatrix = { { intMax, 14, 4, 10, 20 },
                                                  { 14, intMax, 7,  8,  7 },
                                                  { 4, 5, intMax,  7,  16 },
                                                  { 11, 7,  9, intMax,  2 },
                                                  { 18, 7, 17,  4, intMax } };

    auto begin = clock::now();
    prepareTree(costMatrix);
    auto end = clock::now();
    time = end - begin;

    printSolution();
    printTime();

    begin = clock::now();
    bruteForce(costMatrix);
    end = clock::now();
    time = end - begin;

    printSolution();
    printTime();
}

Instance::Instance(std::list<std::tuple<int, int, int> > cities)
{
    using clock = std::chrono::high_resolution_clock;

    std::vector<std::vector<int> > costMatrix(cities.size(), std::vector<int>(cities.size()));
    int x, y;
    for (int i = 0; i < cities.size(); ++i)
    {
        auto first = std::next(cities.begin(), i);
        for (auto k = 0; k < cities.size(); ++k)
        {
            auto second = std::next(cities.begin(), k);
            x = std::get<1>(*first) - std::get<1>(*second);
            y = std::get<2>(*first) - std::get<2>(*second);
            costMatrix[i][k] = (lround(sqrt((x * x + y * y))));
        }
    }
    for (int i = 0; i < cities.size(); ++i)
    {
        costMatrix[i][i] = intMax;
    }

    auto begin = clock::now();
    prepareTree(costMatrix);
    auto end = clock::now();
    time = end - begin;
}

Instance::Instance(std::vector<std::vector<int> > costMatrix)
{
    using clock = std::chrono::high_resolution_clock;

    for (int i = 0; i < costMatrix.size(); ++i)
    {
        costMatrix[i][i] = intMax;
    }
    auto begin = clock::now();
    prepareTree(costMatrix);
    auto end = clock::now();
    time = end - begin;
}

void Instance::printSolution()
{
    std::cout << std::endl;
    std::cout << "Cost: " << minCost << std::endl;
    std::cout << std::endl;

    bestPath.push_back(0);
    std::cout << "Path: ";

    for (const auto& e : bestPath)
    {
        std::cout << e << " ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

void Instance::printTime()
{   
    std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(time).count() << " ns" << std::endl;
    std::cout << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(time).count() << " ms" << std::endl;
    std::cout << std::endl;
    std::cout << std::chrono::duration_cast<std::chrono::seconds>(time).count() << " s" << std::endl;
    std::cout << "---------------------------------" << std::endl;
}

int Instance::calculateLowerBound(std::vector<std::vector<int> >& matrix, std::vector<int>& path) const
{
    int lowerBound = 0;
    for (int i = 0; i < path.size() - 1; ++i)
    {
        lowerBound += matrix[path[i]][path[i + 1]];
    }
    if (path.size() == matrix.size())
    {
        lowerBound += matrix[path.back()][0];
    }
    return lowerBound;
}


void Instance::prepareTree(std::vector<std::vector<int> >& matrix)
{
    std::vector<int> path;
    path.reserve(matrix.size());
    path.push_back(0);
    Node root{ path, 0 };
    branchAndBound(matrix, root);
}

void Instance::branchAndBound(std::vector<std::vector<int> >& matrix, Node node)
{
    if ((node.currentPath.size() > 1) && (calculateLowerBound(matrix, node.currentPath) > minCost))
    {
        return;
    }
    if (node.currentPath.size() == matrix.size())
    {
        int lowerBound = calculateLowerBound(matrix, node.currentPath);
        if (lowerBound < minCost)
        {
            minCost = lowerBound;
            bestPath = node.currentPath;
        }
        return;
    }
    std::vector<Node> nodes;
    nodes.reserve(matrix.size() - 1);

    for (int i = 1; i < matrix.size(); ++i)
    {
        if (std::find(node.currentPath.begin(), node.currentPath.end(), i) == node.currentPath.end())
        {
            Node newNode{ node.currentPath, i };
            newNode.currentPath.push_back(i);
            nodes.push_back(newNode);
        }
    }
    for (const auto& next : nodes)
    {
        branchAndBound(matrix, next);
    }
}

int Instance::calculateBruteForce(std::vector<std::vector<int> >& matrix, std::vector<int>& path) const
{
    int cost = 0;
    cost += matrix[0][path.front()];
    for (int i = 0; i < path.size() - 1; ++i)
    {
        cost += matrix[path[i]][path[i + 1]];
    }
    cost += matrix[path.back()][0];
    return cost;
}

void Instance::bruteForce(std::vector<std::vector<int> >& matrix)
{
    bestPath.clear();
    minCost = intMax;
    std::vector<int> path;
    path.reserve(matrix.size());
    for (int i = 1; i < matrix.size(); ++i)
    {
        path.push_back(i);
    }
    do
    {
        int cost = calculateBruteForce(matrix, path);
        if (cost < minCost)
        {
            minCost = cost;
            bestPath = path;
        }
    } while (std::next_permutation(path.begin(), path.end()));
}
