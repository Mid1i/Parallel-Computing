#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <thread>
#include <chrono>

const int WIDTH = 100;
const int HEIGHT = 30;
const int STEPS = 50;

using Grid = std::vector<std::vector<int>>;

// Инициализации поля случайными значениями
void initializeGrid(Grid &grid) {
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) grid[y][x] = rand() % 2;
	}
}

// Отрисовка поля в консоли
void drawGrid(const Grid &grid) {
	system("clear");
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) std::cout << (grid[y][x] ? "#" : " ");
		std::cout << "\n";
	}
}

// Подсчёт соседей
int countNeighbors(const Grid &grid, int x, int y) {
	static const int dx[] = {-1, -1, -1, 0, 1, 1, 1, 0};
	static const int dy[] = {-1, 0, 1, 1, 1, 0, -1, -1};
	int count = 0;
	
	for (int i = 0; i < 8; ++i) {
		int nx = x + dx[i], ny = y + dy[i];
		if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT) {
			count += grid[ny][nx];
		}
	}

	return count;
}

// Обновление состояния поля с параллельными вычислениями
void updateGrid(const Grid &current, Grid &next) {
	#pragma omp parallel for collapse(2)
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			int neighbors = countNeighbors(current, x, y);
			if (current[y][x] == 1) next[y][x] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
			else next[y][x] = (neighbors == 3) ? 1 : 0;
		}
	}
}

int main() {
	srand(time(nullptr));
	Grid grid(HEIGHT, std::vector<int>(WIDTH, 0));
	Grid nextGrid = grid;

	initializeGrid(grid);

	for (int step = 0; step < STEPS; ++step) {
		auto start = std::chrono::high_resolution_clock::now();
		drawGrid(grid);
		updateGrid(grid, nextGrid);
		std::swap(grid, nextGrid);
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed = end - start;
		std::cout << "Step " << step + 1 << " Time: " << elapsed.count() << " sec\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}

