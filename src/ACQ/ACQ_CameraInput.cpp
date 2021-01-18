/*
 * ACQ_CameraInput.cpp
 *
 *  Created on: Mar 25, 2020
 *      Author: green
 */

#include "ACQ_CameraInput.hpp"

#include "DBG/DBG_Logger.hpp"

using namespace std;

namespace ACQ
{
	std::mutex CCameraInput::m_pointerMutex;								// Protect the access to read and write pointer
	std::condition_variable CCameraInput::m_updatePtr;						// Notify update on write pointer
	std::mutex CCameraInput::m_runMutex;									// Protect the isRunning variable
	std::condition_variable CCameraInput::m_runCV;							// Notify update on isRunning

	CCameraInput::CCameraInput(THD::CThreadSafeObject<cv::Mat> buffers[ACQ_BUFFER_SIZE])
	{
		m_readPointer = -1;
		m_prevWrPointer = -1;
		m_writePointer = -1;
		m_isRunning = false;
		m_buffers = buffers;
		m_thread = thread(&CCameraInput::cameraReader, this);
	}

	CCameraInput::~CCameraInput()
	{
		unique_lock<mutex> lckRun(m_runMutex);
		// Halt thread
		m_isRunning = false;
		// Notify thread that it's other
		m_thread.detach();
		m_runCV.notify_all();
		// Wait end of thread
		m_runCV.wait(lckRun);
#if NO_WEBCAM == 0
		// Release camera
		m_camera.release();
#endif
		DBG::CLogger::Warning("CCameraInput deleted");
	}

	THD::CThreadSafeObject<cv::Mat>* CCameraInput::getValidStorage()
	{
		unique_lock<mutex> lckPtr(m_pointerMutex);
		// Wait until new write
		while (m_prevWrPointer == m_readPointer)
			m_updatePtr.wait(lckPtr);
		m_readPointer = m_prevWrPointer;
		return &m_buffers[m_readPointer];
	}

	void CCameraInput::cameraReader()
	{
		int res;
		unique_lock<mutex> lckRun(m_runMutex);
		m_isRunning = true;

#if NO_WEBCAM == 0
		// https://docs.opencv.org/4.4.0/d8/dfe/classcv_1_1VideoCapture.html
		DBG::CLogger::Info("CCameraInput::cameraReader: Start webcam");
		int deviceID = 0;             // 0 = open default camera
		int apiID = cv::CAP_ANY;      // 0 = autodetect default API
		m_camera.open(deviceID, apiID);
		if (m_camera.isOpened())
		{
			DBG::CLogger::Info("Camera open OK! %d", m_camera.isOpened());
		}
		else
		{
			DBG::CLogger::Error("Camera open failed");
		}
#endif

		DBG::CLogger::Debug("CCameraInput::cameraReader: Start loop");
		while (m_isRunning)
		{
			// Select Available Storage
			unique_lock<mutex> lckPtr(m_pointerMutex);
			do
			{
				m_writePointer = (m_writePointer + 1) % ACQ_BUFFER_SIZE;
			} while (m_writePointer == m_readPointer);
			THD::CThreadSafeObject<cv::Mat>::CWriter writer(&m_buffers[m_writePointer]);
			lckPtr.unlock();
			// Read Camera
			cv::Mat *writePtr = writer.ptr;
			res = getCameraImage(writePtr);
			if (res) continue;
			// Update prevWr pointer
			lckPtr.lock();
			m_prevWrPointer = m_writePointer;
			m_updatePtr.notify_all();
			lckPtr.unlock();
			// Sleep time
			m_runCV.wait_for(lckRun, chrono::milliseconds(ACQ_LOOP_SLEEP_TIME_MS));
		}
		// Notify destructor that thread is over
		m_runCV.notify_all();
	}

	int CCameraInput::getCameraImage(cv::Mat* inputImage)
	{
		cv::Mat camImage;
#if NO_WEBCAM
		*inputImage = cv::imread("capture.png", cv::IMREAD_COLOR);
#else
		m_camera.read(camImage);
		*inputImage = camImage.clone();
#endif

		// We check that our image has been correctly loaded
		if (inputImage->empty()) {
			DBG::CLogger::Error("Error: the image has been incorrectly loaded (isOpened: %d).", m_camera.isOpened());
			return 1;
		}

		return 0;
	}

} /* namespace ACQ */
