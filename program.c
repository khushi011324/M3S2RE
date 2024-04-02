#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mpi.h>

using namespace std;
using namespace std::chrono;

void randomVector(int* vector, int size) {
    for (int i = 0; i < size; i++) {
        vector[i] = rand() % 100;
    }
}

int main(int argc, char** argv) {
    unsigned long size = 100000000;
    srand(time(0));
    int* v1 = nullptr, * v2 = nullptr, * v3 = nullptr;

    MPI_Init(&argc, &argv);
    int rank, num_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Calculate the size of the vector for each process
    unsigned long local_size = size / num_procs;

    // Allocate memory for local vectors in root process
    if (rank == 0) {
        v1 = new int[size];
        v2 = new int[size];
        v3 = new int[size];
        randomVector(v1, size);
        randomVector(v2, size);
    }

    // Allocate memory for local vectors in each process
    int* local_v1 = new int[local_size];
    int* local_v2 = new int[local_size];
    int* local_v3 = new int[local_size];

    // Scatter data to all processes
    MPI_Scatter(v1, local_size, MPI_INT, local_v1, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(v2, local_size, MPI_INT, local_v2, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    auto start = high_resolution_clock::now();

    // Perform computation locally
    for (unsigned long i = 0; i < local_size; i++) {
        local_v3[i] = local_v1[i] + local_v2[i];
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    // Gather results back to the root process
    MPI_Gather(local_v3, local_size, MPI_INT, v3, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Print time taken by root process
    if (rank == 0) {
        cout << "MPI multiplication time: " << duration.count() << " microseconds" << endl;
    }

    // Deallocate memory
    if (rank == 0) {
        delete[] v1;
        delete[] v2;
        delete[] v3;
    }
    delete[] local_v1;
    delete[] local_v2;
    delete[] local_v3;

    MPI_Finalize();

    return 0;
}
