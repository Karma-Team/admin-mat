/*
 * ACQ_CameraInput.hpp
 *
 *  Created on: Mar 25, 2020
 *      Author: green
 */

#ifndef INC_CAMERAINPUT_HPP_
#define INC_CAMERAINPUT_HPP_

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <opencv2/core.hpp>
#include "THD/THD_ThreadSafeObject.hpp"

#define ACQ_BUFFER_SIZE 3
#define ACQ_LOOP_SLEEP_TIME_MS 10

namespace ACQ
{

	/**
	 @class CCameraInput
	 @brief Control camera input
	 **/
	class CCameraInput
	{
		public:
			/**
			 @brief constructor - start camera thread
			 */
			CCameraInput();

			/**
			 * @brief destructor - stop camera thread before destruction
			 */
			~CCameraInput();

			/**
			 * @brief get a pointer to a saved camera Mat - null if none available
			 * Process:
			 *   The Mat buffer work with three spaces											|  |  |  |
			 *   The write operation is done in one												|Wr|  |  |
			 *   The read operation is done in another											|Wr|Rd|  |
			 *   When over, the prevWrPointer is set on this space.								|PW|Rd|  |
			 *   The next write operation will be done on the next space not used for read		|PW|Rd|Wr|
			 *   The next read operation will be done in the more recent write space: prevWr	|Rd|  |Wr|
			 *   Etc...																			|Rd|Wr|PW|
			 * If a read is over before a new write, we wait until a new Wr |Wr|Rd|  | -> |Wr|  |  | -> |PW||  | -> |Rd|  |  |
			 */
			THD::CThreadSafeObject<cv::Mat>* getValidStorage();

		private:
			/**
			 * @brief camera thread
			 */
			void cameraReader();

			/**
			 * @brief camera access
			 */
			int getCameraImage(cv::Mat* inputImage);

			THD::CThreadSafeObject<cv::Mat> m_buffers[ACQ_BUFFER_SIZE];	// Storage space
			std::mutex m_pointerMutex;									// Protect the access to read and write pointer
			std::condition_variable m_updatePtr;						// Notify update on write pointer
			int m_readPointer;											// Read pointer in storage space
			int m_writePointer;											// Write pointer
			int m_prevWrPointer;										// Previous write pointer in storage space
			std::thread m_thread;										// Camera input thread
			std::mutex m_runMutex;										// Protect the isRunning variable
			std::condition_variable m_runCV;							// Notify update on isRunning
			bool m_isRunning;											// Control thread run
	};

} /* namespace ACQ */

#endif /* INC_CAMERAINPUT_HPP_ */
