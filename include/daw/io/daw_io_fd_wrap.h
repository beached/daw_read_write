// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

namespace daw::io {
	struct fd_wrap_t {
		int value;
		constexpr fd_wrap_t( int fd ) noexcept
		  : value( fd ) {}
	};
} // namespace daw::io
