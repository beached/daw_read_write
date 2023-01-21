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
#include <string_view>

namespace daw::io {
	template<typename CharT, daw::string_view_bounds_type BT>
	struct ReadableInput<daw::basic_string_view<CharT, BT>> {
		static_assert( daw::traits::is_one_of_v<CharT, std::byte, char> );
		using value_type = daw::basic_string_view<CharT, BT>;

		template<typename Byte>
		static constexpr IOOpResult read( value_type &sv, std::span<Byte> buff ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			if( sv.empty( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto const to_write = sv.pop_front( buff.size( ) );
			(void)daw::algorithm::transform_n( to_write.data( ), buff.data( ),
			                                   to_write.size( ), []( auto c ) {
				                                   return static_cast<Byte>( c );
			                                   } );
			return { sv.empty( ) ? IOOpStatus::Eof : IOOpStatus::Ok,
			         to_write.size( ) };
		}

		template<typename Byte>
		static constexpr IOOpResult get( value_type &sv, Byte &b ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			if( sv.empty( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto result = sv.pop_front( );
			b = static_cast<Byte>( result );
			return { sv.empty( ) ? IOOpStatus::Eof : IOOpStatus::Ok, 1 };
		}
	};

	template<typename CharT, typename Traits>
	struct ReadableInput<std::basic_string_view<CharT, Traits>> {
		static_assert( daw::traits::is_one_of_v<CharT, std::byte, char> );
		using value_type = std::basic_string_view<CharT, Traits>;

		template<typename Byte>
		static constexpr IOOpResult read( value_type &sv, std::span<Byte> buff ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			if( sv.empty( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto const sz = std::min( sv.size( ), buff.size( ) );
			auto const to_write = sv.substr( 0, sz );
			sv.remove_prefix( sz );
			(void)daw::algorithm::transform_n( to_write.data( ), buff.data( ),
			                                   to_write.size( ), []( auto c ) {
				                                   return static_cast<Byte>( c );
			                                   } );
			return { sv.empty( ) ? IOOpStatus::Eof : IOOpStatus::Ok, sz };
		}

		template<typename Byte>
		static constexpr IOOpResult get( value_type &sv, Byte &b ) {
			static_assert( daw::traits::is_one_of_v<Byte, std::byte, char> );
			if( sv.empty( ) ) {
				return { IOOpStatus::Error, 0 };
			}
			auto result = sv.front( );
			sv.remove_prefix( 1 );
			b = static_cast<Byte>( result );
			return { sv.empty( ) ? IOOpStatus::Eof : IOOpStatus::Ok, 1 };
		}
	};
} // namespace daw::io
