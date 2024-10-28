/**
 * @file poll_base.h
 * @brief base class or variables of io multiplexing mechanism
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 */
#pragma once
#include <stdint.h>
#include <functional>
#include "event_common.h"
#include "../common/type_def.h"

/**
 * @brief stable_infra function namespace
 */
namespace stable_infra {
    /**
     * @brief event namespace
     * All event driven codes are in this namespace
     */
    namespace event {
        /**
         * @brief interface class for io multiplexing mechanism
         * Use pure virtual functions to define interfaces
         */
        class poll_base
        {
            public:
                /**
                 * @brief init interface
                 * Initialization of io multiplexing mechanism
                 * @return result of Initialization
                 * @retval true successful
                 * @retval false failed
                 */
                virtual bool init() = 0;
                /**
                 * @brief set up event interface
                 * Set up event which concerned in io multiplexing mechanism
                 * @param[in] fd file discriptor
                 * @param[in] events events on fd which concern about
                 * @param[in] cb callback function for events
                 * @param[in] buffer buffer for receive data from this fd
                 * @param[in] buffer_iov_cnt count of iovec buffer
                 * @return result of seting up
                 * @retval 0 successful
                 * @retval -1 failed
                 */
                //virtual int32_t set(fd_t fd, uint16_t events, const stable_infra::event::callback& cb) = 0;

                virtual int32_t submit_async_read(fd_t fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb) = 0;

                virtual int32_t submit_async_accept(fd_t listen_fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb) = 0;

                virtual int32_t submit_async_write(fd_t fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb) = 0;

                /**
                 * @brief Dispatch event interface
                 * Dispatch events and invoke callback functions
                 * @return result of dispatching
                 * @retval 0 successful
                 * @retval -1 failed
                 */
                virtual int32_t dispatch(int32_t timeout) = 0;
                /**
                 * @brief Close interface
                 * Close this io multiplexing mechanism
                 */
                virtual void close() = 0;
        };
    }
}
