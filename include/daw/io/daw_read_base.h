// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_read
//

#pragma once

#include "daw_io_base.h"

#include <daw/daw_always_false.h>
#include <daw/daw_string_view.h>

#include <cstddef>
#include <optional>
#include <span>

namespace daw::io {
	// Base trait for ReadableInput.  Specializations must have the methods
	// declared here
	template<typename T>
	struct ReadableInput {
		[[noreturn]] static IOOpResult read( T &, std::span<char> ) {
			static_assert( daw::always_false_v<T>,
			               "ReadableInput not specialized for type" );
		}

		[[noreturn]] static IOOpResult read( T &, std::span<std::byte> ) {
			static_assert( daw::always_false_v<T>,
			               "ReadableInput not specialized for type" );
		}

		[[noreturn]] static IOOpResult get( T &, char & ) {
			static_assert( daw::always_false_v<T>,
			               "ReadableInput not specialized for type" );
		}

		[[noreturn]] static IOOpResult get( T &, std::byte & ) {
			static_assert( daw::always_false_v<T>,
			               "ReadableInput not specialized for type" );
		}

		[[noreturn]] static std::size_t max_peek_size( T & ) {
			static_assert( daw::always_false_v<T>,
			               "ReadableInput not specialized for type" );
		}

		[[noreturn]] static IOOpResult peek( T &, std::span<char> /*buff*/,
		                                       std::size_t /*how_much*/ ) {
			static_assert( daw::always_false_v<T>,
			               "ReadableInput not specialized for type" );
		}

		[[noreturn]] static IOOpResult peek( T &,
		                                       std::span<std::byte> /*buff*/ ) {
			static_assert( daw::always_false_v<T>,
			               "ReadableInput not specialized for type" );
		}
	};
} // namespace daw::io
