#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cstdlib>
#include <ctime>
#include <fstream>

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

std::vector<int> combineSolutions(const std::vector<int> &sol1, const std::vector<int> &sol2) {
    std::vector<int> combined(sol1.size());
    for (size_t i = 0; i < sol1.size(); ++i) {
        combined[i] = (i % 2 == 0) ? sol1[i] : sol2[i];
    }
    return combined;
}

std::vector<int> improveSolution(const std::vector<int> &solution) {
    std::vector<int> improvedSolution = solution;
    std::swap(improvedSolution[0], improvedSolution[1]);
    return improvedSolution;
}

void scatterSearch(int rank, int size) {
    int numOperations = operations.size();
    int numSolutions = 100;

    std::vector<std::vector<int>> solutions = generateInitialSolutions(numSolutions, numOperations);

    for (int iteration = 0; iteration < 100; ++iteration) {
        std::vector<std::vector<int>> newSolutions;
        for (size_t i = 0; i < solutions.size(); ++i) {
            for (size_t j = i + 1; j < solutions.size(); ++j) {
                std::vector<int> combined = combineSolutions(solutions[i], solutions[j]);
                std::vector<int> improved = improveSolution(combined);
                newSolutions.push_back(improved);
            }
        }

        solutions.insert(solutions.end(), newSolutions.begin(), newSolutions.end());
        std::sort(solutions.begin(), solutions.end(), [](const std::vector<int> &a, const std::vector<int> &b) {
            return evaluateSolution(a) < evaluateSolution(b);
        });
        solutions.resize(numSolutions);
    }

    if (rank == 0) {
        std::cout << "Total value: " << evaluateSolution(solutions[0]) << "\n";
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
