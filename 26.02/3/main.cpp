#include <iostream>
#include <chrono>
#include <cmath>
#include <vector>
#include <omp.h>
#include <windows.h>


using Matrix = std::vector<std::vector<int>>;

// Заполнение массива случайными числами
void fillArray(std::vector<int> &arr) {
	for (auto &el : arr) {
		el = rand() % 1000;
	}
}

// Заполнение матрицы
void fillMatrix(Matrix &matrix) {
	for (auto &row : matrix) {
		for (auto &element : row) {
			element = rand() % 100;
		}
	}
}

// Умножение матрицы на вектор последовательно
void multiplySequential(const Matrix &matrix, std::vector<int> &arr) {
	size_t rows = matrix.size();
	size_t cols = matrix[0].size();
	std::vector<double> result(rows, 0.0);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			result[i] += matrix[i][j] * arr[j];
		}
	}
}

// Умножение матрицы на вектор параллельно
void multiplyParallel(const Matrix &matrix, std::vector<int> &arr) {
	size_t rows = matrix.size();
	size_t cols = matrix[0].size();
	std::vector<double> result(rows, 0.0);

	#pragma omp parallel for
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			result[i] += matrix[i][j] * arr[j];
		}
	}
}

int main() {
	SetConsoleOutputCP(65001);

	omp_set_num_threads(4);

	size_t N = 1000;

	Matrix A(N, std::vector<int>(N));
	fillMatrix(A);

	std::vector<int> arr(N);
	fillArray(arr);
    
	auto start = std::chrono::high_resolution_clock::now();
	multiplySequential(A, arr);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Последовательные вычисления: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;
	
	start = std::chrono::high_resolution_clock::now();
	multiplyParallel(A, arr);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Параллельные вычисления: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;
	
	return 0;
}
