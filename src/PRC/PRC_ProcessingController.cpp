/*
 * PRC_ProcessingController.cpp
 *
 *  Created on: Mar 27, 2020
 *      Author: green
 */
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/aruco.hpp>

#include "PRC_ProcessingController.hpp"
#include "DBG/DBG_Logger.hpp"

using namespace std;

namespace PRC
{
	std::mutex CProcessingController::m_runMutex;				// Protect the isRunning variable
	std::condition_variable CProcessingController::m_runCV;	// Notify update on isRunning

	CProcessingController::CProcessingController(ACQ::CCameraInput *p_cameraInput)
	{
		m_cameraInput = p_cameraInput;
		m_isRunning = true;
		m_detectionThread = thread(&CProcessingController::runProcessing, this);
		//m_correctionThread = thread(&CProcessingController::runProcessing, this);

	}

	CProcessingController::~CProcessingController()
	{
		unique_lock<mutex> lckRun(m_runMutex);
		// Halt thread
		m_isRunning = false;
		// Notify thread that it's other
		m_detectionThread.detach();
		m_runCV.notify_all();
		// Wait end of thread
		m_runCV.wait(lckRun);
		DBG::CLogger::Warning("CProcessingController deleted");
	}

	void CProcessingController::runProcessing()
	{
		THD::CThreadSafeObject<cv::Mat> *matImg;
		unique_lock<mutex> lckRun(m_runMutex);
		m_isRunning = true;
		DBG::CLogger::Debug("Start loop in ProcessingController.");
		while (m_isRunning)
		{
			// Request shared Mat from CameraInput
			matImg = m_cameraInput->getValidStorage();
			THD::CThreadSafeObject<cv::Mat>::CReader reader(matImg);
			cv::Mat inputImage = *reader.ptr;
			//TODO Setup shared merge result storage
			// Spawn ArUco detector
			vector<int> markerIds;
			vector<vector<cv::Point2f>> markerCorners, rejectedCandidates;
			cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
			cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
			cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

			DBG::CLogger::Info("Nb Marker detected: %d", markerIds.size());

			cv::Mat outputImage = inputImage.clone();
			cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
			cv::imwrite("ArucoDetection.png", outputImage);
			//TODO spawn ArUco
			// Spawn Cup Detector
			//TODO spawn cup detector
			// Wait until detection done
			//TODO wait detection done
			//TODO setup shared result
			// Spawn correction thread
			//TODO spawn correction
			// Wait until correction done
			m_runCV.wait_for(lckRun, chrono::milliseconds(PRC_LOOP_SLEEP_TIME_MS));
		}
		m_runCV.notify_all();
	}

} /* namespace PRC */
