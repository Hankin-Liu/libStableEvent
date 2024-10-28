/****************************************************************************************
 * @file event_action.cpp
 * @brief store callback for each event
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 ***************************************************************************************/
#include <sys/epoll.h>
#include <cstdio>
#include <cstdlib>
#include "../../include/event/event_action.h"
#include "../../include/util/macros_func.h"
#include "../../include/event/fd_io_operation.h"

namespace stable_infra {
    namespace event {
        const int32_t event_action::none_event_ = 0;
        const int32_t event_action::read_event_ = EPOLLIN | EPOLLPRI | EPOLLRDHUP;
        const int32_t event_action::write_event_ = EPOLLOUT;
        const int32_t event_action::close_event_ = EPOLLHUP;
        const int32_t event_action::error_event_ = EPOLLERR;

        event_action::event_action()
        {
        }

        event_action::~event_action()
        {
        }


        void event_action::disable_reading() 
        {
            events_ &= ~read_event_;
            read_callback_ = nullptr;
        }

        void event_action::disable_writing()
        {
            events_ &= ~write_event_; 
            write_callback_ = nullptr;
        }

        void event_action::set_close_callback(const callback& cb)
        { 
            events_ |= close_event_;
            close_callback_ = cb;
        }

        void event_action::disable_closing()
        {
            events_ &= ~close_event_; 
            close_callback_ = nullptr;
        }

        void event_action::set_error_callback(const callback& cb)
        { 
            events_ |= error_event_;
            error_callback_ = cb;
        }

        void event_action::disable_error()
        {
            events_ &= ~error_event_; 
            error_callback_ = nullptr;
        }

        void event_action::disable_all() 
        {
            events_ = none_event_; 
            read_callback_ = nullptr;
            write_callback_ = nullptr;
            close_callback_ = nullptr; 
            error_callback_ = nullptr;
        }
        
        void event_action::set_ready_events(uint32_t events)
        {
            if (! is_readable_ && events & read_event_) {
                is_readable_ = true;
            }
            if (! is_writable_ && events & write_event_) {
                is_writable_ = true;
            }
            handle_events();
        }

        void event_action::handle_events()
        {
            if (is_readable_ && ! pending_read_task_.empty()) {
                for (const auto& t : pending_read_task_) {
                    do_read_task(t);
                }
            }
            if (is_writable_ && ! pending_write_task_.empty()) {
                for (const auto& t : pending_write_task_) {
                    do_write_task(t);
                }
            }
        }

        void event_action::set_fd_type(const FD_TYPE type)
        {
            STABLE_INFRA_ASSERT(type != FD_TYPE::UNSET);
            fd_operations ops;
            switch(type) {
            case FD_TYPE::TCP_FD:
                {
                    ops.read = &fd_io_operation<FD_TYPE_TCP>::read_fd;
                    ops.write = &fd_io_operation<FD_TYPE_TCP>::write_fd;
                    break;
                }
            case FD_TYPE::UDP_FD:
                {
                    ops.read = &fd_io_operation<FD_TYPE_UDP>::read_fd;
                    ops.write = &fd_io_operation<FD_TYPE_UDP>::write_fd;
                    break;
                }
            case FD_TYPE::FILE_FD:
            case FD_TYPE::TIMER_FD:
            case FD_TYPE::SIGNAL_FD:
            case FD_TYPE::EVENT_FD:
            case FD_TYPE::ACCEPT_FD:
                {
                    ops.read = &fd_io_operation<FD_TYPE_FILE>::read_fd;
                    ops.write = &fd_io_operation<FD_TYPE_FILE>::write_fd;
                    break;
                }
            default:
                {
                }
            }
            fd_ops_ = ops;
            fd_type_ = type;
        }

        void event_action::do_read_task(const task& t)
        {
            auto ret = fd_ops_.read(fd_, t.buffer_, t.buffer_iov_cnt_);
            // TODO
        }

        void event_action::do_write_task(const task& t)
        {
            auto ret = fd_ops_.write(fd_, t.buffer_, t.buffer_iov_cnt_);
            // TODO
        }
    }
}
