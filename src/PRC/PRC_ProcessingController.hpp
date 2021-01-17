/*
 * PRC_ProcessingController.hpp
 *
 *  Created on: Mar 27, 2020
 *      Author: green
 */

#ifndef PRC_PRC_PROCESSINGCONTROLLER_HPP_
#define PRC_PRC_PROCESSINGCONTROLLER_HPP_

#include <thread>
#include <mutex>
#include <condition_variable>

#include "ACQ/ACQ_CameraInput.hpp"

namespace PRC
{

	/**
	 @class CProcessingController
	 @brief Detection and Correction processing
	 **/
	class CProcessingController
	{
		public:
			/**
			 @brief constructor - start processing thread
			 */
			CProcessingController(ACQ::CCameraInput *p_cameraInput);

			/**
			 * @brief destructor - stop processing thread before destruction
			 */
			~CProcessingController();
		private:
			/**
			 * @brief processing thread
			 */
			void runProcessing();

			ACQ::CCameraInput *m_cameraInput;	// Camera interface
			std::thread m_detectionThread;		// Detection thread
			std::thread m_correctionThread;		// Correction thread
			static std::mutex m_runMutex;				// Protect the isRunning variable
			static std::condition_variable m_runCV;	// Notify update on isRunning
			bool m_isRunning;					// Control thread run
	};

} /* namespace PRC */

#endif /* PRC_PRC_PROCESSINGCONTROLLER_HPP_ */
