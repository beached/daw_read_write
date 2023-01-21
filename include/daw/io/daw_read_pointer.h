// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_io_base.h"
#include "daw_io_error.h"
#include "daw_read_base.h"

#include <daw/daw_string_view.h>

#include <cstddef>
#include <optional>
#include <span>

namespace daw::io {
	template<typename Byte>
	struct ReadableInput<Byte *> {
		static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );

		template<typename B>
		[[nodiscard]] static IOOpResult read( Byte *&ptr, std::span<B> sp ) {
			static_assert( daw::traits::is_one_of_v<B, std::byte, char> );
			ptr = daw::algorithm::convert_copy_n<B>( ptr, sp.data( ), sp.size( ) );
			return { IOOpStatus::Ok, sp.size( ) };
		}

		template<typename B>
		[[nodiscard]] static IOOpResult get( Byte *&ptr, B &b ) {
			static_assert( daw::traits::is_one_of_v<B, std::byte, char> );
			b = static_cast<B>( *ptr );
			++ptr;
			return { IOOpStatus::Ok, 1 };
		}
	};
} // namespace daw::io
