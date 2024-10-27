/**
 * @file type_define.h
 * @brief type definition
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 */
#pragma once
#include <stdint.h>
#include "platform_define.h"

#if defined(_OS_WINDOW_64) || defined(_OS_WINDOW_32)
struct iovec
{
    void *iov_base;
    size_t iov_len;
};
#else
#include <sys/uio.h>
#endif

#if defined(_OS_WINDOW_64) || defined(_OS_WINDOW_32)
        typedef SOCKET fd_t;
#else
        typedef int32_t fd_t;
        typedef uint32_t signo_t;
#endif
