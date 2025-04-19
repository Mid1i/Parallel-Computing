#include <mpi.h>
#include <opencv2/opencv.hpp>
#include <complex>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

float mandelbrot(double x0, double y0) {
	std::complex<double> z(0.0, 0.0);
	std::complex<double> c(x0, y0);
	
	int iter = 0;
	
	while (std::abs(z) < 2.0 && iter < 1000) {
		z = z * z + c;
		iter++;
	}

	return iter;
}

cv::Vec3b getColor(float t) {
	t = t / 1000;

	unsigned char r = static_cast<unsigned char>(255.0 * std::pow(t, 0.3));
	unsigned char g = static_cast<unsigned char>(255.0 * std::pow(t, 0.5));
	unsigned char b = static_cast<unsigned char>(255.0 * std::pow(t, 0.7));

	return cv::Vec3b(b, g, r);
}

void generateFractal(cv::Mat& image, int start, int total) {
	for (int y = 0; y < total; ++y) {
		for (int x = 0; x < 800; ++x) {
			double real = -2.5 + x * (1.0 + 2.5) / 800;
			double imag = -1.5 + (start + y) * (1.5 + 1.5) / 800;

			float iter = mandelbrot(real, imag);

			Vec3b color;

			if (iter >= 1000) color = cv::Vec3b(0, 0, 0);
			else color = getColor(iter);

			image.at<cv::Vec3b>(y, x) = color;
		}
	}
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		auto start = std::chrono::high_resolution_clock::now();

		cv::Mat serialImage(800, 800, CV_8UC3);
		generateFractal(serialImage, 0, 800);

		auto end = chrono::high_resolution_clock::now();
		double duration = std::chrono::duration<double>(end - start).count();

		std::cout << "Posled time: " << duration << " sec" << std::endl;
	}

	MPI_Barrier(MPI_COMM_WORLD); 
	int baseRows = 800 / size;
	int extraRows = 800 % size;

	int start = baseRows * rank;

	if (rank < extraRows) start += rank;
	else start += extraRows;

	int localHeight = baseRows;

	if (rank < extraRows) localHeight += 1;

	cv::Mat localImage(localHeight, 800, CV_8UC3);

	auto startMpi = std::chrono::high_resolution_clock::now();

	generateFractal(localImage, start, localHeight);

	if (rank == 0) {
		cv::Mat fullImage(800, 800, CV_8UC3);

		localImage.copyTo(fullImage(cv::Rect(0, start, 800, localHeight)));

		for (int i = 1; i < size; ++i) {
			int otherStart = baseRows * i;

			if (i < extraRows) otherStart += i;
			else otherStart += extraRows;

			int otherRows = baseRows;
			if (i < extraRows) otherRows += 1;

			int recvSize = otherRows * 800 * 3;
			std::vector<unsigned uchar> buffer(recvSize);

			MPI_Recv(buffer.data(), recvSize, MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			cv::Mat temp(otherRows, 800, CV_8UC3, buffer.data());
			temp.copyTo(fullImage(cv::Rect(0, otherStart, 800, otherRows)));
		}

		auto endMpi = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration<double>(endMpi - startMpi).count();
		std::cout << "MPI time: " << duration << " sec" << std::endl;

		cv::imwrite("mandelbrot_mpi.png", fullImage);
		cv::imshow("Mandelbrot MPI", fullImage);
		cv::waitKey(0);
	}
	else {
		int sendSize = localHeight * 800 * 3;
		MPI_Send(localImage.data, sendSize, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
