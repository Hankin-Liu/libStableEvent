/**
 * @file epoll.cpp
 * @brief encapsulation of epoll
 * @author Liu Hua Jun
 * @email wojiaoliuhuajun@126.com
 * @license Use of this source code is governed by The GNU Affero General Public License Version 3
 *          which can be found in the LICENSE file
 */
#include "../../include/common/platform_define.h"
#include <functional>
#ifdef EVENT_EPOLL_EXIST
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "../../include/event/epoll.h"
#include "../../include/event/event_common.h"
#include "../../include/event/event_action.h"
#include "../../include/util/util.h"
#include "../../include/util/macros_func.h"

namespace stable_infra {
    namespace event {
        epoll::epoll() {
            events_ptr_ = std::unique_ptr<epoll_event[]>(new epoll_event[EVENT_CNT]);
        }

        epoll::~epoll() {
            close();
        }

        bool epoll::init() {
            // First, try the new epoll_create1 interface.
            epfd_ = epoll_create1(EPOLL_CLOEXEC);
            if (epfd_ == INVALID_FD) {
                return false;
            }
            return true;
        }

        int32_t epoll::submit_async_accept(fd_t listen_fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb)
        {
            if (epfd_ == INVALID_FD || listen_fd < 0) {
                return -1;
            }
            auto evt_info_ptr = fd_to_event_info_.find(listen_fd);
            bool is_evt_exist = true;
            if (nullptr == evt_info_ptr) {
                evt_info_ptr = std::make_shared<event_info>();
                evt_info_ptr->fd_ = listen_fd;
                is_evt_exist = false;
            }
            if (cb != nullptr) {
                auto new_cb = cb.target<void(*)(int32_t)>();
                auto old_cb = evt_info_ptr->event_action_ptr_->get_read_callback().target<void(*)(int32_t)>();
                if (new_cb != old_cb) {
                    evt_info_ptr->event_action_ptr_->set_read_callback(cb);
                }
            }
            if ((evt_info_ptr->events_ & EV_READ) == 0) {
                // event changed
                evt_info_ptr->events_ |= EV_READ | EV_ET;
                if (! evt_info_ptr->is_in_change_list_) {
                    evt_change_lst_.push_back(evt_info_ptr.get());
                    evt_info_ptr->is_in_change_list_ = true;
                }
            }
            task t(buffer, buffer_iov_cnt); 
            evt_info_ptr->event_action_ptr_->add_read_task(t);
            if (! is_evt_exist) {
                STABLE_INFRA_ASSERT(fd_to_event_info_.insert(listen_fd, evt_info_ptr));
            }
            if (evt_info_ptr->event_action_ptr_->is_readable()) {
                // let epoll to trigger
                ready_events_.push_back(evt_info_ptr->event_action_ptr_.get());
            }
            return 0;
        }

        int32_t epoll::submit_async_write(fd_t fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb)
        {
            if (epfd_ == INVALID_FD || fd < 0) {
                return -1;
            }
            auto evt_info_ptr = fd_to_event_info_.find(fd);
            bool is_evt_exist = true;
            if (nullptr == evt_info_ptr) {
                evt_info_ptr = std::make_shared<event_info>();
                evt_info_ptr->fd_ = fd;
                is_evt_exist = false;
            }
            if (cb != nullptr) {
                auto new_cb = cb.target<void(*)(int32_t)>();
                auto old_cb = evt_info_ptr->event_action_ptr_->get_write_callback().target<void(*)(int32_t)>();
                if (new_cb != old_cb) {
                    evt_info_ptr->event_action_ptr_->set_write_callback(cb);
                }
            }
            if ((evt_info_ptr->events_ & EV_WRITE) == 0) {
                // event changed
                evt_info_ptr->events_ |= EV_WRITE | EV_ET;
                if (! evt_info_ptr->is_in_change_list_) {
                    evt_change_lst_.push_back(evt_info_ptr.get());
                    evt_info_ptr->is_in_change_list_ = true;
                }
            }
            task t(buffer, buffer_iov_cnt); 
            evt_info_ptr->event_action_ptr_->add_write_task(t);
            if (! is_evt_exist) {
                STABLE_INFRA_ASSERT(fd_to_event_info_.insert(fd, evt_info_ptr));
            }
            if (evt_info_ptr->event_action_ptr_->is_writable()) {
                // let epoll to trigger
                ready_events_.push_back(evt_info_ptr->event_action_ptr_.get());
            }
            return 0;
        }

        int32_t epoll::submit_async_read(fd_t fd, ::iovec* buffer, uint32_t buffer_iov_cnt, const std::function<void(int32_t)>& cb)
        {
            if (epfd_ == INVALID_FD || fd < 0) {
                return -1;
            }
            auto evt_info_ptr = fd_to_event_info_.find(fd);
            bool is_evt_exist = true;
            if (nullptr == evt_info_ptr) {
                evt_info_ptr = std::make_shared<event_info>();
                evt_info_ptr->fd_ = fd;
                is_evt_exist = false;
            }
            if (cb != nullptr) {
                auto new_cb = cb.target<void(*)(int32_t)>();
                auto old_cb = evt_info_ptr->event_action_ptr_->get_read_callback().target<void(*)(int32_t)>();
                if (new_cb != old_cb) {
                    evt_info_ptr->event_action_ptr_->set_read_callback(cb);
                }
            }
            if ((evt_info_ptr->events_ & EV_READ) == 0) {
                // event changed
                evt_info_ptr->events_ |= EV_READ | EV_ET;
                if (! evt_info_ptr->is_in_change_list_) {
                    evt_change_lst_.push_back(evt_info_ptr.get());
                    evt_info_ptr->is_in_change_list_ = true;
                }
            }
            task t(buffer, buffer_iov_cnt); 
            evt_info_ptr->event_action_ptr_->add_read_task(t);
            if (! is_evt_exist) {
                STABLE_INFRA_ASSERT(fd_to_event_info_.insert(fd, evt_info_ptr));
            }
            if (evt_info_ptr->event_action_ptr_->is_readable()) {
                // let epoll to trigger
                ready_events_.push_back(evt_info_ptr->event_action_ptr_.get());
            }
            return 0;
        }

        void epoll::apply_one_change(event_info* evt_info_ptr)
        {
            STABLE_INFRA_ASSERT(nullptr != evt_info_ptr);
            int op = EPOLL_CTL_ADD;
            auto events = evt_info_ptr->event_action_ptr_->events();
            if (0 != events) {
                if (evt_info_ptr->is_in_epoll_) {
                    op = EPOLL_CTL_MOD;
                }
            } else {
                if (evt_info_ptr->is_in_epoll_) {
                    op = EPOLL_CTL_DEL;
                } else {
                    fd_to_event_info_.erase(evt_info_ptr->fd_);
                    return;
                }
            }
            if (evt_info_ptr->events_ & EV_ET) {
                events |= EPOLLET;
            }

            struct epoll_event ep_evt;
            memset(&ep_evt, 0, sizeof(ep_evt));
            ep_evt.events = events;
            ep_evt.data.ptr = (void*)(evt_info_ptr->event_action_ptr_.get());
            if (epoll_ctl(epfd_, op, evt_info_ptr->fd_, &ep_evt) == 0) {
                if (EPOLL_CTL_DEL != op) {
                    evt_info_ptr->is_in_epoll_ = true;
                    evt_info_ptr->is_in_change_list_ = false;
                } else {
                    fd_to_event_info_.erase(evt_info_ptr->fd_);
                }
                return;
            }
            switch (op) {
                case EPOLL_CTL_MOD:
                    if (errno == ENOENT) {
                        //If a MOD operation fails with ENOENT, the fd was probably closed and re-opened.
                        //We should retry the operation as an ADD.
                        if (epoll_ctl(epfd_, EPOLL_CTL_ADD, evt_info_ptr->fd_, &ep_evt) == 0) {
                            evt_info_ptr->is_in_epoll_ = true;
                        }
                    }
                    break;
                case EPOLL_CTL_ADD:
                    if (errno == EEXIST) {
                        // If an ADD operation fails with EEXIST, either the operation was redundant
                        // (as with a precautionary add), or we ran into a fun kernel bug where using
                        // dup*() to duplicate the same file into the same fd gives you the same epitem
                        // rather than a fresh one.  For the second case, we must retry with MOD.
                        if (epoll_ctl(epfd_, EPOLL_CTL_MOD, evt_info_ptr->fd_, &ep_evt) == 0) {
                            evt_info_ptr->is_in_epoll_ = true;
                        }
                    }
                    break;
                case EPOLL_CTL_DEL:
                    if (errno == ENOENT || errno == EBADF || errno == EPERM) {
                        // If a delete fails with one of these errors, that's fine too: we closed the fd
                        // before we got around to calling epoll_dispatch.
                        fd_to_event_info_.erase(evt_info_ptr->fd_);
                        return;
                    }
                    break;
                default:
                    break;
            }

            evt_info_ptr->is_in_change_list_ = false;
        }

        void epoll::apply_changes()
        {
            for (auto& evt_info_ptr : evt_change_lst_) {
                apply_one_change(evt_info_ptr);
            }
        }

        int32_t epoll::dispatch(int32_t timeout)
        {
            if (! evt_change_lst_.empty()) {
                apply_changes();
                evt_change_lst_.clear();
            }
            if (! ready_events_.empty()) {
                timeout = 0;
            }
            auto res = epoll_wait(epfd_, events_ptr_.get(), EVENT_CNT, timeout);

            if (res == -1) {
                if (errno != EINTR) {
                    return (-1);
                }
                return (0);
            }
            STABLE_INFRA_ASSERT(res <= EVENT_CNT);

            for (auto i = 0; i < res; ++i) {
                event_action* cb = static_cast<event_action*>(events_ptr_[i].data.ptr);
                STABLE_INFRA_ASSERT(nullptr != cb);
                cb->set_ready_events(events_ptr_[i].events);
            }

            do_pending_tasks();

            return 0;
        }

        void epoll::do_read(const task& t)
        {
            //struct msghdr msg;
            //msg.msg_name = nullptr;
            //msg.msg_namelen = 0;
            //msg.msg_iov = t.buffer_;
            //msg.msg_iovlen = t.buffer_iov_cnt_;
            //msg.msg_control = nullptr;
            //msg.msg_controllen = 0;
            //msg.msg_flags = 0;
            //auto ret = recvmsg(t.fd_, &msg, 0);
            //if (ret > 0) {
            //    //shared_data_ptr_->add_weight(); // all tcp channels share the same weight
            //} else if (ret == 0) {
            //    // socket is closed
            //    close();
            //    return;
            //} else {
            //    if (errno == EAGAIN
            //            || errno == EWOULDBLOCK) {
            //        // no data in socket buffer
            //        break;
            //    } else if (errno == EINTR) {
            //        continue;
            //    } else {
            //        // socket is closed
            //        close();
            //        return;
            //    }
            //}
            //(read_callback_t*)task.cb_();
        }

        void epoll::do_pending_tasks()
        {
            uint32_t ready_cnt = ready_events_.size();
            for (uint32_t i = 0; i < ready_cnt; ++i) {
                event_action* evt_action_ptr = ready_events_.front();
                evt_action_ptr->handle_events();
                ready_events_.pop_front();
            }
        }

        void epoll::close()
        {
            if (epfd_ != INVALID_FD) {
                STABLE_INFRA_SAFE_CLOSE_FD(epfd_);
                epfd_ = INVALID_FD;
                fd_to_event_info_.clear();
                evt_change_lst_.clear();
            }
        }

        //int32_t epoll::unset(fd_t fd, uint16_t events)
        //{
        //    STABLE_INFRA_CHECK_SUC(fd != INVALID_FD, -1);
        //    auto evt_info_ptr = fd_to_event_info_.find(fd);
        //    STABLE_INFRA_CHECK_SUC(nullptr != evt_info_ptr, -1);

        //    auto cur_event = evt_info_ptr->events_;
        //    auto unset_event = cur_event & events;
        //    bool is_eff_evt = false;
        //    if (unset_event & EV_READ) {
        //        evt_info_ptr->event_action_ptr_->disable_reading();
        //        is_eff_evt = true;
        //    }
        //    if (unset_event & EV_WRITE) {
        //        evt_info_ptr->event_action_ptr_->disable_writing();
        //        is_eff_evt = true;
        //    }
        //    if (unset_event & EV_CLOSE) {
        //        evt_info_ptr->event_action_ptr_->disable_closing();
        //        is_eff_evt = true;
        //    }
        //    if (unset_event & EV_ERR) {
        //        evt_info_ptr->event_action_ptr_->disable_error();
        //        is_eff_evt = true;
        //    }
        //    if (! is_eff_evt) {
        //        return -1;
        //    }
        //    if (! evt_info_ptr->is_in_change_list_) {
        //        evt_change_lst_.push_back(evt_info_ptr.get());
        //        evt_info_ptr->is_in_change_list_ = true;
        //    }
        //    evt_info_ptr->events_ &= ~events;
        //    return 0;
        //}
    }
}
#endif
