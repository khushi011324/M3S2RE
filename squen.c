#include <chrono>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace std::chrono;

void randomMatrix(int* matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; ++i) {
        matrix[i] = rand() % 100;
    }
}

void matrixMultiply(int* A, int* B, int* C, int rowsA, int colsA, int colsB) {
    for (int i = 0; i < rowsA; ++i) {
        for (int j = 0; j < colsB; ++j) {
            C[i * colsB + j] = 0;
            for (int k = 0; k < colsA; ++k) {
                C[i * colsB + j] += A[i * colsA + k] * B[k * colsB + j];
            }
        }
    }
}

int main() {
    unsigned long size = 1000; // Size of matrix (size x size)
    srand(time(0));
    int *A, *B, *C;
    auto start = high_resolution_clock::now();
    A = new int[size * size];
    B = new int[size * size];
    C = new int[size * size];
    randomMatrix(A, size, size);
    randomMatrix(B, size, size);

    matrixMultiply(A, B, C, size, size, size);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Sequential multiplication time: " << duration.count() << " microseconds" << endl;

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}

