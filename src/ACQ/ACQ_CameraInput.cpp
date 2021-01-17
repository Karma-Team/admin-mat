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

	CCameraInput::CCameraInput()
	{
		m_readPointer = -1;
		m_prevWrPointer = -1;
		m_writePointer = -1;
		m_isRunning = false;
		m_thread = thread(&CCameraInput::cameraReader, this);
	}

	CCameraInput::~CCameraInput()
	{
		unique_lock<mutex> lckRun(m_runMutex);
		// Halt thread
		m_isRunning = false;
		// Notify thread that it's other
		m_runCV.notify_all();
		// Wait end of thread
		m_runCV.wait(lckRun);
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
		while (m_isRunning)
		{
			// Select Available Storage
			unique_lock<mutex> lckPtr(m_pointerMutex);
			do
			{
				m_writePointer = (m_writePointer + 1) % ACQ_BUFFER_SIZE;
			} while (m_writePointer != m_readPointer);
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
		//TODO Handle a webcam
		// Temp reader for dev
		*inputImage = cv::imread("capture.png", cv::IMREAD_COLOR);

		// We check that our image has been correctly loaded
		if (inputImage->empty()) {
			DBG::CLogger::Error("Error: the image has been incorrectly loaded.");
			return 1;
		}

		return 0;
	}

} /* namespace ACQ */
