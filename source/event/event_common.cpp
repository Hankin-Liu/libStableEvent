/****************************************************************************************
 * @file event_common.cpp
 * @brief common defines
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 ***************************************************************************************/
#include "../../include/common/platform_define.h"
#include "../../include/event/event_common.h"
#include "../../include/event/epoll.h"

namespace stable_infra {
    namespace event {
        std::shared_ptr<poll_base> get_poll_obj()
        {
#ifdef EVENT_EPOLL_EXIST
            return std::make_shared<stable_infra::event::epoll>();
#else
            // TODO other poller
            return nullptr;
#endif
        }
    }
}
