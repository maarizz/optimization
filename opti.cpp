// Note: Change the file path before running

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <set>
#include <numeric>
#include <random>
#include <cmath>

using namespace std;

struct Building {
    double x, y, cost;
    int id;
};

struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

struct Centroid {
    double x, y;
};

class FastPolygonSolver {
private:
    vector<Building> buildings;
    int N, minK;
    double bestCost;
    vector<Point> bestPolygon;

    vector<Point> createRectangle(const vector<int>& indices) {
        if (indices.empty()) return {};

        double minX = buildings[indices[0]].x, maxX = buildings[indices[0]].x;
        double minY = buildings[indices[0]].y, maxY = buildings[indices[0]].y;

        for (int idx : indices) {
            minX = min(minX, buildings[idx].x);
            maxX = max(maxX, buildings[idx].x);
            minY = min(minY, buildings[idx].y);
            maxY = max(maxY, buildings[idx].y);
        }

        return {Point(minX, minY), Point(maxX, minY), Point(maxX, maxY), Point(minX, maxY)};
    }

    double calculateRectanglePerimeter(const vector<int>& indices) {
        if (indices.empty()) return 0;

        double minX = buildings[indices[0]].x, maxX = buildings[indices[0]].x;
        double minY = buildings[indices[0]].y, maxY = buildings[indices[0]].y;

        for (int idx : indices) {
            minX = min(minX, buildings[idx].x);
            maxX = max(maxX, buildings[idx].x);
            minY = min(minY, buildings[idx].y);
            maxY = max(maxY, buildings[idx].y);
        }

        return 2 * ((maxX - minX) + (maxY - minY));
    }

    void evaluateSelection(const vector<int>& selected) {
        if (selected.size() < minK) return;

        double perimeter = calculateRectanglePerimeter(selected);
        double totalBuildingCost = 0;
        for (int idx : selected) {
            totalBuildingCost += buildings[idx].cost;
        }

        double totalCost = perimeter + totalBuildingCost;
        if (totalCost < bestCost) {
            bestCost = totalCost;
            bestPolygon = createRectangle(selected);
        }
    }

    vector<int> fastGreedySelection() {
        vector<int> result;
        vector<bool> used(N, false);
        vector<int> indices(N);
        iota(indices.begin(), indices.end(), 0);
        sort(indices.begin(), indices.end(), [&](int a, int b) {
            return buildings[a].cost < buildings[b].cost;
        });

        for (int i = 0; i < minK && i < N; i++) {
            result.push_back(indices[i]);
            used[indices[i]] = true;
        }

        for (int i = minK; i < N && i < minK + 10; i++) {
            if (buildings[indices[i]].cost < 0) {
                vector<int> temp = result;
                temp.push_back(indices[i]);

                double currentCost = calculateRectanglePerimeter(result);
                for (int idx : result) currentCost += buildings[idx].cost;

                double newCost = calculateRectanglePerimeter(temp);
                for (int idx : temp) newCost += buildings[idx].cost;

                if (newCost < currentCost) {
                    result = temp;
                }
            }
        }

        return result;
    }

    vector<int> selectAllNegativeCosts() {
        vector<int> negativeCost;
        for (int i = 0; i < N; i++) {
            if (buildings[i].cost < 0) {
                negativeCost.push_back(i);
            }
        }

        if (negativeCost.size() >= minK) {
            return negativeCost;
        }

        vector<int> positiveCost;
        for (int i = 0; i < N; i++) {
            if (buildings[i].cost >= 0) {
                positiveCost.push_back(i);
            }
        }

        sort(positiveCost.begin(), positiveCost.end(), [&](int a, int b) {
            return buildings[a].cost < buildings[b].cost;
        });

        vector<int> result = negativeCost;
        for (int i = 0; i < positiveCost.size() && result.size() < minK; i++) {
            result.push_back(positiveCost[i]);
        }

        return result;
    }

    vector<vector<int>> kMeansClustering(int kClusters, int maxIter = 100) {
        vector<Centroid> centroids(kClusters);
        vector<int> assignment(N, -1);
        vector<vector<int>> clusters(kClusters);

        mt19937 rng(42);
        uniform_int_distribution<int> uni(0, N - 1);
        for (int i = 0; i < kClusters; ++i) {
            int idx = uni(rng);
            centroids[i] = {buildings[idx].x, buildings[idx].y};
        }

        for (int iter = 0; iter < maxIter; ++iter) {
            bool changed = false;
            for (int i = 0; i < N; ++i) {
                double minDist = 1e18;
                int best = -1;
                for (int j = 0; j < kClusters; ++j) {
                    double dx = buildings[i].x - centroids[j].x;
                    double dy = buildings[i].y - centroids[j].y;
                    double d = dx * dx + dy * dy;
                    if (d < minDist) {
                        minDist = d;
                        best = j;
                    }
                }
                if (assignment[i] != best) {
                    assignment[i] = best;
                    changed = true;
                }
            }

            vector<double> sumX(kClusters, 0), sumY(kClusters, 0);
            vector<int> count(kClusters, 0);
            for (int i = 0; i < N; ++i) {
                int cl = assignment[i];
                sumX[cl] += buildings[i].x;
                sumY[cl] += buildings[i].y;
                count[cl]++;
            }

            for (int j = 0; j < kClusters; ++j) {
                if (count[j] > 0) {
                    centroids[j].x = sumX[j] / count[j];
                    centroids[j].y = sumY[j] / count[j];
                }
            }

            if (!changed) break;
        }

        for (int i = 0; i < N; ++i) {
            clusters[assignment[i]].push_back(i);
        }

        return clusters;
    }

public:
    FastPolygonSolver(vector<Building>& buildings, int minK)
        : buildings(buildings), N(buildings.size()), minK(minK), bestCost(1e18) {}

    pair<double, vector<Point>> solve() {
        evaluateSelection(selectAllNegativeCosts());
        evaluateSelection(fastGreedySelection());

        vector<int> indices(N);
        iota(indices.begin(), indices.end(), 0);
        sort(indices.begin(), indices.end(), [&](int a, int b) {
            return buildings[a].cost < buildings[b].cost;
        });
        evaluateSelection(vector<int>(indices.begin(), indices.begin() + minK));

        for (int kClusters : {3, 5, 10}) {
            auto clusters = kMeansClustering(kClusters);
            for (const auto& cluster : clusters) {
                if (cluster.size() >= minK) {
                    evaluateSelection(cluster);
                }
            }
        }

        return {bestCost, bestPolygon};
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int N, K;
    ifstream inputFile("C:/Users/hp/Desktop/Opti_TC/input03.txt");
    if (!inputFile.is_open()) {
        cerr << "Error opening input.txt" << endl;
        return 1;
    }

    inputFile >> N >> K;
    vector<Building> buildings(N);
    for (int i = 0; i < N; i++) {
        inputFile >> buildings[i].x >> buildings[i].y >> buildings[i].cost;
        buildings[i].id = i;
    }
    inputFile.close();

    FastPolygonSolver solver(buildings, K);
    pair<double, vector<Point>> ans = solver.solve();

    cout << fixed << setprecision(6) << ans.first << endl;
    if (!ans.second.empty()) {
        int n = ans.second.size();
        for (int i = 0; i < n; i++) {
            int next = (i + 1) % n;
            cout << fixed << setprecision(6)
                 << ans.second[i].x << " " << ans.second[i].y << " "
                 << ans.second[next].x << " " << ans.second[next].y << endl;
        }
    }

    return 0;
}
