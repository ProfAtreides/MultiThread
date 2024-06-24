#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <direct.h>

// Dane wejściowe (przykład)
int n;

int bestSolution;

// Koszty na jednostkę odległości pomiędzy urządzeniami
int costMatrix[1024][1024];

// Odległości pomiędzy lokalizacjami
int distanceMatrix[1024][1024];

void loadData() {
    std::fstream file("../Zad2/data.txt", std::ios_base::in);

    if (!file.is_open()) {
        std::cout << "Error: file not found\n";
        exit(1);
    }

    file >> n >> bestSolution;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file >> costMatrix[i][j];
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file >> distanceMatrix[i][j];
        }
    }
}

// Generowanie losowych początkowych rozwiązań
std::vector<std::vector<int>> generateInitialSolutions(int numSolutions) {
    std::vector<std::vector<int>> solutions(numSolutions, std::vector<int>(n));
    for (auto &solution: solutions) {
        for (int i = 0; i < n; ++i) {
            solution[i] = i;
        }
        std::random_shuffle(solution.begin(), solution.end());
    }
    return solutions;
}

// Funkcja oceny rozwiązania (sumaryczny koszt)
int evaluateSolution(const std::vector<int> &solution) {
    int totalCost = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            totalCost += costMatrix[i][j] * distanceMatrix[solution[i]][solution[j]];
        }
    }
    return totalCost;
}

// Kombinacja dwóch rozwiązań
std::vector<int> combineSolutions(const std::vector<int> &sol1, const std::vector<int> &sol2) {
    std::vector<int> combined(sol1.size());
    for (size_t i = 0; i < sol1.size(); ++i) {
        combined[i] = (i % 2 == 0) ? sol1[i] : sol2[i];
    }
    return combined;
}

// Poprawa rozwiązania (np. przez lokalne przeszukiwanie)
std::vector<int> improveSolution(const std::vector<int> &solution) {
    std::vector<int> improvedSolution = solution;
    // Prosta zamiana dwóch lokalizacji
    if (solution.size() > 1) {
        std::swap(improvedSolution[0], improvedSolution[1]);
    }

    return improvedSolution;
}

// Główna funkcja harmonogramująca z użyciem Scatter Search
void scatterSearch(int rank, int size) {
    int numSolutions = 50;

    // Generowanie początkowej populacji
    std::vector<std::vector<int>> solutions = generateInitialSolutions(numSolutions);

    // Główna pętla Scatter Search
    for (int iteration = 0; iteration < 100; ++iteration) {
        // Kombinacja i poprawa rozwiązań
        std::vector<std::vector<int>> newSolutions;
        for (size_t i = 0; i < solutions.size(); ++i) {
            for (size_t j = i + 1; j < solutions.size(); ++j) {
                std::vector<int> combined = combineSolutions(solutions[i], solutions[j]);
                std::vector<int> improved = improveSolution(combined);
                newSolutions.push_back(improved);
            }
        }

        // Selekcja najlepszych rozwiązań
        solutions.insert(solutions.end(), newSolutions.begin(), newSolutions.end());
        std::sort(solutions.begin(), solutions.end(), [](const std::vector<int> &a, const std::vector<int> &b) {
            return evaluateSolution(a) < evaluateSolution(b);
        });
        solutions.resize(numSolutions);  // Zachowaj tylko najlepsze rozwiązania
    }

    // Proces 0: Zbiera wyniki i wyświetla harmonogram
    if (rank == 0) {
        std::cout << "Best solution:\n";
        for (int loc: solutions[0]) {
            std::cout << loc << " ";
        }
        std::cout << "\nTotal cost: " << evaluateSolution(solutions[0]) << "\n";
        std::cout << "\nExpected cost: " << bestSolution << "\n";
    }
}

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);
    loadData();
    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    scatterSearch(rank, size);

    MPI_Finalize();
    return 0;
}
