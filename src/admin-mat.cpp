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
#include <string>
#include <thread>
#include <mutex>
#include <wiringPi.h>

#include "DBG/DBG_Logger.hpp"
#include "TCP/TCP_Server.hpp"
#include "SSV/SSV_ReadWrite.hpp"
#include "THD/THD_ThreadSafeObject.hpp"
#include "PRC/PRC_ProcessingController.hpp"
#include "ACQ/ACQ_CameraInput.hpp"

using namespace std;


int main() {
	DBG::CLogger::Info("Hello World!");
	DBG::CLogger::Debug("Test Debug");
	DBG::CLogger::Log("Test log");
	DBG::CLogger::Info("Test info");
	DBG::CLogger::Warning("Test warning");
	DBG::CLogger::Error("Test error!");

	DBG::CLogger::Info("Test opencv");
	// We'll start by loading an image from the drive
	cv::Mat inputImage = cv::imread("capture.png", cv::IMREAD_COLOR);

	// We check that our image has been correctly loaded
	if (inputImage.empty()) {
		DBG::CLogger::Error("Error: the image has been incorrectly loaded.");
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

	DBG::CLogger::Info("Nb Marker detected: %d", markerIds.size());

	cv::Mat outputImage = inputImage.clone();
	cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);

	cv::imwrite("/home/pi/work/ArucoDetection.png", outputImage);
	DBG::CLogger::Info("Test Cmn lib ok");


	DBG::CLogger::Info("Lancement de CProcessingController::runProcessing");
	THD::CThreadSafeObject<cv::Mat> buffers[ACQ_BUFFER_SIZE];	// Storage space
	ACQ::CCameraInput cameraInput(buffers);
	PRC::CProcessingController processingController(&cameraInput);

	DBG::CLogger::Info("Return main");
	return 0;
}
