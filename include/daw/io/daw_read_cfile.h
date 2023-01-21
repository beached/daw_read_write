// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_io_error.h"
#include "daw_read_base.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_likely.h>
#include <daw/daw_string_view.h>

#include <cstddef>
#include <cstdio>
#include <optional>
#include <span>

namespace daw::io {
	template<>
	struct ReadableInput<FILE *> {
		template<typename Byte>
		static IOOpResult read( FILE *fp, std::span<Byte> buff ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			auto result = std::fread( buff.data( ), 1, buff.size( ), fp );
			if( std::feof( fp ) ) {
				return { IOOpStatus::Eof, result };
			}
			if( result == buff.size( ) ) {
				return { IOOpStatus::Ok, result };
			}
			if( DAW_LIKELY( std::ferror( fp ) ) ) {
				return { IOOpStatus::Error, result };
			}
			// What does this mean?
			io_impl::do_io_error(
			  "Unexpected state, unread bytes but eof not set and error not set in "
			  "FILE*" );
		}

		template<typename Byte>
		static IOOpResult get( FILE *fp, Byte &c ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			auto result = std::fgetc( fp );
			if( result >= 0 ) {
				c = static_cast<Byte>( result );
				return { IOOpStatus::Ok, 1 };
			}
			if( std::feof( fp ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			if( DAW_LIKELY( std::ferror( fp ) ) ) {
				return { IOOpStatus::Error, 0 };
			}
			// What does this mean?
			io_impl::do_io_error(
			  "Unexpected state, unread bytes but eof not set and error not set in "
			  "FILE*" );
		}
	};
} // namespace daw::io
