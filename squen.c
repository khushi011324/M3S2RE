
#include <chrono>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace std::chrono;

void randomVector(int* vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 100;
    }
}

int main() {
    unsigned long size = 100000000;
    srand(time(0));
    int *v1, *v2, *v3;

    auto start = high_resolution_clock::now();

    v1 = new int[size];
    v2 = new int[size];
    v3 = new int[size];
    randomVector(v1, size);
    randomVector(v2, size);

    for (int i = 0; i < size; i++) {
        v3[i] = v1[i] + v2[i];
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Sequential multiplication time: " << duration.count() << " microseconds" << endl;

    delete[] v1;
    delete[] v2;
    delete[] v3;

    return 0;
}
