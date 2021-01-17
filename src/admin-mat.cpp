//============================================================================
// Name        : admin-mat.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
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

	{
		// Create scope to kill threads at the end of it
		std::mutex runMutex;
		std::condition_variable runMain;
		unique_lock<mutex> lckRun(runMutex);

		DBG::CLogger::Info("Lancement de CProcessingController::runProcessing");
		THD::CThreadSafeObject<cv::Mat> buffers[ACQ_BUFFER_SIZE];	// Storage space
		ACQ::CCameraInput cameraInput(buffers);
		PRC::CProcessingController processingController(&cameraInput);

		DBG::CLogger::Debug("Timer");
		runMain.wait_for(lckRun, chrono::seconds(5));
		DBG::CLogger::Debug("End processes");
	}

	DBG::CLogger::Info("Return main");
	return 0;
}
