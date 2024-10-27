/**
 * @file macros_func.h
 * @brief common macros functions
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 */
#pragma once

#define STABLE_INFRA_CHECK_SUC(expr, ret) \
    if ((!(expr))) [[unlikely]] \
    {\
        return (ret); \
    }

#define STABLE_INFRA_ASSERT(expr) \
    if (!(expr)) [[unlikely]] \
    {\
        printf("%s:%d|%s\n", __FILE__, __LINE__, __FUNCTION__);\
        std::exit(-1);\
    }

#define STABLE_INFRA_IF_TRUE_RETURN_CODE(expr, retcode) \
    if ((expr)) \
    {\
        return retcode; \
    }

#define STABLE_INFRA_IF_TRUE_RETURN(expr) \
    if ((expr)) \
    {\
        return; \
    }

#define STABLE_INFRA_DELETE_OBJ(p) if (p != nullptr) {delete p; p = nullptr;}

#define STABLE_INFRA_DELETE_ADDR(addr) \
    if (nullptr != addr) \
    {\
        delete [] addr; \
        addr = nullptr; \
    }

#define STABLE_INFRA_SAFE_CLOSE_FD(fd) \
    if ((fd) > -1) \
    {\
        ::close((fd));\
        (fd) = -1;\
    }
