/****************************************************************************************
 * @file event_common.h
 * @brief common defines
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 ***************************************************************************************/
#pragma once
#include <functional>
#include <memory>

namespace stable_infra {
    namespace event {
        typedef std::function<void(void)> callback;
        class iovec;
        typedef std::function<void(iovec*, uint32_t)> read_callback;

        typedef std::function<void(void)> pending_func;

        /*
         * @breif get one io multiplexing object, such as epoll, poll, select, iocp
         * @return io multiplexing object pointer
         */
        class poll_base;
        std::shared_ptr<poll_base> get_poll_obj();
    }
}
