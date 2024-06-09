#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

using namespace std;

int calculate_cost(const vector<int>& perm, const vector<int>& times, const vector<int>& weights, const vector<int>& deadlines) {
    int n = perm.size();
    vector<int> C(n, 0);
    vector<int> T(n, 0);
    int cost = 0;

    for (int i = 0; i < n; ++i) {
        if (i == 0) {
            C[i] = times[perm[i] - 1];
        } else {
            C[i] = C[i - 1] + times[perm[i] - 1];
        }
        T[i] = max(0, C[i] - deadlines[perm[i] - 1]);
        cost += weights[perm[i] - 1] * T[i];
    }
    return cost;
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    vector<int> times, weights, deadlines;
    vector<vector<int>> all_permutations;
    int n = 4;

    if (rank == 0) {
        // Example input
        times = {2, 3, 1, 4};
        weights = {4, 2, 1, 3};
        deadlines = {3, 7, 5, 6};

        // Generate all permutations
        vector<int> tasks;
        for (int i = 1; i <= n; ++i) {
            tasks.push_back(i);
        }
        do {
            all_permutations.push_back(tasks);
        } while (next_permutation(tasks.begin(), tasks.end()));
    }

    // Broadcast the problem size
    MPI_Bcast(&n, 1, MPI_INT, 0);

    // Broadcast times, weights, and deadlines
    if (rank != 0) {
        times.resize(n);
        weights.resize(n);
        deadlines.resize(n);
    }
    MPI_Bcast(times.data(), n, MPI_INT, 0);
    MPI_Bcast(weights.data(), n, MPI_INT, 0);
    MPI_Bcast(deadlines.data(), n, MPI_INT, 0);

    int chunk_size = 0;
    if (rank == 0) {
        chunk_size = all_permutations.size() / size;
    }
    MPI_Bcast(&chunk_size, 1, MPI_INT, 0);

    vector<vector<int>> local_permutations(chunk_size, vector<int>(n));
    MPI_Scatter(all_permutations.data(), chunk_size * n, MPI_INT, local_permutations.data()->data(), chunk_size * n, MPI_INT, 0);

    int local_min_cost = INT_MAX;
    vector<int> local_best_perm;

    for (const auto& perm : local_permutations) {
        int cost = calculate_cost(perm, times, weights, deadlines);
        if (cost < local_min_cost) {
            local_min_cost = cost;
            local_best_perm = perm;
        }
    }

    int global_min_cost;
    vector<int> global_best_perm(n);

    MPI_Reduce(&local_min_cost, &global_min_cost, 1, MPI_INT, MPI_MIN, 0);

    if (rank == 0) {
        int best_rank = 0;
        if (local_min_cost == global_min_cost) {
            global_best_perm = local_best_perm;
            best_rank = rank;
        }
        MPI_Bcast(&best_rank, 1, MPI_INT, 0);

        if (rank != best_rank) {
            MPI_Recv(global_best_perm.data(), n, MPI_INT, best_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        cout << "Optimal permutation: ";
        for (int i : global_best_perm) {
            cout << i << " ";
        }
        cout << "with minimum cost: " << global_min_cost << endl;
    } else {
        MPI_Bcast(&local_min_cost, 1, MPI_INT, 0);

        if (rank == local_min_cost) {
            MPI_Send(local_best_perm.data(), n, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}
