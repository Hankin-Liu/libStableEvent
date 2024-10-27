/****************************************************************************************
 * @file event_action.h
 * @brief store callback for each event
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 ***************************************************************************************/
#pragma once
#include <deque>
#include "event_common.h"
#include "../common/type_def.h"

namespace stable_infra {
    namespace event {
        using callback_t = std::function<void(int32_t)>;
        class task
        {
            public:
                task(iovec* buffer, uint32_t buffer_iov_cnt)
                    : buffer_(buffer), buffer_iov_cnt_(buffer_iov_cnt)
                {
                }
                iovec* buffer_{ nullptr };
                uint32_t buffer_iov_cnt_{ 0 };
        };
        class event_action
        {
            public:
                event_action();
                ~event_action();

                void handle_events();
                inline void set_read_callback(const callback_t& cb) {
                    events_ |= read_event_; 
                    read_callback_ = cb;
                }
                inline void set_write_callback(const callback_t& cb) {
                    events_ |= write_event_;
                    write_callback_ = cb;
                }
                inline callback_t& get_read_callback() {
                    return read_callback_;
                }
                inline callback_t& get_write_callback() {
                    return write_callback_;
                }
                inline void add_read_task(const task& t) {
                    pending_read_task_.push_back(t);
                }
                inline void add_write_task(const task& t) {
                    pending_write_task_.push_back(t);
                }
                inline bool is_readable() const {
                    return is_readable_;
                }
                inline bool is_writable() const {
                    return is_writable_;
                }
                inline void set_ready_events(uint32_t events) {
                    if (events & read_event_) {
                        is_readable_ = true;
                    }
                    if (events & write_event_) {
                        is_writable_ = true;
                    }
                }
                void set_write_callback(const callback& cb);
                void set_close_callback(const callback& cb);
                void set_error_callback(const callback& cb);
                inline int32_t events() const { return events_; }
                inline bool is_writing() const { return events_ & write_event_; }
                inline bool is_reading() const { return events_ & read_event_; }
                inline bool is_none_evt() const { return events_ == none_event_; }
                void disable_reading();
                void disable_writing();
                void disable_closing();
                void disable_error();
                void disable_all();
            private:
                static const int32_t none_event_;
                static const int32_t read_event_;
                static const int32_t write_event_;
                static const int32_t error_event_;
                static const int32_t close_event_;
                fd_t fd_{ -1 };
                int32_t events_{ 0 };
                callback_t read_callback_{nullptr};
                callback_t write_callback_{nullptr};
                callback close_callback_{nullptr};
                callback error_callback_{nullptr};
                std::deque<task> pending_read_task_{};
                std::deque<task> pending_write_task_{};
                bool is_readable_{ false };
                bool is_writable_{ false };
        };
    }
}
