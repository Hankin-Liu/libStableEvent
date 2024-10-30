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
                auto size = pending_read_task_.size();
                for (uint32_t i = 0; i < size; ++i) {
                    auto& t = pending_read_task_.front();
                    auto ret = do_read_task(t);
                    if (ret == INT32_MAX) {
                        is_readable_ = false;
                        pending_read_task_.push_back(t);
                    }
                    pending_read_task_.pop_front();
                }
            }
            if (is_writable_ && ! pending_write_task_.empty()) {
                auto size = pending_write_task_.size();
                for (uint32_t i = 0; i < size; ++i) {
                    auto& t = pending_write_task_.front();
                    auto ret = do_write_task(t);
                    if (ret == INT32_MAX) {
                        is_writable_ = false;
                        pending_write_task_.push_back(t);
                    }
                    pending_write_task_.pop_front();
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
                {
                    ops.read = &fd_io_operation<FD_TYPE_FILE>::read_fd;
                    ops.write = &fd_io_operation<FD_TYPE_FILE>::write_fd;
                    break;
                }
            case FD_TYPE::TIMER_FD:
            case FD_TYPE::SIGNAL_FD:
            case FD_TYPE::EVENT_FD:
            case FD_TYPE::ACCEPT_FD:
                {
                    ops.read = &fd_io_operation<FD_TYPE_EVENT>::read_fd;
                    ops.write = &fd_io_operation<FD_TYPE_EVENT>::write_fd;
                    break;
                }
            default:
                {
                }
            }
            fd_ops_ = ops;
            fd_type_ = type;
        }

        int32_t event_action::do_read_task(const task& t)
        {
            if (STABLE_INFRA_UNLIKELY(t.buffer_iov_cnt_ > read_iov_buffer_.size())) {
                read_iov_buffer_.resize(t.buffer_iov_cnt_);
            }
            memcpy((void*)read_iov_buffer_.data(), t.buffer_, sizeof(::iovec) * t.buffer_iov_cnt_);
            bool is_empty = false;
            auto ret = fd_ops_.read(fd_, read_iov_buffer_.data(), t.buffer_iov_cnt_, is_empty);
            STABLE_INFRA_IF_TRUE_RETURN_CODE(is_empty && ret == 0, INT32_MAX);
            read_callback_(ret);
            return 0;
        }

        int32_t event_action::do_write_task(const task& t)
        {
            if (STABLE_INFRA_UNLIKELY(t.buffer_iov_cnt_ > write_iov_buffer_.size())) {
                read_iov_buffer_.resize(t.buffer_iov_cnt_);
            }
            memcpy((void*)write_iov_buffer_.data(), t.buffer_, sizeof(::iovec) * t.buffer_iov_cnt_);
            bool is_full = false;
            auto ret = fd_ops_.write(fd_, write_iov_buffer_.data(), t.buffer_iov_cnt_, is_full);
            STABLE_INFRA_IF_TRUE_RETURN_CODE(is_full && ret == 0, INT32_MAX);
            read_callback_(ret);
            return 0;
        }
    }
}
