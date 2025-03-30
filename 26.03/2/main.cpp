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
	
	// Размер матриц
	const int N = 500;
	std::vector<std::vector<int>> A(N, std::vector<int>(N)), B(N, std::vector<int>(N)), C(N, std::vector<int>(N, 0));

	int rows_per_process = N / size;

	if (rank == 0) {
		srand(time(nullptr));
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				A[i][j] = rand() % 10;
				B[i][j] = rand() % 10;
			}
		}
	}

	std::vector<int> localA(rows_per_process * N);
	MPI_Scatter(&A[0][0], rows_per_process * N, MPI_INT, &localA[0], rows_per_process * N, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Bcast(&B[0][0], N * N, MPI_INT, 0, MPI_COMM_WORLD);

	std::vector<int> localC(rows_per_process * N, 0);

	for (int i = 0; i < rows_per_process; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++)  localC[i * N + j] += localA[i * N + k] * B[k][j];
		}
	}

	MPI_Gather(&localC[0], rows_per_process * N, MPI_INT, &C[0][0], rows_per_process * N, MPI_INT, 0, MPI_COMM_WORLD);

	double startTime = MPI_Wtime();

	// Часть итоговой матрицы
	if (rank == 0) {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) std::cout << C[i][j] << " ";
			std::cout << std::endl;
		}

		double endTime = MPI_Wtime();
		std::cout << "Time: " << (endTime - startTime) << " s" << std::endl;
	}

	MPI_Finalize();
	return 0;
}
