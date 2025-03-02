#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include <windows.h>


const int N = 1000; // Размер матриц
using Matrix = std::vector<std::vector<int>>;

// Заполнение матрицы случайными числами
void initializeMatrix(Matrix &mat) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			mat[i][j] = rand() % 100;
		}
	}
}

// Однопоточное умножение матриц
void multiplySingleThread(const Matrix &A, const Matrix &B, Matrix &C) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			for (int k = 0; k < N; ++k) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

// Умножение матриц static
void multiplyOmpStatic(const Matrix &A, const Matrix &B, Matrix &C) {
	#pragma omp parallel for schedule(static)
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			for (int k = 0; k < N; ++k) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

// Умножение матриц dynamic
void multiplyOmpDynamic(const Matrix &A, const Matrix &B, Matrix &C) {
	#pragma omp parallel for schedule(dynamic, 4)
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			for (int k = 0; k < N; ++k) {
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

int main() {
	SetConsoleOutputCP(65001);

	Matrix A(N, std::vector<int>(N)), B(N, std::vector<int>(N)), C(N, std::vector<int>(N, 0));
	initializeMatrix(A);
	initializeMatrix(B);

	// Однопоточный вариант
	auto start = std::chrono::high_resolution_clock::now();
	multiplySingleThread(A, B, C);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Однопоточный вариант: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

	// Static
	C.assign(N, std::vector<int>(N, 0));
	start = std::chrono::high_resolution_clock::now();
	multiplyOmpStatic(A, B, C);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "OpenMP static: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

	// Dynamic
	C.assign(N, std::vector<int>(N, 0));
	start = std::chrono::high_resolution_clock::now();
	multiplyOmpDynamic(A, B, C);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "OpenMP dynamic: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;
	
	return 0;
}


