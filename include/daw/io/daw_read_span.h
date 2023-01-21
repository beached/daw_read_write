// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_read_base.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_string_view.h>

#include <cstddef>
#include <optional>
#include <span>

namespace daw::io {
	template<typename CharT, std::size_t Extent>
	struct ReadableInput<std::span<CharT, Extent>> {
		static_assert( Extent == std::dynamic_extent,
		               "Only dynamic extent span's are supported" );
		using value_type = std::span<CharT, Extent>;

		template<typename Byte>
		static constexpr IOOpResult read( value_type &sp, std::span<Byte> buff ) {
			if( sp.empty( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto sz = std::min( buff.size( ), sp.size( ) );
			auto const to_write = sp.subspan( 0, sz );
			sp = sp.subspan( sz );
			(void)daw::algorithm::transform_n( to_write.data( ), buff.data( ),
			                                   to_write.size( ), []( auto c ) {
				                                   return static_cast<Byte>( c );
			                                   } );
			return { sp.empty( ) ? IOOpStatus::Eof : IOOpStatus::Ok,
			         to_write.size( ) };
		}

		template<typename Byte>
		static constexpr IOOpResult get( value_type &sp, Byte &b ) {
			if( sp.empty( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto result = sp[0];
			sp = sp.subspan( 1 );
			b = static_cast<Byte>( result );
			return { sp.empty( ) ? IOOpStatus::Eof : IOOpStatus::Ok, 1 };
		}

		static constexpr std::size_t max_peek_size( value_type const &sp ) {
			return sp.size( );
		}

		template<typename Byte>
		static constexpr IOOpResult peek( value_type &sp, std::span<Byte> buff ) {
			if( sp.empty( ) ) {
				return { IOOpStatus::Eof, 0 };
			}
			auto const peek_size = std::min( sp.size( ), buff.size( ) );
			daw::algorithm::transform_n( sp.data( ), buff.data( ), peek_size,
			                             []( auto c ) {
				                             return static_cast<Byte>( c );
			                             } );
			return { peek_size == sp.size( ) ? IOOpStatus::Eof : IOOpStatus::Ok,
			         peek_size };
		}
	};
} // namespace daw::io
