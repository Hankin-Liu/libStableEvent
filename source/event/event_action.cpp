/****************************************************************************************
 * @file event_action.cpp
 * @brief store callback for each event
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 ***************************************************************************************/
#include <sys/epoll.h>
#include "../../include/event/event_action.h"

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

        void event_action::handle_events()
        {
            //if ((events & close_event_) && !(events & read_event_)) {
            //    if (close_callback_ != nullptr) {
            //        close_callback_();
            //    }
            //}
            //if (events & error_event_) {
            //    if (error_callback_ != nullptr) {
            //        error_callback_();
            //    }
            //}
            //if (events & read_event_) {
            //    if (read_callback_ != nullptr) {
            //        read_callback_(ret);
            //    }
            //}
            //if (events & write_event_) {
            //    if (write_callback_ != nullptr) {
            //        write_callback_(ret);
            //    }
            //}
        }
    }
}
