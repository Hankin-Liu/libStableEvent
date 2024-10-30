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
#define FD_TYPE_GENERAL   3

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
                while (true) {
                    auto ret_recv = recvmsg(fd, &msg, 0);
                    if (ret_recv > 0) {
                        result += ret_recv;
                        auto ret = stable_infra::util::move_iov(iov, iov_cnt, ret_recv);
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
                while (true) {
                    auto ret_w = sendmsg(fd, &msg, 0);
                    if (ret_w >= 0) {
                        result += ret_w;
                        auto ret = stable_infra::util::move_iov(iov, iov_cnt, ret_w);
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
                            return ret_w;
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
            class fd_io_operation<FD_TYPE_GENERAL>
            {
            public:
                static int32_t read_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_empty)
                {
                    uint32_t result = 0;
                    is_empty = false;
                    while (true) {
                        auto ret_recv = readv(fd, iov, iov_cnt);
                        if (ret_recv >= 0) {
                            result += ret_recv;
                            auto ret = stable_infra::util::move_iov(iov, iov_cnt, ret_recv);
                            if (ret != 0) {
                                break;
                            }
                            continue;
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

                // only for writing event_fd
                static int32_t write_fd(fd_t fd, ::iovec* iov, uint32_t iov_cnt, bool& is_full)
                {
                    is_full = false;
                    uint32_t result = 0;
                    while (true) {
                        auto ret_w = writev(fd, iov, iov_cnt);
                        if (ret_w >= 0) {
                            result += ret_w;
                            auto ret = stable_infra::util::move_iov(iov, iov_cnt, ret_w);
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
                                return ret_w;
                            }
                        }
                    }
                    return result;
                }
            };
    }
}
