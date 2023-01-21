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
#include <unistd.h>

namespace daw::io {
	template<>
	struct ReadableInput<fd_wrap_t> {
		template<typename Byte>
		static IOOpResult read( fd_wrap_t fd, std::span<Byte> buff ) {
			static_assert( sizeof( Byte ) == 1 );
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
			return ReadableInput::read( fd,
			                            std::span<Byte>( std::addressof( c ), 1 ) );
		}

		static inline std::size_t max_peek_size( FILE * ) {
			return 0;
		}

		template<typename Byte>
		[[noreturn]] DAW_ATTRIB_NOINLINE static IOOpResult
		peek( FILE *, std::span<Byte> ) {
			static_assert( sizeof( Byte ) == 1 );
			io_impl::do_io_error( "Unsupported operation" );
		}
	};
} // namespace daw::io
