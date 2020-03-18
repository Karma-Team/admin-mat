//============================================================================
// Name        : admin-mat.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/aruco.hpp>

#include <iostream>
#include <thread>
#include <wiringPi.h>

#include "TCP/TCP_Server.hpp"
#include "SSV/SSV_ReadWrite.hpp"

using namespace std;


int main() {
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	cout << "Test cmn lib - start TCP server" << endl;
	string servAddr = "0.0.0.0";
	TCP::CTcpServer l_CTcpServer(55000, servAddr);
	//l_CTcpServer = new TCP::CTcpServer(55000, servAddr);
	l_CTcpServer.initTcpServer();

	// We'll start by loading an image from the drive
	cout << "Test opencv" << endl;
	cv::Mat inputImage = cv::imread("capture.png", cv::IMREAD_COLOR);

	// We check that our image has been correctly loaded
	if (inputImage.empty()) {
		cout << "Error: the image has been incorrectly loaded."
				<< endl;
		return 1;
	}

//	// Then we create a window to display our image
//	cv::namedWindow("My first OpenCV window");
//
//	// Finally, we display our image and ask the program to wait for a key to be pressed
//	cv::imshow("My first OpenCV window", inputImage);
//	cv::waitKey(0);

	vector<int> markerIds;
	vector<vector<cv::Point2f>> markerCorners, rejectedCandidates;
	cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
	cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
	cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

	cout << "Nb Marker detected: " << markerIds.size() << endl;

	cv::Mat outputImage = inputImage.clone();
	cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);

	cv::imwrite("/home/pi/work/ArucoDetection.png", outputImage);
	cout << "End" << endl;
	return 0;
}
