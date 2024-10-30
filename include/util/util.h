/****************************************************************************************
 * @file util.h
 * @brief common functions for general use
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 ***************************************************************************************/
#pragma once
#include <stdint.h>
#include <cassert>
#include <string>
#include <thread>
#include <string.h>
#include <vector>
#include "../common/platform_define.h"
#include "../util/macros_func.h"
#include "../common/const_variable.h"
#include "../common/type_def.h"
#if defined(_OS_LINUX)
#include <unistd.h>
#include <dirent.h>
#endif

namespace stable_infra {
    namespace util {
        int32_t util_make_fd_close_on_exec(fd_t fd);

        int32_t util_closesocket(fd_t sock);
        
        int32_t util_make_fd_nonblocking(fd_t fd);

        int32_t move_iov(::iovec*& iov, uint32_t& iov_cnt, uint32_t move_size);

        FD_TYPE get_fd_type(int fd);
    }
}
