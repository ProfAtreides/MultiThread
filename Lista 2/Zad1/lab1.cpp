#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>

// Struktura operacji
struct Operation {
    int taskId;
    int machineId;
    int processingTime;
};

std::vector<Operation> operations;

int numMachines;
int numTasks;

void loadData(){
    std::fstream file("../Zad1/data.txt", std::ios_base::in);

    if (!file.is_open()) {
        std::cout << "Error: file not found\n";
        exit(1);
    }

    file >> numMachines >> numTasks;
    for(int i = 0; i < numTasks; i++){
        for(int j = 0; j < numMachines; j++){
            Operation temp;
            temp.taskId = i + 1;
            file >> temp.machineId;
            file >> temp.processingTime;
            operations.push_back(temp);
        }
    }
}

// Generowanie losowych początkowych rozwiązań
std::vector<std::vector<int>> generateInitialSolutions(int numSolutions, int numOperations) {
    std::vector<std::vector<int>> solutions(numSolutions, std::vector<int>(numOperations));
    for (auto &solution : solutions) {
        for (int i = 0; i < numOperations; ++i) {
            solution[i] = i;
        }
        std::random_shuffle(solution.begin(), solution.end());
    }
    return solutions;
}

// Funkcja oceny rozwiązania (sumaryczny czas przetwarzania)
int evaluateSolution(const std::vector<int> &solution) {
    std::vector<int> machineEndTime(numMachines, 0);
    std::vector<int> taskEndTime(numTasks, 0);

    for (int opIndex : solution) {
        const Operation &op = operations[opIndex];
        int startTime = std::max(machineEndTime[op.machineId - 1], taskEndTime[op.taskId - 1]);
        int endTime = startTime + op.processingTime;
        machineEndTime[op.machineId - 1] = endTime;
        taskEndTime[op.taskId - 1] = endTime;
    }
    return *std::max_element(machineEndTime.begin(), machineEndTime.end());
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
    // Prosta zamiana dwóch operacji
    std::swap(improvedSolution[0], improvedSolution[1]);
    return improvedSolution;
}

// Główna funkcja harmonogramująca z użyciem Scatter Search
void scatterSearch(int rank, int size) {
    int numOperations = operations.size();
    int numSolutions = 10;

    // Generowanie początkowej populacji
    std::vector<std::vector<int>> solutions = generateInitialSolutions(numSolutions, numOperations);

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
        for (const auto &opIndex : solutions[0]) {
            const Operation &op = operations[opIndex];
            std::cout << "Task " << op.taskId << " on Machine " << op.machineId
                      << " with Processing Time " << op.processingTime << "\n";
        }
        std::cout << "Total processing time: " << evaluateSolution(solutions[0]) << "\n";
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    loadData();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Harmonogramowanie operacji
    scatterSearch(rank, size);

    MPI_Finalize();
    return 0;
}
