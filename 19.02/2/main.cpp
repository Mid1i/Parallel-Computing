#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>


const int N = 10000;

// Заполнение массива случайными числами
void fillArray(std::vector<int> &arr) {
	for (auto &el : arr) {
		el = rand() % 1000;
	}
}

// Последовательная версия сортировки
void oddEvenTranspositionSortSequential(std::vector<int> &arr) {
	bool sorted = false;
	int n = arr.size();
	while (!sorted) {
		sorted = true;
		
		// Нечетная итерация
		for (int i = 1; i < n - 1; i += 2) {
			if (arr[i] > arr[i + 1]) {
				std::swap(arr[i], arr[i + 1]);
				sorted = false;
			}
		}

		// Четная итерация
		for (int i = 0; i < n - 1; i += 2) {
			if (arr[i] > arr[i + 1]) {
				std::swap(arr[i], arr[i + 1]);
				sorted = false;
			}
		}
	}
}

// Параллельная версия сортировки
void oddEvenTranspositionSortParallel(std::vector<int> &arr) {
	bool sorted = false;
	int n = arr.size();
	while (!sorted) {
		sorted = true;
		
		// Нечетная итерация
		#pragma omp parallel for shared(arr, sorted)
		for (int i = 1; i < n - 1; i += 2) {
			if (arr[i] > arr[i + 1]) {
				std::swap(arr[i], arr[i + 1]);
				#pragma omp critical
				sorted = false;
			}
		}

		// Четная итерация
		#pragma omp parallel for shared(arr, sorted)
		for (int i = 0; i < n - 1; i += 2) {
			if (arr[i] > arr[i + 1]) {
				std::swap(arr[i], arr[i + 1]);
				#pragma omp critical
				sorted = false;
			}
		}
	}
}

int main() {
	SetConsoleOutputCP(65001);

	omp_set_num_threads(4);

	std::vector<int> arrSequential(N);
	std::vector<int> arrParallel(N);
	
	// Заполнение массивов случайными числами
	fillArray(arrSequential);
	arrParallel = arrSequential;

	// Последовательная сортировка
	auto start = std::chrono::high_resolution_clock::now();
	oddEvenTranspositionSortSequential(arrSequential);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Последовательная сортировка: " << std::chrono::duration<double>(end - start).count() << " сек\n";
	
	// Параллельная сортировка
	start = std::chrono::high_resolution_clock::now();
	oddEvenTranspositionSortParallel(arrParallel);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Параллельная сортировка: " << std::chrono::duration<double>(end - start).count() << " сек\n";

	return 0;
}
