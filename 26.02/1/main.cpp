#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
#include <windows.h>


const int arraySize = 100000000;

// Заполнение массива случайными числами
void fillArray(std::vector<int> &arr) {
	for (auto &el : arr) {
		el = rand() % 1000;
	}
}

// Последовательное вычисление суммы
long long sequentialSum(const std::vector<int>& arr) {
	long long sum = 0;
	for (int i = 0; i < arr.size(); ++i) {
		sum += arr[i];
	}

	return sum;
}

// Параллельное вычисление суммы
long long parallelSum(const std::vector<int>& arr) {
	long long sum = 0;
	#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < arr.size(); ++i) {
		sum += arr[i];
	}
	return sum;
}

int main() {
	SetConsoleOutputCP(65001);

	omp_set_num_threads(4);

	std::vector<int> arr(arraySize);
	
	// Заполнение массива случайными числами
	fillArray(arr);

	// Параллельное вычисление
	auto start = std::chrono::high_resolution_clock::now();
	long long parSum = parallelSum(arr);
	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Сумма при параллельном вычислении: " << parSum << std::endl;
	std::cout << "Параллельное вычисление: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;

	// Последовательное вычисление
	start = std::chrono::high_resolution_clock::now();
	long long seqSum = sequentialSum(arr);
	end = std::chrono::high_resolution_clock::now();

	std::cout << "Сумма при последовательном вычислении: " << seqSum << std::endl;
	std::cout << "Последовательное вычисление: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl << std::endl;

	return 0;
}
