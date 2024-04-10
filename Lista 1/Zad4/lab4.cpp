#include <iostream>
#include <vector>
#include <thread>
#include <cmath>

using namespace std;

double calcPartialSum(int start, int end) {
    double partial_sum = 0;
    for (int i = start; i <= end; i++) {
        partial_sum += 1.0 / i;
    }
    return partial_sum;
}

void calcPartialSumLink(int start, int end, double& result) {
    result = calcPartialSum(start, end);
}

double calculate_gamma(int n, int p) {
    vector<thread> threads;
    vector<double> partialSums(p);

    for (int i = 0; i < p; i++) {
        int start = 1 + (n / p) * i;
        int end = std::min(n, (n / p) * (i + 1));
        threads.emplace_back(calcPartialSumLink, start, end, ref(partialSums[i]));
    }

    for (auto& thread : threads) {
        thread.join();
    }

    double sum = 0;
    for (auto temp : partialSums) {
        sum += temp;
    }

    double gamma = sum - log(n);

    return gamma;
}

int main() {
    int n, p;
    cin >> n >> p;

    double gamma = calculate_gamma(n, p);
    cout << "Gamma: " << gamma << "\n";
}