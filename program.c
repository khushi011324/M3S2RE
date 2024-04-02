#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

// Function to generate random matrix
void randomMatrix(int* matrix, int rows, int cols) {
    for (int i = 0; i < rows * cols; ++i) {
        matrix[i] = rand() % 100;
    }
}

// Function to multiply matrices
void matrixMultiply(int* A, int* B, int* C, int rowsA, int colsA, int colsB, int startRow, int endRow) {
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < colsB; ++j) {
            C[i * colsB + j] = 0;
            for (int k = 0; k < colsA; ++k) {
                C[i * colsB + j] += A[i * colsA + k] * B[k * colsB + j];
            }
        }
    }
}

int main(int argc, char** argv) {
    int rank, size;
    int rowsA = 1000; // Number of rows in matrix A
    int colsA = 1000; // Number of columns in matrix A
    int colsB = 1000; // Number of columns in matrix B
    int* A, * B, * C;
    int* localA, * localC;

    srand(time(NULL));

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Calculate number of rows to be processed by each process
    int rowsPerProcess = rowsA / size;
    int remainingRows = rowsA % size;

    // Allocate memory for matrices B and localC
    B = new int[colsA * colsB];
    localC = new int[rowsPerProcess * colsB];

    // Allocate memory for matrix A in root process
    if (rank == 0) {
        A = new int[rowsA * colsA];
        randomMatrix(A, rowsA, colsA);
        // Allocate memory for receive buffer in root process
        C = new int[rowsA * colsB];
    }

    // Allocate memory for localA based on rank
    localA = new int[rowsPerProcess * colsA];

    // Scatter rows of matrix A to all processes
    MPI_Scatter(A, rowsPerProcess * colsA, MPI_INT, localA, rowsPerProcess * colsA, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcast matrix B to all processes
    MPI_Bcast(B, colsA * colsB, MPI_INT, 0, MPI_COMM_WORLD);

    auto start = high_resolution_clock::now();

    // Perform matrix multiplication
    matrixMultiply(localA, B, localC, rowsA, colsA, colsB, 0, rowsPerProcess);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    // Gather the results from all processes to rank 0
    MPI_Gather(localC, rowsPerProcess * colsB, MPI_INT, C, rowsPerProcess * colsB, MPI_INT, 0, MPI_COMM_WORLD);

    // Print the time taken for matrix multiplication
    if (rank == 0) {
        cout << "Time taken for matrix multiplication: " << duration.count() << " microseconds" << endl;
    }

    // Deallocate memory
    delete[] A;
    delete[] B;
    delete[] localA;
    delete[] localC;
    if (rank == 0) {
        delete[] C;
    }

    MPI_Finalize();
    return 0;
}
