// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_read_write
//

#pragma once

#include "daw_write_base.h"
#include "daw_write_to_buffer.h"

#include <daw/daw_algorithm.h>
#include <daw/daw_string_view.h>

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <numeric>
#include <span>
#include <string>
#include <string_view>

namespace daw::io {
	template<typename CharT, typename Allocator>
	struct WritableOutput<std::basic_string<CharT, Allocator>> {
		using value_type = std::basic_string<CharT, Allocator>;

		static DAW_CPP20_CX_ALLOC IOOpResult write( value_type &s,
		                                            daw::string_view sv ) {
			if DAW_CPP20_CX_ALLOC( std::is_same_v<CharT, char> ) {
				s.append( static_cast<std::string_view>( sv ) );
			} else {
				auto const idx_first = s.size( );
				s.resize( s.size( ) + idx_first );
				(void)io_details::write_to_buffer( s.data( ) + idx_first, sv );
			}
			return { IOOpStatus::Ok, sv.size( ) };
		}

		static DAW_CPP20_CX_ALLOC IOOpResult
		write( value_type &s, std::initializer_list<daw::string_view> svs ) {
			auto const idx_first = s.size( );
			auto const total_to_add =
			  std::accumulate( svs.begin( ), svs.end( ), std::size_t{ 0 },
			                   []( std::size_t sz, daw::string_view sv ) {
				                   return sz + sv.size( );
			                   } );
			auto const new_size = idx_first + total_to_add;
			s.resize( new_size );
			auto out_ptr =
			  std::next( s.data( ), static_cast<std::ptrdiff_t>( idx_first ) );
			for( daw::string_view const &sv : svs ) {
				out_ptr = io_details::write_to_buffer( out_ptr, sv );
			}
			return { IOOpStatus::Ok, total_to_add };
		}

		static DAW_CPP20_CX_ALLOC IOOpResult
		write( value_type &s, std::span<std::byte const> sp ) {
			auto const idx_first = s.size( );
			s.resize( s.size( ) + idx_first );
			(void)io_details::write_to_buffer(
			  std::next( s.data( ), static_cast<std::ptrdiff_t>( idx_first ) ), sp );
			return { IOOpStatus::Ok, sp.size( ) };
		}

		static DAW_CPP20_CX_ALLOC IOOpResult write(
		  value_type &s, std::initializer_list<std::span<std::byte const>> sps ) {
			auto const idx_first = s.size( );
			auto const total_to_add =
			  std::accumulate( sps.begin( ), sps.end( ), std::size_t{ 0 },
			                   []( std::size_t sz, auto sp ) {
				                   return sz + sp.size( );
			                   } );
			auto const new_size = idx_first + total_to_add;
			s.resize( new_size );
			auto out_ptr =
			  std::next( s.data( ), static_cast<std::ptrdiff_t>( idx_first ) );
			for( auto const &sp : sps ) {
				out_ptr = io_details::write_to_buffer( out_ptr, sp );
			}
			return { IOOpStatus::Ok, total_to_add };
		}

		static DAW_CPP20_CX_ALLOC IOOpResult put( value_type &writer, char c ) {
			writer += static_cast<CharT>( c );
			return { IOOpStatus::Ok, 1 };
		}

		static DAW_CPP20_CX_ALLOC IOOpResult put( value_type &writer,
		                                          std::byte b ) {
			writer += static_cast<CharT>( b );
			return { IOOpStatus::Ok, 1 };
		}
	};
} // namespace daw::io
