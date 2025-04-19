#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <vector>

void smiles(cv::Mat& faceROI_gray, cv::CascadeClassifier& smile_cascade, std::vector<cv::Rect>& smiles) {
	smile_cascade.detectMultiScale(faceROI_gray, smiles, 1.24, 15, 0, cv::Size(40, 40));
}

void eyes(cv::Mat& faceROI_gray, cv::CascadeClassifier& eyes_cascade, std::vector<cv::Rect>& eyes) {
	eyes_cascade.detectMultiScale(faceROI_gray, eyes, 1.1, 10, 0, cv::Size(50, 50));
}

int main() {
	cv::CascadeClassifier face_cascade, eyes_cascade, smile_cascade;

	if (
			!face_cascade.load("./haarcascade_frontalface_default.xml") ||
			!eyes_cascade.load("./haarcascade_eye.xml") ||
			!smile_cascade.load("./haarcascade_smile.xml")
	) {
			std::cerr << "fail to load haar cascade!" << std::endl;
			return -1;
	}

	cv::VideoCapture cap("./ZUA.mp4");

	if (!cap.isOpened()) {
			std::cerr << "fail to load video!" << std::endl;
			return -1;
	}

	cv::Mat frame;
	while (cap.read(frame)) {
			if (frame.empty()) break;

			cv::Mat gray;
			cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
			cv::equalizeHist(gray, gray);
			cv::GaussianBlur(gray, gray, cv::Size(5, 5), 0);

			std::vector<cv::Rect> faces;
			face_cascade.detectMultiScale(gray, faces, 1.1, 5, 0, cv::Size(150, 150));

			for (const auto& face : faces) {
					cv::rectangle(frame, face, cv::Scalar(255, 0, 0), 2);

					cv::Mat faceROI_gray = gray(face);
					cv::Mat faceROI_color = frame(face);

					std::vector<cv::Rect> eyes, smiles;

					std::thread eyesThread(eyes, std::ref(faceROI_gray), std::ref(eyes_cascade), std::ref(eyes));
					std::thread smilesThread(smiles, std::ref(faceROI_gray), std::ref(smile_cascade), std::ref(smiles));

					eyesThread.join();
					smilesThread.join();

					for (const auto& eye : eyes) {
							cv::rectangle(faceROI_color, eye, cv::Scalar(0, 0, 255), 2);
					}

					for (const auto& smile : smiles) {
							cv::rectangle(faceROI_color, smile, cv::Scalar(0, 255, 0), 2);
					}
			}

			cv::imshow("face detect", frame);
			if (cv::waitKey(30) >= 0) break;
	}

	cap.release();
	cv::destroyAllWindows();
	return 0;
}
