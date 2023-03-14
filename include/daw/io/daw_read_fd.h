// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_io_error.h"
#include "daw_io_fd_wrap.h"
#include "daw_read_base.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_likely.h>
#include <daw/daw_string_view.h>

#include <cstddef>
#include <cstdio>
#include <optional>
#include <span>

#if not __has_include( <unistd.h> )
#error fd is only supported when unistd.h is present
#endif
#include <unistd.h>

namespace daw::io {
	template<>
	struct ReadableInput<fd_wrap_t> {
		template<typename Byte>
		static IOOpResult read( fd_wrap_t fd, std::span<Byte> buff ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			if( buff.empty( ) ) {
				return { IOOpStatus::Ok, 0 };
			}
			auto result = ::read( fd.value, buff.data( ), buff.size( ) );
			if( result == buff.size( ) ) {
				return { IOOpStatus::Ok, result };
			}
			if( result == 0 ) {
				return { IOOpStatus::Eof, 0 };
			}
			// It is unspecified if the file position is set or not.  Let the caller
			// check or deal with it
			return { IOOpStatus::Error, 0 };
		}

		template<typename Byte>
		static IOOpResult get( fd_wrap_t fd, Byte &c ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			return ReadableInput::read( fd,
			                            std::span<Byte>( std::addressof( c ), 1 ) );
		}
	};
} // namespace daw::io
