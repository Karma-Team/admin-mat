/*
 * PRC_ProcessingController.cpp
 *
 *  Created on: Mar 27, 2020
 *      Author: green
 */

#include "PRC_ProcessingController.hpp"

using namespace std;

namespace PRC
{

	CProcessingController::CProcessingController(ACQ::CCameraInput &p_cameraInput)
	{
		m_cameraInput = p_cameraInput;
		m_isRunning = true;
		m_detectionThread = thread(&CProcessingController::runProcessing, this);
		m_correctionThread = thread(&CProcessingController::runProcessing, this);

	}

	CProcessingController::~CProcessingController()
	{
		unique_lock<mutex> lckRun(m_runMutex);
		// Halt thread
		m_isRunning = false;
		// Notify thread that it's other
		m_runCV.notify_all();
		// Wait end of thread
		m_runCV.wait(lckRun);
	}

	void CProcessingController::runProcessing()
	{
		unique_lock<mutex> lckRun(m_runMutex); //TODO deadlock avec le destructeur
		m_isRunning = true;
		while (m_isRunning)
		{
			// Request shared Mat from CameraInput
			//TODO request access
			//TODO Setup shared merge result storage
			// Spawn ArUco detector
			//TODO spawn ArUco
			// Spawn Cup Detector
			//TODO spawn cup detector
			// Wait until detection done
			//TODO wait detection done
			//TODO setup shared result
			// Spawn correction thread
			//TODO spawn correction
			// Wait until correction done
		}
	}

} /* namespace PRC */
