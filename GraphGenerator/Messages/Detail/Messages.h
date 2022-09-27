#pragma once

#include <boost/asio/io_service.hpp>
#include <boost/asio/executor_work_guard.hpp>

namespace messages::detail {

	// Thread safe.
	inline auto GetMessageQueue() -> auto& {
		static auto service = boost::asio::io_service();
		static auto worker = boost::asio::make_work_guard(service); // prevent early exits.
		return service;
	}

}