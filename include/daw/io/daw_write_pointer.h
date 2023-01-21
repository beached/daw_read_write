// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_write_base.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <numeric>
#include <span>

namespace daw::io {
	template<typename Byte>
	struct WritableOutput<Byte *> {
		static_assert( sizeof( Byte ) == 1 );
		static_assert( not std::is_const_v<Byte> );
		[[nodiscard]] static inline IOOpResult write( Byte *&ptr,
		                                              daw::string_view sv ) {
			ptr =
			  daw::algorithm::transform_n( sv.data( ), ptr, sv.size( ), []( auto b ) {
				  return static_cast<Byte>( b );
			  } ).output;
			return { IOOpStatus::Ok, sv.size( ) };
		}

		[[nodiscard]] static inline IOOpResult
		write( Byte *&ptr, std::initializer_list<daw::string_view> svs ) {
			std::size_t written = 0;
			for( daw::string_view const &sv : svs ) {
				auto const r = WritableOutput::write( ptr, sv );
				if( r.status != IOOpStatus::Ok ) {
					return { r.status, r.count + written };
				}
				written += r.count;
			}
			return { IOOpStatus::Ok, written };
		}

		[[nodiscard]] static inline IOOpResult
		write( Byte *&ptr, std::span<std::byte const> sp ) {
			ptr =
			  daw::algorithm::transform_n( sp.data( ), ptr, sp.size( ), []( auto b ) {
				  return static_cast<Byte>( b );
			  } ).output;
			return { IOOpStatus::Ok, sp.size( ) };
		}

		static constexpr IOOpResult
		write( Byte *&ptr, std::initializer_list<std::span<std::byte const>> sps ) {
			std::size_t written = 0;
			for( std::span<std::byte const> const &sp : sps ) {
				auto const r = WritableOutput::write( ptr, sp );
				if( r.status != IOOpStatus::Ok ) {
					return { r.status, r.count + written };
				}
				written += r.count;
			}
			return { IOOpStatus::Ok, written };
		}
	};
} // namespace daw::io
