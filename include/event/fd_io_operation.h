/****************************************************************************************
 * @file fd_io_operation.h
 * @brief read or write fd
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 ***************************************************************************************/
#pragma once
#include <cstdint>
#include "../common/type_def.h"

namespace stable_infra {
    namespace event {
        template<uint32_t TYPE>
        class fd_io_operation
        {
        public:
            static int32_t read_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt)
            {
                return 0;
            }
            static int32_t write_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt)
            {
                return 0;
            }
        };
    }
}
