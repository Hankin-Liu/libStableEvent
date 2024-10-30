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
#include <sys/socket.h>
#include <cerrno>
#include "../common/type_def.h"
#include "../util/util.h"

namespace stable_infra {
    namespace event {
#define FD_TYPE_TCP      1
#define FD_TYPE_UDP      2
#define FD_TYPE_ACCEPT   3
#define FD_TYPE_TIMER    4
#define FD_TYPE_SIGNAL   5
#define FD_TYPE_EVENT    6
#define FD_TYPE_FILE     7

        template<uint32_t TYPE>
        class fd_io_operation
        {
        };

        template<>
        class fd_io_operation<FD_TYPE_TCP>
        {
        public:
            static int32_t read_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_empty)
            {
                uint32_t result = 0;
                struct msghdr msg{};
                msg.msg_iov = iov;
                msg.msg_iovlen = iov_cnt;
                int32_t ret = 0;
                while (ret == 0) {
                    auto ret_recv = recvmsg(fd, &msg, 0);
                    if (ret_recv > 0) {
                        result += ret_recv;
                        ret = stable_infra::util::move_iov(iov, iov_cnt, ret_recv);
                        if (ret != 0) {
                            break;
                        }
                        continue;
                    } else if (ret_recv == 0) {
                        // closed
                        return 0;
                    } else {
                        if (errno == EAGAIN
                            || errno == EWOULDBLOCK) {
                            // no data in socket buffer
                            is_empty = true;
                            break;
                        } else if (errno == EINTR) {
                            continue;
                        } else {
                            // error
                            return ret_recv;
                        }
                    }
                }
                return result;
            }

            static int32_t write_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_full)
            {
                uint32_t result = 0;
                struct msghdr msg{};
                msg.msg_iov = iov;
                msg.msg_iovlen = iov_cnt;
                int32_t ret = 0;
                while (ret == 0) {
                    auto ret = sendmsg(fd, &msg, 0);
                    if (ret >= 0) {
                        result += ret;
                        ret = stable_infra::util::move_iov(iov, iov_cnt, ret);
                        if (ret != 0) {
                            break;
                        }
                        continue;
                    } else {
                        if (errno == EAGAIN
                            || errno == EWOULDBLOCK) {
                            // socket buffer has no space, release cpu to do other events
                            is_full = true;
                            break;
                        } else if (errno == EINTR) {
                            continue;
                        } else {
                            return ret;
                        }
                    }
                }
                return result;
            }
        };

        template<>
            class fd_io_operation<FD_TYPE_UDP>
            {
            public:
                static int32_t read_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_empty)
                {
                    return 0;
                }
                static int32_t write_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_full)
                {
                    return 0;
                }
            };

        template<>
            class fd_io_operation<FD_TYPE_EVENT>
            {
            public:
                static int32_t read_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_empty)
                {
                    is_empty = false;
                    while (true) {
                        auto ret = readv(fd, iov, iov_cnt);
                        if (ret == sizeof(uint64_t)) {
                            is_empty = true;
                            return ret;
                        }
                    }
                }

                // only for writing event_fd
                static int32_t write_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_full)
                {
                    is_full = false;
                    while (true) {
                        auto ret = writev(fd, iov, iov_cnt);
                        if (ret == sizeof(uint64_t)) {
                            return ret;
                        }
                    }
                }
            };

        template<>
            class fd_io_operation<FD_TYPE_FILE>
            {
            public:
                static int32_t read_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_empty)
                {
                    return 0;
                }
                static int32_t write_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_full)
                {
                    return 0;
                }
            };

    }
}
