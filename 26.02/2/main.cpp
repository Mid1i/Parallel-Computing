#include <iostream>
#include <chrono>
#include <cmath>
#include <omp.h>
#include <windows.h>


// Исходный интеграл
double f(double x) {
	return cos(x);
}

// Последовательное вычисление
double integrateSequential(int n) {
	double a = 0.0, b = 3.1415;
	double h = (b - a) / n;
	double sum = 0.0;
	
	for (int i = 0; i < n; i++) {
			double x = a + (i + 0.5) * h;
			sum += f(x);
	}
	return sum * h;
}

// Параллельное вычисление
double integrateParallel(int n) {
	double a = 0.0, b = 3.1415;
	double h = (b - a) / n;
	double sum = 0.0;

	#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < n; i++) {
			double x = a + (i + 0.5) * h;
			sum += f(x);
	}
	return sum * h;
}

int main() {
	SetConsoleOutputCP(65001);

	omp_set_num_threads(4);

	int n = 1000000;
    
	auto start = std::chrono::high_resolution_clock::now();
	double resultSeq = integrateSequential(n);
	auto end = std::chrono::high_resolution_clock::now();
	std::cout << "Последовательные вычисления: " << resultSeq << ", Время: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;
	
	start = std::chrono::high_resolution_clock::now();
	double resultPar = integrateParallel(n);
	end = std::chrono::high_resolution_clock::now();
	std::cout << "Параллельные вычисления: " << resultPar << ", Время: " << std::chrono::duration<double>(end - start).count() << "s" << std::endl;
	
	return 0;
}
