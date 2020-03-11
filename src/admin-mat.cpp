//============================================================================
// Name        : admin-mat.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
//#include <opencv2/aruco.hpp>

#include <iostream>
#include <wiringPi.h>

using namespace std;

int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	// We'll start by loading an image from the drive
	cv::Mat inputImage = cv::imread((cv::String) "capture.png", cv::IMREAD_COLOR);

	// We check that our image has been correctly loaded
	if (inputImage.empty()) {
		cout << "Error: the image has been incorrectly loaded."
				<< endl;
		return 0;
	}

//	// Then we create a window to display our image
//	cv::namedWindow("My first OpenCV window");
//
//	// Finally, we display our image and ask the program to wait for a key to be pressed
//	cv::imshow("My first OpenCV window", inputImage);
//	cv::waitKey(0);

//	vector<int> markerIds;
//	vector<vector<cv::Point2f>> markerCorners, rejectedCandidates;
//	cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
//	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
//	cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);
//
//	cv::Mat outputImage = inputImage.clone();
//	cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
//
//	cv::namedWindow("Aurco Detection");
//	cv::imshow("Aruco Detection", outputImage);
	return 0;
}
