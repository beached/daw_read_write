// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include <cstddef>

namespace daw::io {
	enum class IOOpStatus { Ok, Eof, Error };
	struct IOOpResult {
		IOOpStatus status = IOOpStatus::Ok;
		std::size_t count = 0;
	};
} // namespace daw::io
