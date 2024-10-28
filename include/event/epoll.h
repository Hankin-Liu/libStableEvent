/**
 * @file epoll.h
 * @brief encapsulation of epoll
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 */
#pragma once
#include "../common/platform_define.h"
#ifdef EVENT_EPOLL_EXIST
#include <memory>
#include <sys/epoll.h>
#include <deque>
#include <list>
#include "poll_base.h"
#include "../data_struct/opt_map.h"
#include "../common/const_variable.h"
#include "event_action.h"

/// event count receive from epoll once
#define EVENT_CNT 1024

/// max fd which can be optimized 
#define MAX_FD 100000

#if !defined(EPOLL_CLOEXEC)
/// Flags for epoll_create1
#define EPOLL_CLOEXEC O_CLOEXEC
#endif

/**
 * @brief stable_infra namespace
 */
namespace stable_infra {
    /**
     * @brief event namespace
     * All event driven codes are in this namespace
     */
    namespace event {
        /**
         * @brief event
         */
        enum class EVENT : uint16_t
        {
            EV_READ = 0x1,  ///< read event
#define EV_READ (uint16_t)(stable_infra::event::EVENT::EV_READ)
            EV_WRITE = 0X2, ///< write event
#define EV_WRITE (uint16_t)(stable_infra::event::EVENT::EV_WRITE)
            EV_CLOSE = 0x4, ///< close event
#define EV_CLOSE (uint16_t)(stable_infra::event::EVENT::EV_CLOSE)
            EV_ERR = 0X8,   ///< error event
#define EV_ERR (uint16_t)(stable_infra::event::EVENT::EV_ERR)
            EV_ET = 0x10,   ///< ET mode
#define EV_ET (uint16_t)(stable_infra::event::EVENT::EV_ET)
        };

        class event_action;
        /**
         * @brief event information
         */
        class event_info
        {
            public:
                using pointer_t = std::shared_ptr<event_info>;
                /**
                 * @brief construction function
                 */
                event_info()
                {
                    event_action_ptr_ = std::make_shared<event_action>();
                }
            public:
                fd_t fd_{ -1 };                                     ///< file discriptor
                uint16_t events_{ 0 };                                      ///< set changed event
                bool is_in_epoll_{ false };                                 ///< if this fd is in epoll
                bool is_in_change_list_{ false };                           ///< if this event is in change list
                std::shared_ptr<event_action> event_action_ptr_{ nullptr }; ///< event action pointer
        };

        /**
         * @brief encapsulation of epoll mechanism
         */
        class epoll : public poll_base
        {
            public:
                /**
                 * @brief construction function
                 */
                epoll();
                /**
                 * @brief destruction function
                 */
                virtual ~epoll();
            public:
                /**
                 * @brief get epoll fd
                 * @return epoll fd
                 */
                virtual inline fd_t get_epfd() const { return epfd_; }
                /**
                 * @brief init epoll
                 * @return result of Initialization
                 * @retval true successful
                 * @retval false failed
                 */
                virtual bool init() override;
                /**
                 * @brief Dispatch event
                 * Dispatch events and invoke callback functions
                 * @return result of dispatching
                 * @retval 0 successful
                 * @retval -1 failed
                 */
                virtual int32_t dispatch(int32_t timeout) override;
                /**
                 * @brief Close interface
                 * Close this epoll
                 */
                virtual void close() override;

                virtual int32_t submit_async_read(fd_t fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb) override;

                virtual int32_t submit_async_accept(fd_t listen_fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb) override;

                virtual int32_t submit_async_write(fd_t fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb) override;
            private:
                /**
                 * @brief make changed event effective
                 */
                void apply_changes();
                /**
                 * @brief make changed event effective for one fd
                 * @param evt_info_ptr event_info object for one fd
                 */
                void apply_one_change(event_info* evt_info_ptr);
                void do_pending_tasks();
                void do_read(const task& t);
            private:
                std::unique_ptr<epoll_event[]> events_ptr_; ///< used for receive active events
                fd_t epfd_{ INVALID_FD }; ///< epoll fd
                /**< event information for each fd */
                stable_infra::data_struct::opt_map<event_info::pointer_t, uint32_t, MAX_FD> fd_to_event_info_;
                std::list<event_info*> evt_change_lst_; ///< event_info which has been changed
                int32_t errno_{ 0 };
                std::deque<event_action*> ready_events_{};
        };
    }
}
#endif
