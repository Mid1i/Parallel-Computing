#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>


int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	// Размер массива
	const int arraySize = 1000000;
	std::vector<int> data;
	
	int localSize = arraySize / size;
	std::vector<int> localData(localSize);
	
	if (rank == 0) {
		data.resize(arraySize);
		srand(time(nullptr));
		for (int i = 0; i < arraySize; i++) {
			data[i] = rand() % 100;
		}
	}
	
	double startTime = MPI_Wtime();
	MPI_Scatter(data.data(), localSize, MPI_INT, localData.data(), localSize, MPI_INT, 0, MPI_COMM_WORLD);
	
	int localSum = 0;
	for (int num : localData) {
		localSum += num;
	}
	
	int globalSum = 0;
	MPI_Reduce(&localSum, &globalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	double endTime = MPI_Wtime();
	
	if (rank == 0) {
		std::cout << "Sum: " << globalSum << std::endl;
		std::cout << "Time: " << (endTime - startTime) << " s" << std::endl;
	}
	
	MPI_Finalize();
	return 0;
}
