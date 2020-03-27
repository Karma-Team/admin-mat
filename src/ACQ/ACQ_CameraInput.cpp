/*
 * ACQ_CameraInput.cpp
 *
 *  Created on: Mar 25, 2020
 *      Author: green
 */

#include "ACQ_CameraInput.hpp"

using namespace std;

namespace ACQ
{

	CCameraInput::CCameraInput()
	{
		m_readPointer = -1;
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
		//TODO update readPointer
		return &m_buffers[m_readPointer];
	}

	void CCameraInput::selectAvailableStorage()
	{
		//TODO update writePointer
	}

	void CCameraInput::cameraReader()
	{
		unique_lock<mutex> lckRun(m_runMutex);
		m_isRunning = true;
		while (m_isRunning)
		{
			// Select Available Storage
			//TODO select camera avaible storage
			// Read Camera
			//TODO read camera
			// Sleep time
			m_runCV.wait_for(lckRun, chrono::milliseconds(ACQ_LOOP_SLEEP_TIME_MS));
		}
		// Notify destructor that thread is over
		m_runCV.notify_all();
	}

} /* namespace ACQ */
