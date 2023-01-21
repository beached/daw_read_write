// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_io_base.h"

#include <daw/daw_always_false.h>
#include <daw/daw_string_view.h>

#include <cstddef>
#include <initializer_list>
#include <span>

namespace daw::io {
	// Base trait for WritableOutput.  Specializations must have the methods
	// declared here
	template<typename T>
	struct WritableOutput {
		[[noreturn]] static IOOpResult write( T &, daw::string_view ) {
			static_assert( daw::always_false_v<T>,
			               "WritableOutput not specialized for type" );
		}

		[[noreturn]] static IOOpResult
		write( T &, std::initializer_list<daw::string_view> ) {
			static_assert( daw::always_false_v<T>,
			               "WritableOutput not specialized for type" );
		}

		[[noreturn]] static IOOpResult write( T &, std::span<std::byte const> ) {
			static_assert( daw::always_false_v<T>,
			               "WritableOutput not specialized for type" );
		}

		[[noreturn]] static IOOpResult
		write( T &, std::initializer_list<std::span<std::byte const>> ) {
			static_assert( daw::always_false_v<T>,
			               "WritableOutput not specialized for type" );
		}

		[[noreturn]] static IOOpResult put( T &, char ) {
			static_assert( daw::always_false_v<T>,
			               "WritableOutput not specialized for type" );
		}

		[[noreturn]] static IOOpResult put( T &, std::byte ) {
			static_assert( daw::always_false_v<T>,
			               "WritableOutput not specialized for type" );
		}
	};
} // namespace daw::io
