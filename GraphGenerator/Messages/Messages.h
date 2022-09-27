// Simple message passing, useful across threads.
// Soultatos Stefanos 2022

#pragma once

#include "Detail/Messages.h"

#include <type_traits>
#include <boost/asio/post.hpp>
#include <boost/asio/io_service.hpp>

namespace messages {

    // Thread safe.
    template < typename Message >
    inline void PostMessage(Message msg) {
        using boost::asio::post;
        static_assert(std::is_invocable_v< Message >);
        post(detail::GetMessageQueue(), [token = std::move(msg)]() { token(); });
    }

    // Thread safe.
    inline void PollMessage() { detail::GetMessageQueue().poll_one(); }

}
