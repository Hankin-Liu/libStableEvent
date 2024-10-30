/****************************************************************************************
 * @file util.cpp
 * @brief common functions for general use
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 ***************************************************************************************/
#if !defined(_OS_WINDOW_64) && !defined(_OS_WINDOW_32)
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <netinet/tcp.h>
#endif
#include <stdio.h>
#include <chrono>
#include "../../include/util/util.h"

namespace stable_infra {
    namespace util {
        int32_t util_make_fd_close_on_exec(fd_t fd)
        {
#if !defined(_OS_WINDOW_64) && !defined(_OS_WINDOW_32)
            int flags;
            if ((flags = fcntl(fd, F_GETFD, NULL)) < 0) {
                return RET_ERR;
            }
            if (!(flags & FD_CLOEXEC)) {
                if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
                    return RET_ERR;
                }
            }
#endif
            return RET_SUC;
        }

        int32_t util_closesocket(fd_t sock)
        {
            int32_t ret = 0;
#if !defined(_OS_WINDOW_64) && !defined(_OS_WINDOW_32)
            ret = close(sock);
#else
            ret = closesocket(sock);
#endif
            if (0 != ret) {
                return RET_ERR;
            } else {
                return RET_SUC;
            }
        }

        int32_t util_make_fd_nonblocking(fd_t fd)
        {
#if !defined(_OS_WINDOW_64) && !defined(_OS_WINDOW_32)
            {
                int flags;
                if ((flags = fcntl(fd, F_GETFL, NULL)) < 0) {
                    return RET_ERR;
                }
                if (!(flags & O_NONBLOCK)) {
                    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
                        return RET_ERR;
                    }
                }
            }
#else
            {
                unsigned long nonblocking = 1;
                if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR) {
                    LOG_BASE_WARN(fd, "fcntl(%d, F_GETFL)", (int32_t)fd);
                    return RET_ERR;
                }
            }
#endif
            return RET_SUC;
        }

        int32_t move_iov(::iovec*& iov, uint32_t& iov_cnt, uint32_t move_size)
        {
            if (iov_cnt == 0) {
                return -1;
            }
            uint32_t tmp_iov_cnt = iov_cnt;
            for (uint32_t i = 0; i < tmp_iov_cnt; ++i) {
                if (move_size >= iov->iov_len) {
                    move_size -= iov->iov_len;
                    ++iov;
                    --iov_cnt;
                    continue;
                }
                iov->iov_base = (char*)iov->iov_base + move_size;
                iov->iov_len -= move_size;
                return 0;
            }
            return -1;
        }
    }
}
