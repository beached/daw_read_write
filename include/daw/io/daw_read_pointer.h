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
		static_assert( sizeof( Byte * ) == 1 );
		static_assert( not std::is_const_v<Byte *> );

		[[nodiscard]] static IOOpResult read( Byte *&ptr, std::span<char> sp ) {
			ptr = daw::algorithm::transform_n( ptr, sp.data( ), sp.size( ),
			                                   []( auto b ) {
				                                   return static_cast<char>( b );
			                                   } )
			        .input;
			return { IOOpStatus::Ok, sp.size( ) };
		}

		[[nodiscard]] static IOOpResult read( Byte *&ptr,
		                                      std::span<std::byte> sp ) {
			ptr = daw::algorithm::transform_n( ptr, sp.data( ), sp.size( ),
			                                   []( auto b ) {
				                                   return static_cast<std::byte>( b );
			                                   } )
			        .input;
			return { IOOpStatus::Ok, sp.size( ) };
		}

		[[nodiscard]] static IOOpResult get( Byte *&ptr, char &b ) {
			b = static_cast<char>( *ptr );
			++ptr;
			return { IOOpStatus::Ok, 1 };
		}

		[[nodiscard]] static IOOpResult get( Byte *&ptr, std::byte &b ) {
			b = static_cast<std::byte>( *ptr );
			++ptr;
			return { IOOpStatus::Ok, 1 };
		}

		[[nodiscard]] static std::size_t max_peek_size( Byte *& ) {
			return 0;
		}

		template<typename B>
		[[nodiscard]] static IOOpResult peek( Byte *&, std::span<B> /*buff*/,
		                                      std::size_t /*how_much*/ ) {
			static_assert( sizeof( B ) == 1 );
			io_impl::do_io_error( "Unsupported operation" );
		}
	};
} // namespace daw::io
