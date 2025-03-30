#include <opencv2/opencv.hpp>
#include <omp.h>
#include <iostream>


void drawCarpet(cv::Mat &img, int x, int y, int size, int depth) {
    if (depth == 0) return;
    
    int newSize = size / 3;
    rectangle(img, cv::Point(x + newSize, y + newSize), cv::Point(x + 2 * newSize, y + 2 * newSize), cv::Scalar(0, 0, 0), cv::FILLED);
    
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == 1 && j == 1) continue;
            drawCarpet(img, x + i * newSize, y + j * newSize, newSize, depth - 1);
        }
    }
}

int main() {
    // Размер изображения
    int size = 729;
    // Глубина рекурсии
    int depth = 5;

    cv::Mat img(size, size, CV_8UC3, cv::Scalar(255, 255, 255));
    drawCarpet(img, 0, 0, size, depth);
    
    cv::imshow("Sierpinski Carpet", img);
    cv::waitKey(0);
    return 0;
}
